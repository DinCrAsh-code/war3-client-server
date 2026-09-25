//============================================================================
//  Two direct callees of SModelInstanceNotify::RefreshDrawState
//  (0x6F50D8C0, spritesethidden.cpp) - a rendering-side walk kept as a naked
//  thunk there because it is "out of scope" for that target.  These two are
//  small enough, and readable enough off the disassembly alone, to
//  reconstruct on their own even though the object they operate on is only
//  known through this one caller.
//
//  Both are reached with `ecx` = the same pointer RefreshDrawState itself
//  loads out of its own `this+0x98` before either call, so they are two
//  probes on one object - but nothing else in this closure names that
//  object's class, so each function below gets its own minimal struct that
//  only names the offsets it actually touches, the same "offsets are exact,
//  the surrounding layout is not" choice every other probe struct in this
//  tree makes.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F511920 - set or clear bit 0x8 of the object's own +0x54 dword
//  depending on whether `enable` is non-zero.  `__fastcall(void*, int)`:
//  `edx` is tested before anything is read off the stack, and the function
//  returns with nothing on the stack (`retn` with no operand).
//----------------------------------------------------------------------------
struct SDrawFlagHolder
{
    char         m_reserved00[0x54];
    unsigned int m_flags54;   // +0x54 - bit 0x8 toggled here
};

void __fastcall SetDrawFlag8(SDrawFlagHolder* self, int enable)
{
    if (enable != 0)
        self->m_flags54 |= 8;
    else
        self->m_flags54 &= ~8u;
}

//----------------------------------------------------------------------------
//  0x6F514700 - decide whether track `index` should draw.
//
//  Three arrays hang off the same object:
//    +0x44 -> +0x58   a per-track "kind" byte array, stride 0xE4, read at
//                      +0xE0 of the `index`'th element;
//    +0x78            a float array, stride 0x38, whose +0x34 float is
//                      compared against 0.0f for `index`;
//    +0x34            a byte array, stride 0x1C, indexed by the *kind byte*
//                      itself (not by `index`) and tested at +0x18.
//
//  The float gate comes first and is the early-out: the shipped code loads
//  0.0f into st(0) and does a single-operand `fcomp` against the +0x38-stride
//  float, so the comparison is `0.0f > track[index]` (docs/msvc-vc8-idioms.md's
//  `test ah,5` / `jp` pair - NaN-safe `>`, and the branch here is to the
//  "return 0" path, i.e. the *negation* of that condition survives).  Only
//  once that float is negative does the kind byte matter at all: 0xFF means
//  "always draw"; anything else re-indexes the third array by the kind byte
//  and tests one bit of it.
//
//  `__fastcall(void*, int)`: `edx` is used as `index` before anything comes
//  off the stack, and the function returns with nothing on the stack.
//----------------------------------------------------------------------------
struct STrackDrawGate
{
    char          m_reserved00[0x34];
    char*         m_pKindLimits;   // +0x34 - stride 0x1C, indexed by kind byte
    char          m_reserved38[0x0C];
    char*         m_pTrackFloats;  // +0x44 -> +0x58 is the real base, see below
    char          m_reserved48[0x30];
    char*         m_pRange;        // +0x78 - stride 0x38, +0x34 is the float
};

struct STrackFloatArrayHeader
{
    char  m_reserved00[0x58];
    char* m_pBase;   // +0x58 - stride 0xE4, +0xE0 is the kind byte
};

int __fastcall ShouldDrawTrack(STrackDrawGate* self, int index)
{
    STrackFloatArrayHeader* header = (STrackFloatArrayHeader*)self->m_pTrackFloats;
    unsigned int kind = *(unsigned char*)(header->m_pBase + index * 0xE4 + 0xE0);

    float range = *(float*)(self->m_pRange + index * 0x38 + 0x34);
    if (range > 0.0f)
    {
        if (kind == 0xFF)
            return 1;

        if (self->m_pKindLimits[kind * 0x1C + 0x18] & 1)
            return 1;
    }

    return 0;
}
