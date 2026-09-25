//============================================================================
//  TSGrowableArray<RCString> - a by-value RCString array (element size 16,
//  the `shl esi,4` this closure's own sub_6F45D720/sub_6F44C5A0 share).
//  0x6F4493F0 (ComputeChunk, cap 0x10 = 256/16) is a plain leaf, same shape
//  as every other ComputeChunk instantiation in this closure, and real.
//
//  0x6F44C5A0 (SetAlloc) and 0x6F44DBE0 (SetCount) are not: both open with
//  the `__except_handler4`/GS-cookie SEH frame this build's fixed
//  `/GS- /EHs-c-` cannot reproduce (docs/msvc-vc8-idioms.md, and every
//  `sub_6F27...`-class DIFFERS entry in this file that cites it) - RCString
//  has a real destructor, so growing or shrinking this array needs the
//  exception safety a trivial pointer element never does.  Kept redirects.
//============================================================================
//  RCString's own size (`??_7RCString@@6B@` stamped in sub_6F44C5A0's own
//  body, an vtable-pointer/2-dword-plus-padding 16-byte object) - not
//  reconstructed further than its size here, since both real callers of
//  this array's growth are themselves redirects.
struct SJassRCStringElem { unsigned int m_reserved[4]; };

#define TSARRAY_TYPENAME ".AURCString@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<SJassRCStringElem>::ComputeChunk(unsigned int);

#define JASS_RCSTRING_THUNK(addr) \
    {                              \
        __asm { mov eax, addr }   \
        __asm { jmp eax }          \
    }

//  0x6F44C5A0 - SetAlloc, `retn 4`.
__declspec(naked) void __fastcall SetAllocRCStringArray(unsigned int)
    JASS_RCSTRING_THUNK(06F44C5A0h)

//  0x6F44DBE0 - SetCount, `retn 4`.
__declspec(naked) void __fastcall SetCountRCStringArray(unsigned int)
    JASS_RCSTRING_THUNK(06F44DBE0h)
