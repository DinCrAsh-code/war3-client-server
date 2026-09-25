//============================================================================
//  CDestructable's short, self-contained vtable slots - everything that
//  needs no translation unit of its own because nothing else in this file
//  calls anything else in it and none of them are large enough to be worth
//  splitting further.  See destructable.h for the class and the field
//  evidence.
//============================================================================
#include "destructable.h"
#include "positiontrack.h"
#include "observer.h"
#include "agenttimer.h"

//  0x6F26A010 - the death/regrowth trigger slot 3 (Method_0x0C) hands
//  non-zero message ids to.  Takes `this` alone and cleans nothing
//  (`retn`); its own body is eighty instructions deep into rendering/
//  pathing teardown that is out of scope for this vtable-slot pass (see
//  docs/targets/CDestructable__vtable.md) - declared here as a naked
//  redirect only so the one real call site below compiles and links
//  against the shipped address.  `__fastcall` with one pointer argument is
//  this repo's spelling for "receiver in ecx, cleans nothing"
//  (unreconstructed_thunks.cpp's own ADDR_THUNK examples).
__declspec(naked) void __fastcall CDestructableDeathTrigger(void*)
{
    __asm { mov eax, 06F26A010h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F26A4A0 - slot 3 (+0x0C).  Every other class in this family answers a
//  flat 0 (CObserver::Method_0x0C, agentwar3_slots.cpp) or looks at the
//  message id (CItem::Method_0x0C, item_message.cpp); this one looks at
//  the id only as a boolean ("is this message about anything at all"),
//  with no per-id dispatch of its own.
//----------------------------------------------------------------------------
int CDestructable::Method_0x0C(const SAgentMessage* msg)
{
    if (msg->m_id != 0)
        return 0;

    CDestructableDeathTrigger(this);
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F2661E0 - slot 8 (+0x20).  Same shape as CItem::Method_0x20
//  (item_vtable2.cpp), reaching Position::RecordSlot8 through a
//  reinterpret cast to the embedded handle sub-object rather than a real
//  Position member - see destructable.h's own note on m_handleObject.
//----------------------------------------------------------------------------
int CDestructable::Method_0x20(int context)
{
    int n = CWidget::Method_0x20(context);
    ((PositionTrack*)&m_handleObject)->RecordSlot8(n, context, 0, 0);
    return n + 1;
}

//----------------------------------------------------------------------------
//  0x6F266820 - slot 9 (+0x24).  CItem::Method_0x24's own shape
//  (item_vtable2.cpp): through Position's own vtable slot 3 rather than a
//  named function.
//----------------------------------------------------------------------------
typedef void (__thiscall *PositionSlot3Fn)(void* self, int n, int context);

int CDestructable::Method_0x24(int context)
{
    int n = CWidget::Method_0x24(context);
    PositionTrack* handle = (PositionTrack*)&m_handleObject;
    ((PositionSlot3Fn)(*(void***)handle)[0x0C / 4])(handle, n, context);
    return n + 1;
}

//----------------------------------------------------------------------------
//  0x6F267370 - slot 10 (+0x28), GetRecordVersion.  CAgentWar3's own
//  version of this slot adds one to CAgent's; CDestructable adds two.
//----------------------------------------------------------------------------
int CDestructable::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 2;
}

//----------------------------------------------------------------------------
//  0x6F2668A0 - slot 13 (+0x34), Deactivate.  Cancel the timer, then drop
//  the reference the same "test/decrement/dispatch slot 0" shape
//  unit_dtorbody.cpp's own ReleaseRef uses - reused verbatim here rather
//  than duplicated with a different name, since it is file-scope static in
//  that translation unit and this one needs its own copy either way.
//----------------------------------------------------------------------------
static void ReleaseTimerRef(CAgentTimer** field)
{
    TRefCnt* p = *(TRefCnt**)field;
    if (p != 0 && --p->m_refcount == 0)
        p->ReleaseSelf();
}

void CDestructable::Deactivate()
{
    CWidget::Deactivate();
    if (m_ref58 != 0)
    {
        m_ref58->Cancel();
        ReleaseTimerRef(&m_ref58);
    }
    m_ref58 = 0;
}

//----------------------------------------------------------------------------
//  0x6F267280 - slot 17 (+0x44), DumpState.
//----------------------------------------------------------------------------
void CDestructable::DumpState(void* sink)
{
    CWidget::DumpState(sink);
    m_floatB.DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F266140 - slot 22 (+0x58), GetClassName.
//----------------------------------------------------------------------------
const char* CDestructable::GetClassName()
{
    return "CDestructable";
}

//----------------------------------------------------------------------------
//  0x6F267240 - slot 46 (+0xB8), GetHandleObject.
//----------------------------------------------------------------------------
SHandleWithType* CDestructable::GetHandleObject()
{
    return &m_handleObject;
}

//----------------------------------------------------------------------------
//  0x6F266330 - slot 47 (+0xBC).  A fixed config constant, `fld` and
//  nothing else - the same shape CItem's and CWidget's own slot 47 use for
//  a different constant each (item.h's own comment, "CWidget's (g_flt...
//  against g_flt...)").
//----------------------------------------------------------------------------
extern const float g_flt6F92E788;

float CDestructable::Method_0xBC()
{
    return g_flt6F92E788;
}

//----------------------------------------------------------------------------
//  0x6F267490 - slot 60 (+0xF0), QueryVisible.  Bit 13 of CAgentWar3's own
//  m_flags (+0x20), negated - `which` is unread, matching CWidget's own
//  slot 60 shape of ignoring the argument for a single fixed bit.
//----------------------------------------------------------------------------
int CDestructable::QueryVisible(int /*which*/)
{
    return ((m_flags >> 13) & 1) == 0;
}

//----------------------------------------------------------------------------
//  0x6F266300 - slot 61 (+0xF4), RefreshSpriteVisibility.  If this
//  object's own slot 79 (Method_0x13C) says so, hide the sprite outright;
//  otherwise defer to the base (CWidget::RefreshSpriteVisibility,
//  widget_slot61.cpp).
//----------------------------------------------------------------------------
typedef int (__thiscall *Slot0x13CFn)(void* self);

void __fastcall SpriteSetHidden(CSprite* sprite, int visible);

void CDestructable::RefreshSpriteVisibility()
{
    if (((Slot0x13CFn)(*(void***)this)[0x13C / 4])(this))
        SpriteSetHidden(m_pSprite, 0);
    else
        CWidget::RefreshSpriteVisibility();
}

//----------------------------------------------------------------------------
//  0x6F2672E0/0x6F2672F0/0x6F267300 - slots 75-77 (+0x12C/+0x130/+0x134),
//  the plain CFloat getters CItem's own GetLife/GetMaxLife/GetMinLife
//  mirror (item_vtable.cpp's own doc note on the shape).
//----------------------------------------------------------------------------
CFloat* CDestructable::GetLife(CFloat* out)
{
    *out = m_floatB.m_value;
    return out;
}

CFloat* CDestructable::GetMaxLife(CFloat* out)
{
    *out = m_floatB.m_max;
    return out;
}

CFloat* CDestructable::GetMinLife(CFloat* out)
{
    *out = m_floatB.m_min;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F267310/0x6F267340 - slots 80/81 (+0x140/+0x144), SetMinLife/
//  SetMaxLife.  CItem::SetMinLife/SetMaxLife's own shape exactly
//  (item_vtable.cpp), through FloatMiniB::SetPair.
//----------------------------------------------------------------------------
void CDestructable::SetMinLife(const CFloat* value)
{
    m_floatB.SetPair(*value, m_floatB.m_max);
}

void CDestructable::SetMaxLife(const CFloat* value)
{
    m_floatB.SetPair(m_floatB.m_min, *value);
}

//----------------------------------------------------------------------------
//  0x6F267260/0x6F267270 - slots 92/93 (+0x170/+0x174), the plain
//  m_positionNotifyHandle accessors - CWidget's own slot 92/93 shape
//  (position.h's own m_notifyHandle), just against this class's own
//  standalone field rather than a real Position member.
//----------------------------------------------------------------------------
int CDestructable::GetPositionNotifyHandle()
{
    return (int)m_positionNotifyHandle;
}

void CDestructable::SetPositionNotifyHandle(int handle)
{
    m_positionNotifyHandle = (unsigned int)handle;
}

//----------------------------------------------------------------------------
//  0x6F2672D0 - slot 107 (+0x1AC), the first slot CDestructable adds.
//  Bit 4 of CAgentWar3's own m_flags.
//----------------------------------------------------------------------------
int CDestructable::IsFlagBit10Set()
{
    return (m_flags >> 4) & 1;
}

//----------------------------------------------------------------------------
//  0x6F267250 - slot 108 (+0x1B0), the second slot CDestructable adds.
//  `retn 0Ch` and no stack reads: three dead parameters this reconstruction
//  declares as none, matching what thunk_abi_audit.py can check (the
//  shipped cleanup) rather than guessing at three unread types.
//----------------------------------------------------------------------------
float CDestructable::GetReservedFloat80(int, int, int)
{
    return m_reservedFloat80;
}
