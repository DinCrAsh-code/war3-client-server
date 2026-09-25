//============================================================================
//  0x6F3B18A0/0x6F3B1870 - the JASS script-compile-error message formatter
//  and its lookup table.  0x6F3B18A0 is passed to the compile driver
//  (sub_6F458900, see jscriptbin3_scriptloaderthunks.cpp's own header) as a
//  callback function pointer - it is what turns a raw compiler failure into
//  the `"<detail> (<base message>)"` string the engine actually reports.
//
//  Confirmed NOT `this`-based despite the leading register argument: `ecx`
//  is copied straight into `edi` and never dereferenced - it is used only
//  as the *second* `%s` substitution in the final `"%s (%s)"` format, so it
//  is a plain `const char*` (the base/primary error message), not an
//  object pointer.  `edx` is a genuine second register argument (an
//  unsigned key/category, `HASHKEY`-shaped) fed straight into
//  0x6F3B1870's own linear table search with no spill to a stack slot in
//  between - an ordinary MSVC `__fastcall(ecx, edx)` two-register
//  convention, not one of the WPO-only conventions
//  `docs/msvc-vc8-idioms.md` catalogues, so both are reproducible from
//  normal C++.
//
//  `retn 0Ch` (3 stack dwords) demands 3 stack parameters be declared for
//  the ABI to come out right, but only two of them (`arg_4`/`arg_8`) are
//  ever read in this body - `arg_0` is pushed and cleaned up by every
//  caller but never touched here.  Declared and named `unusedArg0` rather
//  than invented a role for it; it is very likely read by 0x6F458900's own
//  other error-callback family members this closure does not reach.
//
//  0x6F3B1870 does a linear, 0-terminated search of a 3-dword-stride table
//  (`dword_6FA79CF0` - key/kind/format) for `key == edx`, defaulting to the
//  table's own first (sentinel) entry when nothing matches or the table is
//  itself empty.  Not previously reconstructed; no `funcmap.py`/
//  `agent_worktrees` entry gave the table any name, so its declared type
//  and its `extern` array are new here, from the disassembly's own stride
//  and field-width evidence alone - the table's *contents* (which keys map
//  to which of the three `kind`s, and what each format string actually
//  says) are not recoverable from code alone and are not claimed.
//
//  The `kind` selector (0/1/2, `[entry+4]`) picks which of three
//  `Storm_578` substitution orders builds the "extra" detail string into a
//  0x400-byte stack buffer: kind 0 takes one substitution (`arg_4`); kind 1
//  takes two, `arg_8` then `arg_4`; kind 2 takes the same two the other way
//  round, `arg_4` then `arg_8` - confirmed from each case's own push order,
//  not guessed from symmetry. Any other kind value (the table's own broken
//  invariant, or a corrupt/absent entry) falls back to the literal string
//  "bad 'extra' enum" via `Storm_501` instead of formatting anything.
//  Either way, the result is combined with the base message via the fixed
//  `"%s (%s)"` format into a second 0x400-byte buffer and that call's own
//  return value (an `int`, whatever `Storm_578` returns - never itself
//  read as a pointer anywhere in this body) is this function's own return
//  value.  Nothing in this body reads the second buffer's contents again
//  after that call - if the compile driver actually reports the formatted
//  string rather than just this length/result code, it does so by a route
//  outside this function, most likely reading it back through a pointer
//  0x458900's own closure holds, not shown here.
//
//  This function's stack carries the game's own `/GS` cookie
//  (`dword_6FAAE140 xor esp`, `sub_6F7E1059` at the tail) around its two
//  0x400-byte buffers - the same permanent ceiling
//  Jass/jassscriptfileloadfallback.cpp's own header already documents for
//  its sibling `sub_6F3B1970`: this build is fixed `/GS-`
//  (`pipeline/CLAUDE.md`), so no reconstruction can ever emit that
//  cookie/check pair, however faithfully the rest of the body matches.
//  `DIFFERS`, not a transcription defect to keep chasing.
//============================================================================
#include "crtsec.h"   // g_securityCookie / SecurityCheckCookie - documented only, see above

int Storm_578(char* dest, unsigned int size, const char* format, ...);
void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);

//  dword_6FA79CF0 - 0-key-terminated, 3-dword-stride.
struct SScriptErrorExtraEntry
{
    unsigned int key;
    unsigned int kind;      // 0/1/2 selects the Storm_578 substitution order below
    const char*  format;
};
extern const SScriptErrorExtraEntry g_scriptErrorExtraTable[];   // dword_6FA79CF0

//  0x6F3B1870 - `retn 0`, `__fastcall(unused ecx, key in edx)`.  Linear
//  0-terminated search; returns the sentinel (index 0) entry both when the
//  table is itself empty and when nothing matches, exactly as the shipped
//  loop does (both paths land on the same tail instructions).
static const SScriptErrorExtraEntry* __fastcall LookupScriptErrorExtraEntry(void*, unsigned int key)
{
    unsigned int index = 0;
    unsigned int candidateKey = g_scriptErrorExtraTable[0].key;

    if (candidateKey != 0)
    {
        for (;;)
        {
            if (candidateKey == key)
                break;
            index++;
            candidateKey = g_scriptErrorExtraTable[index].key;
            if (candidateKey == 0)
                break;
        }
    }

    return &g_scriptErrorExtraTable[index];
}

//  0x6F3B18A0 - `retn 0Ch`, `__fastcall(baseMessage in ecx, extraKey in
//  edx, 3 stack args - only the 2nd/3rd read)`.
int __fastcall FormatScriptCompileErrorMessage(
    const char* baseMessage, unsigned int extraKey,
    void* /*unusedArg0*/, const char* detail1, const char* detail2)
{
    char extra[0x400];
    char message[0x400];

    const SScriptErrorExtraEntry* entry = LookupScriptErrorExtraEntry(0, extraKey);

    switch (entry->kind)
    {
    case 0:
        Storm_578(extra, sizeof(extra), entry->format, detail1);
        break;
    case 1:
        Storm_578(extra, sizeof(extra), entry->format, detail2, detail1);
        break;
    case 2:
        Storm_578(extra, sizeof(extra), entry->format, detail1, detail2);
        break;
    default:
        Storm_501(extra, "bad 'extra' enum", 0x7FFFFFFF);
        break;
    }

    return Storm_578(message, sizeof(message), "%s (%s)", extra, baseMessage);
}
