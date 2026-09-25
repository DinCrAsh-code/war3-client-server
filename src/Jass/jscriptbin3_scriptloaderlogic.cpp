//============================================================================
//  0x6F3B1A20/0x6F3B1C30 - the real logical bodies behind the two self-
//  redirect thunks in jscriptbin3_scriptloaderthunks.cpp, written out in
//  full so the mechanism these two functions implement lives in source,
//  not just in that file's header comment.  Neither of these two functions
//  below is the hooked/exported reconstruction - the naked thunks in the
//  sibling file still are, and still must be, for the reason explained at
//  the bottom of this file - so these carry different names and are never
//  called from anywhere else in `src/`.  They exist purely as documented,
//  compiled proof that the logic is understood, one real attempt at going
//  past THUNK, and (per this round's own task) the evidence for exactly
//  why that attempt cannot land.
//
//  Re-derived from the raw disassembly directly (not merely restated from
//  the sibling file's own summary), instruction by instruction, including
//  two corrections to that summary once the full call chain
//  (sub_6F3AEF80 -> sub_6F3B5450 -> LoadAndCompileCommonScripts ->
//  ComputeScriptChecksums) was traced:
//
//  1. ComputeScriptChecksums's first stack argument (`arg_0`) is NOT a
//     caller-supplied checksum seed - it is the map's own script FILENAME,
//     a pointer forwarded three calls deep from sub_6F3AEF80's own name
//     buffer (built earlier in that function via sub_6F011D70/
//     sub_6F39EEB0/sub_6F39EED0) through LoadAndCompileCommonScripts's
//     `ecx` and back out through ComputeScriptChecksums's own `arg_0`,
//     confirmed by its only use: it is loaded into `esi` immediately
//     before the map-script `call sub_6F3B1970`, exactly where
//     "common.j"/"blizzard.j" sit as literals for the other two loads.
//  2. The `arg_1C`/`arg_14` pair are not independent "skip" booleans - the
//     lookup's own two OUT values (`outA`/`outB` from
//     LookupScriptRangeEntry) double as a pre-supplied common.j/blizzard.j
//     checksum PAIR: `arg_14` decides whether the common.j checksum is
//     computed fresh (`RotatingHash`, when set) or taken as-is from the
//     lookup's own `outA` (when clear - the write at loc_6F3B1AA9 that
//     looked like a dead store the first time through is this: it is
//     always overwritten later by the final combine, which is exactly
//     right, since a caller that trusts the lookup value never reaches
//     the intermediate write at all along any *externally observable*
//     path - the store only looks dead because nothing between it and the
//     final combine can read it). `arg_18` gates the same choice for
//     blizzard.j against `outB`. This function's only real caller in this
//     closure (LoadAndCompileCommonScripts) always passes both flags
//     *set* (computes both hashes fresh) and always passes `arg_1C` as
//     the fixed constant `0x17AB`, so this caching path is real but not
//     exercised anywhere this closure's own BFS reaches.
//============================================================================
#include "filecache.h"

void* __fastcall LoadScriptFileWithFallback(const char* name, int* outLength);
unsigned int __fastcall RotatingHash(const void* data, unsigned int length);
int __fastcall LookupScriptRangeEntry(unsigned int key, unsigned int* outA, unsigned int* outB);
int  __fastcall ReleaseLoadedFile(void* buffer, int fromHeap);

