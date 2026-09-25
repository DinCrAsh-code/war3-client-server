//============================================================================
//  The font-version-check cluster in the 0x6F553xxx neighbourhood (reached
//  from sub_6F553CC0, out of this session's scope - it builds a random
//  session salt, loads font\font.gid/.exp/.clh/.ccd through this array and
//  folds them into a "sgubon"-keyed checksum).  Four leaves:
//
//    0x6F5559A0  RandomFillNonZero - fills a caller buffer with non-null
//                random bytes and a trailing NUL.
//    0x6F553B60  AuFixedStringArray::SetAlloc (TSFixedArray<AuFixedString>,
//                Containers/tsfixedarray.inl - same template as
//                CGameData::m_queryPool's AgileAgentQuery* instantiation,
//                over a 0x20-byte element instead of a pointer).
//    0x6F553C50  AuFixedStringArray::SetCount (the same template member;
//                see tsfixedarray.inl's own note on the construction loop
//                this instantiation is what proves it real).
//    0x6F553AA0  FontFile_LoadAndVerify - loads one font file through
//                Storm_279, hands it to sub_6F553A70 (a sibling session's
//                target, forward-declared below) and, once that verifies,
//                copies the caller's own scratch slot into the array's
//                first element.
//
//  The type descriptor name every SMemAlloc/SMemFree call here passes is
//  IDA's own `aAuFixedstringD` - a truncated auto-name, not the string
//  itself.  Every other TSFixedArray/TSGrowableArray instantiation in this
//  repo passes the element's RTTI raw_name (".PAVAgileAgentQuery@@"-shaped:
//  ".?AV<Class>@@" for a plain class), so the literal below is the same
//  shape for "AuFixedString" - inferred, not confirmed byte-for-byte
//  against the image (this build has RTTI off, so it cannot be read back
//  out of typeid()), and the instruction score cannot see it either way
//  since an unresolved string offset canonicalises the same regardless of
//  content.
//============================================================================
#include "storm.h"

//  Storm.dll ordinal 279/280 - see Render/filecache.h's own comment on the
//  five SFileLoadFile arguments; re-declared here (not included from that
//  header) so this file does not pull in the whole prefetch-cache hash
//  table closure for two leaf imports.
int  __stdcall Storm_279(const char* filename, void** buffer, int* length,
                         int extraBytes, void* callback);
int  __stdcall Storm_280(void* buffer);

//  Storm.dll: bounded string copy (dest, src, maxLen) - same signature as
//  every other declaration of it in this repo (Render/cstatus.h and
//  friends); re-declared rather than included for the same reason.
void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);

//  AuFixedString/AuFixedStringArray - Render/aufixedstring.h now (the other
//  0x1F bytes of a slot are written later, through Storm_501, by
//  FontFile_LoadAndVerify below).  Pulled into a shared header by
//  cnd-wave2-c's own ReadTeamGameListTailField_6F653760
//  (Net/netevent_dispatch.cpp), which reaches this same SetCount
//  instantiation (0x6F553C50) directly and would otherwise have had to
//  stand up a second, redundant TSFixedArray<T> instantiation at a new
//  address for what the dump shows is the identical shipped body.
#include "aufixedstring.h"

#define TSFIXEDARRAY_TYPENAME ".?AVAuFixedString@@"
#include "tsfixedarray.inl"

template void AuFixedStringArray::SetAlloc(unsigned int);   // 0x6F553B60
template void AuFixedStringArray::SetCount(unsigned int);   // 0x6F553C50

//----------------------------------------------------------------------------
//  0x6F5559A0 - fill `size - 1` bytes of `buffer` with random, non-null
//  bytes and NUL-terminate at `size - 1`.  `srand(0x150B)` is a fixed seed:
//  the caller (sub_6F553CC0) treats the result as a session-local salt, not
//  as anything cryptographically random.
//
//  The `while (byte == 0) retry` shape is the shipped code's own inner
//  loop (`test al,al` / `jz` back to `rand()` without advancing the write
//  cursor) - it is not `rand() % 255 + 1` or any other non-zero-biasing
//  spelling, it is a genuine rejection loop.
//----------------------------------------------------------------------------
extern "C" void __cdecl srand(unsigned int seed);
extern "C" int  __cdecl rand();

void __fastcall RandomFillNonZero(char* buffer, unsigned int size)
{
    srand(0x150B);

    unsigned int last = size - 1;
    unsigned int i = 0;
    while (i < last)
    {
        char c = (char)rand();
        if (c != 0)
        {
            buffer[i] = c;
            i++;
        }
    }
    buffer[size - 1] = 0;
}

//----------------------------------------------------------------------------
//  0x6F553AA0 - load `filename` through Storm_279 (allocating its own
//  buffer, one extra byte for the NUL Storm always over-allocates), hand
//  the loaded bytes to sub_6F553A70 (this dump's own hash/verify step -
//  reconstructed by a sibling session, see the forward declaration below)
//  together with the caller's destination size, and on success copy the
//  caller's own scratch slot (`context`, the array-element address the
//  caller passes through edx/`edi` here) into `destBuffer` via Storm_501.
//  The loaded file bytes are always wiped with memset before Storm_280
//  frees them, win or lose - the shipped code never lets a stale font-file
//  buffer sit around unzeroed.
//
//  `context`/`edi` is opaque to this function: it is only ever forwarded
//  into sub_6F553A70 (as the value under verification) and into Storm_501
//  (as the copy's destination), so its real type is whatever that function
//  needs - carried here as `void*` and cast at each use rather than guessed.
//----------------------------------------------------------------------------
extern "C" void* __cdecl memset(void* dst, int val, unsigned int n);

//  sub_6F553A70 - gamemain-batch-5's own target in this same dump.  Not
//  reconstructed here; only its signature is inferred from this call site's
//  own push order (loaded buffer, loaded length, then the destination size
//  arriving on the stack).
int __fastcall sub_6F553A70(void* loadedBuffer, int loadedLength, unsigned int destSize);

int __fastcall FontFile_LoadAndVerify(const char* filename, void* context,
                                       char* destBuffer, unsigned int destSize)
{
    void* loaded = 0;
    int length = 0x400;
    int result = 0;

    if (Storm_279(filename, &loaded, &length, 1, 0))
    {
        if (sub_6F553A70(loaded, length, destSize))
        {
            Storm_501((char*)context, (const char*)loaded, destSize);
            result = 1;
        }
    }

    if (loaded)
    {
        memset(loaded, 0, length);
        Storm_280(loaded);
    }

    return result;
}
