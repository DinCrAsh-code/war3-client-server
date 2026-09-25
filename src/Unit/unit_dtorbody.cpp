//============================================================================
//  0x6F28B340 - CUnit::~CUnit().  See unit.h.
//
//  Own translation unit so slot 1 (unit_dtor.cpp) keeps its real `call`,
//  exactly as item_dtor.cpp is split from CItem's own slot 1.
//
//  The shipped body is a plain member teardown in **strict descending
//  offset order** - +0x2C4, +0x224, +0x214, +0x180, +0x164, +0x12C,
//  +0x128, +0x118, +0x110, +0xFC, +0xD8, +0xB8, +0xB4, +0xA8, +0x98,
//  +0x7C, +0x68, then the base - which is what a compiler-generated
//  destructor visiting members in reverse declaration order looks like,
//  and it is where unit.h's field types come from.  Three shapes appear:
//
//    * a refcounted pointer: test it, drop a reference, and destroy
//      through vtable slot 0 if that was the last.  No null-out - the
//      object is going away.  Eight fields.
//    * a TRefCnt-derived sub-object: one store of ??_7TRefCnt@@6B@ over
//      its own vtable word, which is the inlined base destructor of
//      something that owns no other resource.  Six fields.
//    * an embedded CAgentTimer: a real call to ~CAgentTimer
//      (0x6F477E20, agenttimer.h).  Three fields.
//
//  **This one cannot match, and the reason is the flags rather than the
//  body.**  The shipped destructor carries an `__except_handler4`-shaped
//  SEH frame with a stack cookie and a seventeen-state unwind map (the
//  `mov [esp+20h+var_4], <state>` stores between every pair of steps),
//  because each of those seventeen members has to be unwound if a later
//  one throws.  This repo builds at a fixed `/GS- /EHs-c-`, which is
//  exactly the combination that cannot produce that frame -
//  docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce".  Every other constructor and destructor in
//  this family (CItem::CItem, CAbility::CAbility, CObserver::~CObserver)
//  loses the same twenty-odd instructions to it.  What is here is the
//  seventeen teardown steps in the shipped order, with nothing else.
//============================================================================
#include "unit.h"
#include "refcnt.h"

//  A refcounted slot as the destructor sees it: drop a reference, destroy
//  through vtable slot 0 if it was the last, and leave the pointer alone.
static void ReleaseRef(void* field)
{
    TRefCnt* p = *(TRefCnt**)field;
    if (p != 0 && --p->m_refcount == 0)
        p->ReleaseSelf();
}

//  A TRefCnt-derived sub-object's own destructor, inlined: one store of
//  ??_7TRefCnt@@6B@.  Spelled as an explicit call to ~TRefCnt through the
//  field's address rather than by making these fields derive from TRefCnt
//  for real - that would give each of them a vtable of its own and stamp
//  the wrong symbol, which tools/abi_audit.py reports as VTABLE SYMBOL
//  WRONG CLASS and the instruction score cannot see.
static void StampTRefCnt(void* field)
{
    ((TRefCnt*)field)->TRefCnt::~TRefCnt();
}

CUnit::~CUnit()
{
    ReleaseRef(&m_pRef2C4);
    ReleaseRef(&m_ref224);

    StampTRefCnt(&m_ref214);
    //  +0x180's own ~CAgentTimer belongs here, between the +0x214 stamp
    //  and the +0x164 one.  It is not written: all three embedded timers
    //  are real CAgentTimer members, so the compiler appends their
    //  destructors itself - correctly, and in the right order among
    //  themselves - but after this body rather than interleaved with it.
    StampTRefCnt(&m_movement);

    ReleaseRef(&m_ref12C);
    ReleaseRef(&m_ref128);

    StampTRefCnt(&m_ref118);
    ReleaseRef(&m_pRef110);

    StampTRefCnt(&m_refFC);
    ReleaseRef(&m_pRefD8);

    StampTRefCnt(&m_mana);
    ReleaseRef(&m_pRefB4);
    ReleaseRef(&m_pRefA8);

    StampTRefCnt(&m_life);
}