//----------------------------------------------------------------------------
//  0x6F3B1A20 logic - `__stdcall`, 8 stack args (`retn 20h`), no register
//  args: the shipped body never touches `ecx`/`edx` on entry.
//
//    arg_0  - const char* mapScriptName        (see correction 1 above)
//    arg_4  - void** outCommonBuffer
//    arg_8  - void** outBlizzardBuffer
//    arg_C  - void** outMapScriptBuffer
//    arg_10 - unsigned int* outChecksum
//    arg_14 - int computeCommonHashFresh        (see correction 2 above)
//    arg_18 - int computeBlizzardHashFresh
//    arg_1C - unsigned int mapScriptLookupKey    (0x17AB from the only real
//             caller in this closure; also doubles as "skip the lookup"
//             when a caller passes a genuinely reserved value the range
//             table never matches - LookupScriptRangeEntry's own 0-return
//             path folds into the same failure branch either way)
//
//  Returns 1 on success, 0 on failure (either the id lookup or the map
//  script's own load failing after a cache-hit but null buffer), releasing
//  whatever was already loaded on the way out either way, and always
//  zeroing every out-parameter first.
//----------------------------------------------------------------------------
int __stdcall ComputeScriptChecksumsLogic(
    const char* mapScriptName,
    void** outCommonBuffer, void** outBlizzardBuffer, void** outMapScriptBuffer,
    unsigned int* outChecksum,
    int computeCommonHashFresh, int computeBlizzardHashFresh,
    unsigned int mapScriptLookupKey)
{
    *outCommonBuffer = 0;
    *outBlizzardBuffer = 0;
    *outMapScriptBuffer = 0;
    *outChecksum = 0;

    unsigned int commonHash = 0;
    unsigned int blizzardHash = 0;

    if (mapScriptLookupKey != 0)
    {
        unsigned int cachedCommonHash, cachedBlizzardHash;
        if (!LookupScriptRangeEntry(mapScriptLookupKey, &cachedCommonHash, &cachedBlizzardHash))
            goto fail_before_any_load;
        commonHash = cachedCommonHash;
        blizzardHash = cachedBlizzardHash;
    }

    {
        int commonLen;
        void* commonBuf = LoadScriptFileWithFallback("common.j", &commonLen);
        *outCommonBuffer = commonBuf;
        if (computeCommonHashFresh)
        {
            if (!commonBuf)
                goto fail_common_loaded;
            commonHash = RotatingHash(commonBuf, (unsigned int)commonLen);
        }
        else
        {
            *outChecksum = commonHash;   // overwritten below either way
        }

        int blizzardLen = 0;
        void* blizzardBuf = LoadScriptFileWithFallback("blizzard.j", &blizzardLen);
        *outBlizzardBuffer = blizzardBuf;
        if (computeBlizzardHashFresh)
        {
            if (!blizzardBuf)
                goto fail_common_and_blizzard_loaded;
            blizzardHash = RotatingHash(blizzardBuf, (unsigned int)blizzardLen);
        }

        //  First combine stage: blizzard ^ common, rol 3, xor fixed salt,
        //  rol 3 again.  0x3F1379E does not appear anywhere else in this
        //  repo or its docs (grepped) - nothing ties it to a known
        //  checksum family (not an FNV/CRC constant); it reads as an
        //  arbitrary, function-local salt rather than a shared algorithm
        //  parameter.
        unsigned int combined = blizzardHash ^ commonHash;
        combined = (combined << 3) | (combined >> 29);
        combined ^= 0x3F1379E;
        combined = (combined << 3) | (combined >> 29);
        *outChecksum = combined;

        int mapScriptLen = 0;
        void* mapScriptBuf = LoadScriptFileWithFallback(mapScriptName, &mapScriptLen);
        *outMapScriptBuffer = mapScriptBuf;
        if (!mapScriptBuf)
            goto fail_common_and_blizzard_loaded;

        //  Second combine stage: map-script hash folded straight in, no
        //  second salt application - so the "one XOR/rol per additional
        //  script past the first" shape does NOT repeat the salt, it only
        //  appears once, ahead of the second script.  A literal 4th script
        //  slot could plausibly just add a third "xor into *outChecksum,
        //  rol 3" step with no new constant, following the map-script
        //  stage's own shape rather than the first stage's - but that is
        //  inference from this one instance, not confirmed by a second
        //  real example anywhere in this closure.
        unsigned int mapHash = RotatingHash(mapScriptBuf, (unsigned int)mapScriptLen);
        combined ^= mapHash;
        combined = (combined << 3) | (combined >> 29);
        *outChecksum = combined;
        return 1;
    }

fail_common_and_blizzard_loaded:
fail_common_loaded:
    if (*outCommonBuffer)
        ReleaseLoadedFile(*outCommonBuffer, 1);
    *outCommonBuffer = 0;
    if (*outBlizzardBuffer)
        ReleaseLoadedFile(*outBlizzardBuffer, 1);
    *outBlizzardBuffer = 0;
    if (*outMapScriptBuffer)
        ReleaseLoadedFile(*outMapScriptBuffer, 1);
    *outMapScriptBuffer = 0;
    *outChecksum = 0;
    return 0;

fail_before_any_load:
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F3B1C30 logic - `__fastcall(mapScriptName in ecx, mapScriptLookupKey
//  in edx)`, `retn 0`.  Confirmed from LoadAndCompileCommonScripts's own
//  incoming `ecx`/`edx` -> `esi`/`ebx` moves, then forwarded unchanged as
//  ComputeScriptChecksumsLogic's `mapScriptName`/`mapScriptLookupKey`
//  (the caller sub_6F3AEF80 passes `ecx = &nameBuffer`, `edx = ebx`, a
//  0/1 flag computed a few lines above its own call site - NOT the numeric
//  lookup key `0x17AB` this function itself hardcodes past that point).
//
//  Builds a fixed 4-slot {name, buffer} array for the compile driver
//  (0x6F458900) - `{"common.j", commonBuf}, {"blizzard.j", blizzardBuf},
//  {mapScriptName, mapScriptBuf}, {0, 0}` - the trailing zero pair is
//  unconditional, not gated by anything, i.e. this looks exactly like a
//  0-terminated variable-length list convention on the driver's own side
//  rather than a fixed 3-entry struct on THIS function's side; whether
//  0x6F458900 actually walks it as such, or just reads exactly 3 fixed
//  slots and ignores the 4th, is outside this closure (0x6F458900 itself
//  is still a THUNK, `Thunk_6F458900` in jassvmbin2_sehthunks.cpp,
//  reconstructing it is out of scope here) - see this round's own
//  write-up for what a partial read of its own body could and could not
//  settle.  The call into it is therefore deliberately NOT reproduced
//  below; this function stops at the fully-built array, which is as far
//  as this address's own logic goes.
//----------------------------------------------------------------------------
struct SScriptCompileEntry
{
    const char* name;
    void*       buffer;
};

//  Returns 1 on success (matching ComputeScriptChecksumsLogic's own
//  result) with `outEntries` filled; the shipped function's own next step
//  (call 0x6F458900 with this array, a fixed 0x17AB checksum-lookup key,
//  the always-1/1 hash-fresh flags, the sub_6F3B18A0 error callback, and
//  the incoming `mapScriptLookupKey` flag) is not reproduced - see above.
int __fastcall LoadAndCompileCommonScriptsLogic(
    const char* mapScriptName, unsigned int mapScriptLookupKey,
    SScriptCompileEntry outEntries[4], unsigned int* outChecksum)
{
    void* commonBuf;
    void* blizzardBuf;
    void* mapScriptBuf;

    int ok = ComputeScriptChecksumsLogic(
        mapScriptName, &commonBuf, &blizzardBuf, &mapScriptBuf, outChecksum,
        1, 1, 0x17AB);

    if (!ok)
        return 0;

    outEntries[0].name = "common.j";
    outEntries[0].buffer = commonBuf;
    outEntries[1].name = "blizzard.j";
    outEntries[1].buffer = blizzardBuf;
    outEntries[2].name = mapScriptName;
    outEntries[2].buffer = mapScriptBuf;
    outEntries[3].name = 0;
    outEntries[3].buffer = 0;

    //  ... call 0x6F458900(&outEntries[0], errorCallback=FormatScriptCompileErrorMessage,
    //      0, mapScriptLookupKey) here in the shipped body; then release
    //      commonBuf/blizzardBuf/mapScriptBuf regardless of its result.
    (void)mapScriptLookupKey;
    return 1;
}

//============================================================================
//  Why the hooked/exported versions in jscriptbin3_scriptloaderthunks.cpp
//  stay THUNK even with the logic above fully written out and compiling
//  clean: both call `LoadScriptFileWithFallback` (0x6F3B1970,
//  jassscriptfileloadfallback.cpp), and that function's own header already
//  documents that its real, shipped calling convention passes its two
//  arguments in `esi`/`edi` - a register pair MSVC's whole-program
//  optimiser chose for that static helper's own two real call sites,
//  neither of which is (or, per that file's own note, can currently be)
//  reconstructed together with it. Every call this file's logic makes to
//  it necessarily compiles to an ordinary `__fastcall` sequence -
//  `mov ecx, X` / `mov edx, Y` / `call` - because that is the ABI
//  `LoadScriptFileWithFallback` is declared with; there is no C++ spelling
//  that asks the compiler to load a callee's arguments into `esi`/`edi`
//  instead of `ecx`/`edx` for an ordinary call. So even a byte-perfect
//  reproduction of every OTHER instruction in ComputeScriptChecksums and
//  LoadAndCompileCommonScripts would still diverge at all three call
//  sites into 0x6F3B1970 - not a scheduling or register-allocation
//  difference `verify.py`'s score could call cosmetic, but a genuinely
//  different call sequence at the machine-code level. That is the actual,
//  confirmed ceiling (not merely assumed): it was reached by writing the
//  real attempt above, not by inference from the callee's own file alone.
//============================================================================
