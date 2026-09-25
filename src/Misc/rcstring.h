//============================================================================
//  RCString - a reference-counted string handle, and the name is stamped:
//  its destructor (0x6F4C48F0) writes `??_7RCString@@6B@` into the object
//  before tearing it down and `??_7TRefCnt@@6B@` after, so the class derives
//  from TRefCnt (src/Agent/refcnt.cpp) and holds a second refcounted object
//  - the shared character buffer - at +0x08.
//
//  That buffer's own `char*` is at +0x1C, which is where 0x6F4C4630 reads it
//  from.  **That function is already reconstructed in
//  src/Agent/handleobject.cpp as `GetHandleOrZero`**, under a
//  `SHandleHolder`/`m_handle` reading of the same two fields; the vtable
//  stamp here says it is really `RCString::GetCStr`.  It is left where it is
//  and called from here rather than renamed - a rename is a new mix_config
//  key and would move another target's numbers, and the emitted code is
//  identical either way.  See the target write-up.
//============================================================================
#ifndef RCSTRING_H
#define RCSTRING_H

#include "handleobject.h"

int __stdcall Storm_508(const char* a, const char* b, unsigned int max);

//----------------------------------------------------------------------------
//  The shared buffer.  Only the three fields this call tree touches are
//  named.
//----------------------------------------------------------------------------
struct RCSTRINGDATA
{
    //  0x6F4C4580 - retn 4.
    int Equals(const RCSTRINGDATA* other) const;

    void** m_vtable;            // +0x00 - slot 2 (+0x08) is the release
    int    m_refcount;          // +0x04
    char   m_reserved8[0x14];   // +0x08
    char*  m_chars;             // +0x1C
};

//----------------------------------------------------------------------------
//  0x6F4C4540 - the comparison tail both Equals and RCString::EqualsCStr
//  fall into.  It has no dump heading of its own: IDA counts it inside each
//  of the two, so it has to be inlined into both, and /Ob2 declines it at
//  this size on its own.
//
//  The `neg`/`sbb`/`add 1` the shipped tail ends with is `Storm_508(...) == 0`
//  returned as an int - see docs/msvc-vc8-idioms.md on the -1/0 mask, and
//  note that `!Storm_508(...)` is not the same code.
//----------------------------------------------------------------------------
__forceinline int RCStringCharsEqual(const char* mine, const char* other)
{
    if (mine == 0)
        return other == 0;

    if (other == 0)
        return 0;

    return Storm_508(other, mine, 0x7FFFFFFF) == 0;
}

class RCString
{
public:
    //  0x6F011300 - retn 4.  Build one over `text` (a null text is the
    //  shared empty buffer).  A **member**, and that is the whole point of
    //  the declaration: the shipped body takes `this` in ecx, one push, and
    //  pops it.  It used to be redirected as a free
    //  `__fastcall StringFieldConstruct(void*, const char*)`, which pushes
    //  nothing where the callee cleans four bytes - a live stack shift in
    //  every one of its five call sites, CSaveGame's own constructor
    //  included.  `thunk_abi_audit.py` did not catch it: IDA's body for
    //  0x6F011300 carries an appended chunk that ends in a bare `retn`, so
    //  the tool's accepted set for that address is {0, 4} and the wrong
    //  declaration's 0 was in it.
    //
    //  0x6F011300 also carries a *second* C++ name in this repo,
    //  `SUnitCreateParamsSub::SUnitCreateParamsSub` (Unit/unitcreateparams.h),
    //  which is the same twelve-byte object seen from the unit-creation
    //  side.  Both are redirects, so neither shadows a real body, but every
    //  call site of one of them scores against the other's name.  Unifying
    //  the two types is a repo-wide refactor and is left recorded rather
    //  than done.
    void Construct(const char* text);

    //  0x6F4C48F0 - retn 0.
    ~RCString();

    //  0x6F4C4610 - retn 4.
    int EqualsCStr(const char* other) const;

    //  0x6F4C45A0 - retn 4.  Take a reference on `other`'s buffer, drop this
    //  one's, and adopt it.  The order is the shipped one and it matters:
    //  the new buffer is retained *before* the old is released, so
    //  self-assignment cannot free the string it is about to keep.
    RCString* Assign(const RCString* other);

    //  0x6F4C45E0 - retn 0.  Drop the buffer unless it is the shared empty
    //  one at 0x6FA85E18.
    void Reset();

    //  0x6F4C5D50 - retn 0Ch, with a receiver in ecx nothing in the body
    //  reads: a static member.  Build `out` as `a` then `b`, through a
    //  0x1001-byte stack buffer.
    RCString* Concat(RCString* out, const char* a, const char* b);

    //  0x6F4C5DD0 / 0x6F4C5E70 - retn 4 each.  Append, through a temporary
    //  so that appending a string to itself is safe.
    RCString* AppendCStr(const char* text);
    RCString* AppendRCString(const RCString* other);

    void**        m_vtable;     // +0x00
    int           m_refcount;   // +0x04
    RCSTRINGDATA* m_data;       // +0x08
};

//: off_6FA85E18 - the one shared empty RCSTRINGDATA.
extern RCSTRINGDATA g_emptyRCStringData;

//: ??_7RCString@@6B@ / ??_7TRefCnt@@6B@ - the two vtables the destructor
//: stamps.  This build emits neither, so they are *local* symbols defined in
//: rcstringglobals.cpp rather than funcmap.DATA rows - see
//: src/Misc/cstringrep.h for the reasoning and the arm-with-caution
//: consequence.  Not `const`: a const array at namespace scope that its own
//: translation unit never reads gets internal linkage and is discarded, which
//: is how the first spelling here left the .mix with an unresolved external.
extern void* g_vftableRCString[1];
extern void* g_vftableTRefCnt[1];

#endif
