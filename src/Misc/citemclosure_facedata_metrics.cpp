//============================================================================
//  A handful of self-contained leaves off the FACEDATA glyph-cache object -
//  CItem vtable slots 32/104 closure, batch K, depth 12-25+ (see
//  Misc/citemclosure_facedata_hashtable.cpp's own header for the hash table
//  living inside the same class, and Misc/citemclosure_facedata_release.cpp
//  for its release chain).  "FACEDATA" is not a guess: sub_6F7C85F0, right
//  above this address range and reached from the same closure, stamps
//  `??_7FACEDATA@@6B@` directly - the shipped mangled vtable symbol, ground
//  truth rather than inference.
//
//  Every function here is signed 16.16 fixed-point arithmetic (FixedMul's
//  own `cmp edx,10000h` identity check against 0x10000 = 1.0 in that format
//  is what pins the representation) or a plain intrusive doubly-linked-list
//  splice - none of it touches FACEDATA's own fields, so none of it needs a
//  struct for the object itself.
//============================================================================
#include "storm.h"

//----------------------------------------------------------------------------
//  0x6F7C93A0 - FACEDATA::FixedMul: signed 16.16 fixed-point multiply.
//  Two identities short-circuit the general path: a zero operand answers
//  zero outright, and multiplying by 0x10000 (1.0) answers the other
//  operand unchanged.  Otherwise the magnitudes are multiplied unsigned and
//  the sign reapplied at the end; small operands (|a|<=0x800, |b|<=0x100000)
//  take a rounded `imul`-then-`shr` fast path, and everything else falls
//  through to an unrounded halfword cross-multiply that computes the same
//  `(ua*ub)>>16` a different way, without adding 0x8000 first - the shipped
//  code really does not round in that branch, so this does not either.
//----------------------------------------------------------------------------
int __fastcall FACEDATA_FixedMul(int a, int b)
{
    int eax = a;
    if (eax == 0)
        goto done;
    if (b == 0x10000)
        goto done;

    {
        int sign = eax;
        eax = (eax >= 0) ? eax : -eax;
        sign ^= b;
        unsigned int ub = (b >= 0) ? (unsigned int)b : (unsigned int)(-b);
        unsigned int ua = (unsigned int)eax;

        unsigned int result;
        if (ua <= 0x800 && ub <= 0x100000)
        {
            //  Plain 32-bit `imul` (the product cannot overflow 32 bits at
            //  these magnitudes), rounded before the shift.
            result = (ua * ub + 0x8000) >> 16;
        }
        else
        {
            //  The general path: no rounding, and no 64-bit multiply -
            //  the shipped code cross-multiplies the two halfwords by
            //  hand instead, so this does too rather than let the
            //  compiler choose a `mul`/`shrd` pair that never appears in
            //  the target stream.
            unsigned int lo   = (ua & 0xFFFFu) * (ub & 0xFFFFu);
            unsigned int mid1 = (ub >> 16) * (ua & 0xFFFFu);
            unsigned int mid2 = (ua >> 16) * ub;
            result = (lo >> 16) + mid1 + mid2;
        }

        eax = (sign >= 0) ? (int)result : -(int)result;
    }
done:
    return eax;
}

//----------------------------------------------------------------------------
//  A 64-bit fixed value carried as two plain ints - the same shape and the
//  same Add/Divide members Misc/citemclosure_facedata_fixed64.cpp defines
//  out of line (its own header says why that file has to be separate from
//  this one).
//----------------------------------------------------------------------------
struct FACEDATA_Fixed64
{
    unsigned int lo;   // 0x00
    int          hi;   // 0x04
};

void __fastcall FACEDATA_Fixed64Add(FACEDATA_Fixed64* a, FACEDATA_Fixed64* b,
                                     FACEDATA_Fixed64* out);
int  __fastcall FACEDATA_Fixed64Divide(FACEDATA_Fixed64* v, int divisor);

//----------------------------------------------------------------------------
//  A plain intrusive doubly-linked list: the list object itself carries
//  `m_head`/`m_tail`, and every node carries `m_prev`/`m_next` at the same
//  two offsets - two distinct shapes at the identical layout, the way an
//  intrusive list with separate head/tail fields (rather than a
//  self-referencing sentinel node) usually is.
//----------------------------------------------------------------------------
struct SFaceList
{
    struct SFaceListNode* m_head;   // 0x00
    struct SFaceListNode* m_tail;   // 0x04
};

struct SFaceListNode
{
    SFaceListNode* m_prev;   // 0x00
    SFaceListNode* m_next;   // 0x04
};

