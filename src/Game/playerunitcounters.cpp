//============================================================================
//  0x6F03F970 / 0x6F40F5E0 - the two per-player running totals the unit
//  factory (0x6F29F990, jasscreateunit.h) bumps as it finishes a unit.
//
//  Both are called on the *same* receiver, and the factory says what it is:
//
//      6F29FBE0    mov     ecx, dword_6FAB65F4
//      6F29FBE6    push    ebx                       ; the player index
//      6F29FBE7    call    sub_6F3A1650              ; IndexedArrayHolder::At
//      6F29FBEC    mov     esi, eax                  ; <- the receiver
//
//  i.e. the player record for the index JASS_CreateUnit resolved.  Each
//  call is gated on one bit of the unit type's own flags and carries one
//  field of the unit-type record (unittypequeries.cpp):
//
//      test ebx, 100h  ->  UnitTypeField5C_6F29BAF0  ->  0x6F40F5E0 (+0x90)
//      test ebx, 400h  ->  UnitTypeField60_6F29BB20  ->  0x6F03F970 (+0x80)
//
//  A flag-gated per-unit-type quantity accumulated per player, with a
//  high-water mark kept beside it, is the shape of the food-used /
//  food-provided pair - but that is what the shape suggests, not something
//  this call tree proves, so the names say what the code does and the
//  fields keep their offsets.
//
//  One translation unit for two modules a megabyte apart, which the usual
//  rule would split: neither calls the other, so there is nothing for the
//  compiler to inline across, and both need real calls into
//  misc_handle_lookups.cpp.
//
//  **The counter is read back a second time rather than reused.**  The
//  shipped code calls QueryField78 again after SetField78 instead of
//  keeping the sum it just wrote, and the peak is loaded *between* those
//  two calls.  Both are load-bearing: fold the second read into the value
//  already in hand and the re-read disappears, hoist the peak load and it
//  moves ahead of SetField78.
//
//  **And the first read is named, not the argument.**  `SetField78(current
//  + amount)` with `current` a local is 22 of 22; every spelling that
//  leaves the read inline - `QueryField78() + amount`, the operands
//  swapped, or a named copy of `amount` instead - is 19, because MSVC then
//  folds the argument straight into the add (`add eax, [esp+16]`) where
//  the shipped code loads it into ecx first.  A named copy of `amount`
//  does not do it: the compiler proves that one dead and drops it.
//============================================================================
#include "CFloat.h"

//  Re-declared to match misc_handle_lookups.cpp's own struct exactly - same
//  keyword, same members, same order - so MSVC mangles these two calls onto
//  that file's definitions instead of inventing a second pair of symbols
//  the linker never joins (link_check.py's first defect class).
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int  QueryField78();
    void SetField78(int value);
};

//  The player record, viewed for just these four fields.  A separate view
//  struct rather than four more members on jassnatives.h's SJassPlayer -
//  the same "one object, one view per use" shape that header already uses
//  for SJassPlayerRates and SJassPlayerUnits, and it keeps a struct twenty
//  finished natives depend on untouched.
struct SPlayerUnitCounters
{
    char                         m_reserved00[0x80];
    HandleRefFieldOwner_6F473170 m_counter80;   // +0x80 - field at +0x88/+0x8C
    HandleRefFieldOwner_6F473170 m_counter90;   // +0x90
    char                         m_reservedA0[0x248 - 0xA0];
    unsigned int                 m_peak248;     // +0x248 - m_counter80's
    unsigned int                 m_peak24C;     // +0x24C - m_counter90's

    void AddToCounter80(int amount);   // 0x6F03F970
    void AddToCounter90(int amount);   // 0x6F40F5E0
};

void SPlayerUnitCounters::AddToCounter80(int amount)
{
    HandleRefFieldOwner_6F473170* counter = &m_counter80;

    int current = counter->QueryField78();
    counter->SetField78(current + amount);

    unsigned int peak = m_peak248;
    unsigned int now = (unsigned int)counter->QueryField78();

    if (now > peak)
        m_peak248 = now;
}

void SPlayerUnitCounters::AddToCounter90(int amount)
{
    HandleRefFieldOwner_6F473170* counter = &m_counter90;

    int current = counter->QueryField78();
    counter->SetField78(current + amount);

    unsigned int peak = m_peak24C;
    unsigned int now = (unsigned int)counter->QueryField78();

    if (now > peak)
        m_peak24C = now;
}
