//============================================================================
//  0x6F4937C0 - `retn 4`: given a pointer, copy the one dword it points to
//  into `this+0xB8` - CUnit's own m_mana (SUnitTrackedRef m_mana, unit.h).
//  +0xB8 is that sub-object's own +0x00, which is its vtable slot (every
//  TRefCnt-derived member CUnit embeds gets its vtable stamped this way -
//  the same "constructor may only stamp a finished class's own vtable,
//  otherwise it stores the shipped address" rule ctor_vtable_audit.py
//  enforces, see docs/notes and pipeline/CLAUDE.md's "The behaviour
//  verdict").
//
//  Nothing in this call tree's own dump names a caller or says what the
//  source pointer points at (a global vtable-address table, most likely,
//  the same shape CUnit's own constructor uses for its four other
//  TRefCnt-derived members at +0x98/+0xFC/+0x118/+0x214) - so the
//  reconstruction below reproduces the four instructions literally through
//  a generic field-copy rather than naming a caller or a table that is not
//  evidenced here.
//============================================================================

//  A local reinterpretation of CUnit's own +0xB8 (m_mana, an
//  SUnitTrackedRef - see unit.h), reached this way rather than by adding a
//  member function to CUnit itself: nothing in this call tree's own dump
//  says this address is a genuine CUnit method (no other member of this
//  family is reached from CUnit's own vtable at this address), only that
//  something with a pointer at +0xB8 is the receiver.
struct SManaVtableHost
{
    //  0x6F4937C0
    void StampManaVtableSlot(void** vtableSlotSource);

    char  m_reserved00[0xB8];
    void* m_manaVtable;   // +0xB8 - CUnit::m_mana.m_vtable
};

void __thiscall SManaVtableHost::StampManaVtableSlot(void** vtableSlotSource)
{
    m_manaVtable = *vtableSlotSource;
}
