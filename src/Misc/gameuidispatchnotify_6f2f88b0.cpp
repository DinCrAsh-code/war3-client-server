//============================================================================
//  CUnit batch-16 (slot-101/103 closure), reached from sub_6F332790.
//
//  0x6F2F88B0 - resolves `g_unk6FAB65F4`'s acting-slot record's own +0x34
//  pointer's own +0x1E0 field (GetField0x1E0_6F41FA00); if it is set,
//  wraps it in a scoped `SCheckedUnitSlot` (holding one ref across the
//  call the same RAII shape agenttypedslots.h documents) and dispatches
//  a no-argument notification through this object's own +0x3F4 pointer's
//  vtable slot 0x110.
//============================================================================
#include "agenttypedslots.h"

extern void* g_unk6FAB65F4;

int __fastcall GetField0x1E0_6F41FA00(const void* self);

struct IndexedArrayHolder_6F3A1650
{
    char m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
    int At(unsigned int index);
};

struct SActingSlotRecord
{
    char  m_reserved00[0x34];
    void* m_field34;   // +0x34
};

typedef void (__thiscall *DispatchSlot110Fn)(void*);

struct SGameUIDispatchHost
{
    char  m_reserved000[0x3F4];
    void* m_pDispatch3F4;   // +0x3F4

    void NotifyDispatch3F4Slot110(void* arg);
};

void SGameUIDispatchHost::NotifyDispatch3F4Slot110(void* /*arg, unread*/)
{
    IndexedArrayHolder_6F3A1650* holder =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    SActingSlotRecord* record =
        (SActingSlotRecord*)holder->At(holder->m_actingSlot);

    CAgent* candidate = (CAgent*)GetField0x1E0_6F41FA00(record->m_field34);
    if (candidate != 0)
    {
        SCheckedUnitSlot guard(candidate);
        void* dispatch = m_pDispatch3F4;
        ((DispatchSlot110Fn)(*(void***)dispatch)[0x110 / 4])(dispatch);
    }
}
