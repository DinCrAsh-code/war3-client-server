//============================================================================
//  0x6F600720 - CLayer::CLayer.
//
//  Constructs the CObserver base first (a real call, 0x6F629A70), stamps
//  ??_7CLayer@@6B@ over its vtable pointer, zeroes the whole record, and
//  then hands itself to SetOwner - which is what puts a fresh layer either
//  under its owner or on the module's root list.
//
//  The +0xB0 state word is the interesting part: bits 1-3 are rewritten in
//  one read-mask-or, and bit 1 is set only when the *owner* already carries
//  one of its own bits 0 and 1.
//============================================================================
#include "frame.h"
#include "framethunks.h"

CLayer::CLayer(CLayer* owner, int a, int order)
{
    //  No vtable store here: CObserver is a real base class, so MSVC emits
    //  its constructor call and then stamps ??_7CLayer@@6B@ itself, which is
    //  the shipped pair of instructions at 0x6F60074B/0x6F600750.
    m_flags = 0;
    m_flagMask = 0;
    m_children.Init();
    m_pOwner = 0;
    m_float34 = 0.0f;
    m_field24 = a;
    m_field28 = 0;
    m_field2C = 0;
    m_field30 = 0;
    m_field38 = 0;
    m_field3C = 0;
    m_field40 = 0;
    m_field44 = 0;
    m_field50 = 0;
    m_field54 = 0;
    m_field58 = 0;
    m_field5C = 0;
    m_field64 = 0;
    m_field68 = 0;
    m_float6C = 0.0f;
    m_float70 = 0.0f;
    m_list74.Init();

    //  Placement-constructed rather than declared as a member: a member's
    //  constructor runs before the body, and the shipped stream has this
    //  call after two thirds of the field stores.
    new ((void*)m_shortcuts) SHORTCUTREGTABLE;

    m_order = order;
    m_enabled = 0xFF;
    m_field44 = this;
    m_field68 = this;

    int flag = 0;
    if (owner != 0)
    {
        int free_ = ((owner->m_stateBits & 3) != 0) ? 0 : 1;
        flag = (free_ == 0);
    }

    unsigned int before = m_stateBits;
    m_stateBits = ((((unsigned int)flag & 1) | 6) * 2) | (before & 0xFFFFFFCCu);

    SetOwner(owner, 1, 1);
}