//----------------------------------------------------------------------------
//  0x6F7CA8B0 - FACEDATA::LinkTail: append `node` (edx) to the tail of the
//  list `list` (ecx).
//----------------------------------------------------------------------------
//  `void*` rather than `SFaceList*`/`SFaceListNode*` - several other
//  translation units in this closure call this against their own local
//  view of the same objects, and MSVC bakes the parameter type into the
//  mangled name (see FACEDATA_AllocateAndZero's own note,
//  Misc/citemclosure_facedata_release.cpp, for the `link_check.py` failure
//  a mismatched re-declaration causes).
void __fastcall FACEDATA_LinkTail(void* listVoid, void* nodeVoid)
{
    SFaceList* list = (SFaceList*)listVoid;
    SFaceListNode* node = (SFaceListNode*)nodeVoid;
    SFaceListNode* oldTail = list->m_tail;

    node->m_next = 0;
    node->m_prev = oldTail;

    if (oldTail != 0)
    {
        oldTail->m_next = node;
        list->m_tail = node;
    }
    else
    {
        list->m_head = node;
        list->m_tail = node;
    }
}

//----------------------------------------------------------------------------
//  0x6F7CA8F0 - FACEDATA::Unlink: splice `node` (edx) out of the list
//  `list` (ecx), patching whichever of `list->m_head`/`list->m_tail` was
//  pointing at `node` when its own `m_prev`/`m_next` is null.
//----------------------------------------------------------------------------
//  `void*` rather than `SFaceList*`/`SFaceListNode*` - same note as
//  FACEDATA_LinkTail above.
void __fastcall FACEDATA_Unlink(void* listVoid, void* nodeVoid)
{
    SFaceList* list = (SFaceList*)listVoid;
    SFaceListNode* node = (SFaceListNode*)nodeVoid;
    SFaceListNode* prev = node->m_prev;
    SFaceListNode* next = node->m_next;

    if (prev != 0)
        prev->m_next = next;
    else
        list->m_head = next;

    if (next != 0)
        next->m_prev = prev;
    else
        list->m_tail = prev;
}

//----------------------------------------------------------------------------
//  0x6F7CA0F0 - FACEDATA::InitRecord: a plain field-by-field record
//  initialiser, `retn 8` for its two stack arguments.  `out` (the second
//  stack arg) gets `in`'s own vtable-ish first field copied into its own
//  +0x1C, the first stack argument stored at +0x04, `edx` (the caller's
//  ecx-adjacent register argument) stored at +0x00, and the remaining
//  fields zeroed - the shape of a fresh node about to be linked in.
//----------------------------------------------------------------------------
struct SFaceRecordInit
{
    void*        m_field00;   // 0x00
    void*        m_field04;   // 0x04
    void*        m_field08;   // 0x08
    char         m_reserved0C[0x08];
    void*        m_field14;   // 0x14
    void*        m_field18;   // 0x18
    void*        m_field1C;   // 0x1C
    void*        m_field20;   // 0x20
};

//----------------------------------------------------------------------------
//  0x6F7CB3F0 - FACEDATA::FixedDiv: signed 16.16 fixed-point divide,
//  `(a<<16)/b` computed at extended precision with half the divisor added
//  first (the classic round-to-nearest fixed-point division idiom, via
//  this file's own Fixed64Add/Fixed64Divide), falling back to a plain
//  32-bit `div` when `|a|<<16` does not overflow 32 bits, and 0x7FFFFFFF
//  on division by zero.
//----------------------------------------------------------------------------
int __fastcall FACEDATA_FixedDiv(int a, int b)
{
    int ua = a;
    int sign = ua;
    if (ua < 0)
        ua = -ua;
    sign ^= b;

    int ub = b;
    if (ub < 0)
        ub = -ub;

    if (ub == 0)
        return 0x7FFFFFFF;

    unsigned int shifted = (unsigned int)ua << 16;
    unsigned int hiPart = (unsigned int)ua >> 16;

    int result;
    if (hiPart == 0)
    {
        result = (int)(shifted / (unsigned int)ub);
    }
    else
    {
        FACEDATA_Fixed64 acc;
        acc.lo = shifted;
        acc.hi = (int)hiPart;

        FACEDATA_Fixed64 half;
        half.lo = (unsigned int)ub >> 1;
        half.hi = 0;

        FACEDATA_Fixed64Add(&acc, &half, &acc);
        result = FACEDATA_Fixed64Divide(&acc, ub);
    }

    return (sign >= 0) ? result : -result;
}

//  `out` is `void*` rather than `SFaceRecordInit*` - same note as
//  FACEDATA_LinkTail above.
void __fastcall FACEDATA_InitRecord(void* srcVtablish, void* field00Value,
                                     void* field04Value, void* outVoid)
{
    SFaceRecordInit* out = (SFaceRecordInit*)outVoid;
    out->m_field1C = *(void**)srcVtablish;
    out->m_field00 = field00Value;
    out->m_field04 = field04Value;
    out->m_field08 = 0;
    out->m_field20 = 0;
    out->m_field14 = 0;
    out->m_field18 = 0;
}
