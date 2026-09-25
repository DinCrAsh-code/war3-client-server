//============================================================================
//  BFS-depth-9 leaves out of CItem's vtable slots 32/104
//  (asm/processed/CItem__Method_0x80_0x6F2B6130_calltree_asm.md), continuing
//  the same frontier as Misc/citemclosure_terrain_gates.cpp and its
//  siblings.  Two self-contained functions: a fourth ComputeChunk
//  instantiation (Containers/tsarray.inl) over a 32-byte element none of
//  this closure's dumps names, and a flat field-zeroing constructor with no
//  vtable, no Storm call and no class identity to guess at.
//============================================================================
#include "storm.h"
#include "tsarray.inl"

//----------------------------------------------------------------------------
//  0x6F73D230 - TSGrowableArray<T>::ComputeChunk for a 32-byte T
//  (`256 / 32 = 8`, the literal `8` this dump stores into `[this+0x0C]` for
//  an input at or past the cap).  No caller in this closure and no owning
//  array is named by anything reachable here, so the element stays an
//  anonymous 32-byte placeholder - the same "sizeof is all ComputeChunk
//  ever needs" situation citemclosure_terrain_hashtrio.cpp's own trio is in
//  for a 12-byte element.
//----------------------------------------------------------------------------
struct SPlaceholder32Byte { char m_reserved00[0x20]; };

template unsigned int TSGrowableArray<SPlaceholder32Byte>::ComputeChunk(unsigned int);

//----------------------------------------------------------------------------
//  0x6F74A780 - zero 33 dwords of a caller-supplied object at fixed offsets
//  spanning 0x00-0x90h, skipping one dword every 0x14 bytes from 0x30
//  onward (0x30/0x44/0x58/0x6C/0x80 all stay untouched).  That is a plain,
//  regular record: 12 leading dwords (0x00-0x2C) followed by five 0x14-byte
//  sub-records whose own first dword this constructor leaves alone -
//  consistent with that first dword being a sub-object with its own
//  constructor this closure never reaches (a vtable pointer or a handle
//  stamped elsewhere), not a field this function forgot.  No vtable call,
//  no Storm allocation anywhere in the body, and no caller in this closure
//  names the owning class, so the offsets are reproduced directly rather
//  than through an invented struct - `char m_reservedNN[]` would just be
//  the same offsets with extra ceremony given nothing here is ever read
//  back.  `retn` (`__thiscall`, no stack argument - `this` in `ecx` only).
//  A plain free function cannot be declared `__thiscall` (MSVC: "can only be
//  used on native member functions"), so this is spelled as a member of an
//  otherwise-empty struct, the same way this repo's other `this`-only, no-
//  named-owner leaves are.
//----------------------------------------------------------------------------
struct SUnknownFieldRecordArray
{
    void ZeroInit();
};

void SUnknownFieldRecordArray::ZeroInit()
{
    char* p = (char*)this;

    *(int*)(p + 0x00) = 0; *(int*)(p + 0x04) = 0; *(int*)(p + 0x08) = 0;
    *(int*)(p + 0x0C) = 0; *(int*)(p + 0x10) = 0; *(int*)(p + 0x14) = 0;
    *(int*)(p + 0x18) = 0; *(int*)(p + 0x1C) = 0; *(int*)(p + 0x20) = 0;
    *(int*)(p + 0x24) = 0; *(int*)(p + 0x28) = 0; *(int*)(p + 0x2C) = 0;

    *(int*)(p + 0x34) = 0; *(int*)(p + 0x38) = 0; *(int*)(p + 0x3C) = 0;
    *(int*)(p + 0x40) = 0;

    *(int*)(p + 0x48) = 0; *(int*)(p + 0x4C) = 0; *(int*)(p + 0x50) = 0;
    *(int*)(p + 0x54) = 0;

    *(int*)(p + 0x5C) = 0; *(int*)(p + 0x60) = 0; *(int*)(p + 0x64) = 0;
    *(int*)(p + 0x68) = 0;

    *(int*)(p + 0x70) = 0; *(int*)(p + 0x74) = 0; *(int*)(p + 0x78) = 0;
    *(int*)(p + 0x7C) = 0;

    *(int*)(p + 0x84) = 0; *(int*)(p + 0x88) = 0; *(int*)(p + 0x8C) = 0;
    *(int*)(p + 0x90) = 0;
}
