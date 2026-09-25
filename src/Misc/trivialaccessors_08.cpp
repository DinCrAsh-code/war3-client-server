//============================================================================
//  Trivial accessors, batch 08 - a continuation of the sweep
//  docs/targets/trivial_accessor_sweep.md describes.  Same rules: every one
//  of these was already sitting in a dump this repo has and scored TODO;
//  none needed a new dump; each is small enough that "what shape of C++
//  produces this" is the only question left.
//
//  The owners are not identified.  Each carries a struct named for what it
//  does and the address it was found at, and only the fields it actually
//  touches - the same convention HandleRefFieldOwner_6F473170 and
//  IndexedArrayHolder_6F3A1650 already set.
//
//  This batch started larger.  Twenty-one of its functions - the plain and
//  dirty-flag field setters, the eight element-address getters, the two
//  fixed-stride row tables and one of the two fallback selectors - turned
//  out to have been reconstructed concurrently by the CUnit-vtable session
//  under SPlainFieldOwner / SDirtyFlagOwner / SRecordArrayOwner /
//  SRecordTableOwner / SFallbackFieldOwner.  Those are kept and these were
//  dropped on merge: two definitions of one shipped function is a defect in
//  a build that gets JMP-patched into a running game.  Their grouping (one
//  struct per shape, the offset in the method name) is also the better of
//  the two conventions.
//============================================================================
#include "game.h"
#include "refcnt.h"

//----------------------------------------------------------------------------
//  Release the reference this holder carries: decrement, and when that took
//  it to zero destroy through vtable slot 0.  The destroy is a tail jump, so
//  the holder's own frame is gone before the destructor starts.
//----------------------------------------------------------------------------
typedef void (__thiscall *RefDestroyFn)(void* self);

struct RefHolder_6F029510
{
    void Release();

    TRefCnt* m_ref;             // +0x00
};

void RefHolder_6F029510::Release()
{
    TRefCnt* ref = m_ref;
    if (ref != 0 && --ref->m_refcount == 0)
        ((RefDestroyFn)(*(void***)ref)[0])(ref);
}

struct RefHolder_6F029560
{
    void Release();

    TRefCnt* m_ref;             // +0x00
};

void RefHolder_6F029560::Release()
{
    TRefCnt* ref = m_ref;
    if (ref != 0 && --ref->m_refcount == 0)
        ((RefDestroyFn)(*(void***)ref)[0])(ref);
}

struct RefHolder_6F02E200
{
    void Release();

    TRefCnt* m_ref;             // +0x00
};

void RefHolder_6F02E200::Release()
{
    TRefCnt* ref = m_ref;
    if (ref != 0 && --ref->m_refcount == 0)
        ((RefDestroyFn)(*(void***)ref)[0])(ref);
}

struct RefHolder_6F031D20
{
    void Release();

    TRefCnt* m_ref;             // +0x00
};

void RefHolder_6F031D20::Release()
{
    TRefCnt* ref = m_ref;
    if (ref != 0 && --ref->m_refcount == 0)
        ((RefDestroyFn)(*(void***)ref)[0])(ref);
}

//  Byte-identical to RefHolder_6F029560::Release above (same three
//  instructions, same encoding) but at a distinct address - a second COMDAT
//  the linker did not fold, reached as the `eh vector destructor iterator`
//  callback for CCameraWar3::~CCameraWar3's own m_pRampModifiers/
//  m_pOffsetModifiers pairs (GameUI/cameradestructor.cpp, 0x6F308340).
struct RefHolder_6F302CD0
{
    void Release();

    TRefCnt* m_ref;             // +0x00
};

void RefHolder_6F302CD0::Release()
{
    TRefCnt* ref = m_ref;
    if (ref != 0 && --ref->m_refcount == 0)
        ((RefDestroyFn)(*(void***)ref)[0])(ref);
}

//----------------------------------------------------------------------------
//  "That one's field, or mine if there is no that one."  The argument's own
//  +0x04 when it is not null, and this object's +0x08 when it is.  The
//  null-and-fall-back arm is written second, which is what puts it out of
//  line - see the sweep's note on source order deciding block layout.
//----------------------------------------------------------------------------
struct OtherOrOwn_6F35C530
{
    void* Get(const void* other);

    char  m_reserved00[0x08];
    void* m_own;                // +0x08
};

void* OtherOrOwn_6F35C530::Get(const void* other)
{
    if (other != 0)
        return ((void* const*)other)[1];
    return m_own;
}

//----------------------------------------------------------------------------
//  "Are all of these bits set?" - the mask ANDed against the field and
//  compared back against itself, answered as a 0/1 with `setz`.
//----------------------------------------------------------------------------
struct MaskTest_6F601DC0
{
    int HasAll(unsigned int mask);

    char         m_reserved00[0x1CC];
    unsigned int m_flags;       // +0x1CC
};

int MaskTest_6F601DC0::HasAll(unsigned int mask)
{
    return (m_flags & mask) == mask;
}

struct MaskTest_6F601E40
{
    int HasAll(unsigned int mask);

    char         m_reserved00[0x1D4];
    unsigned int m_flags;       // +0x1D4
};

int MaskTest_6F601E40::HasAll(unsigned int mask)
{
    return (m_flags & mask) == mask;
}
