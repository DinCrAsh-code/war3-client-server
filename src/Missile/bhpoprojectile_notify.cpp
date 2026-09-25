//============================================================================
//  0x6F497340 - CBhPoProjectile::NotifyVelocityDelta, vtable slot 17.  See
//  bhpoprojectile.h's own header comment for the full shape.
//
//  `arg`'s own +0x10 pointer resolves to a small record this session did
//  not otherwise identify - only the four fields this function itself
//  touches are named, per CLAUDE.md's "only touched members get names"
//  rule.
//============================================================================
#include "bhpoprojectile.h"
#include "CFloat.h"
#include "game.h"

struct SPoNotifyEventArg
{
    char         m_reserved00[8];
    unsigned int m_tag08;    // opaque - not a legible FourCC either way round

    //  0x6F4AABF0, already real - Missile/ponotifyeventgetter.cpp. A real
    //  call in the shipped stream, not an inlined +0x10 read - kept as one
    //  here too.
    void* __thiscall GetTarget();
};

struct SPoNotifyTarget
{
    CFloat       m_field0;
    CFloat       m_field4;
    CFloat       m_field8;   // written: this->m_velOri - m_field0
    CFloat       m_fieldC;   // written: this->m_velMag - m_field4
    char         m_reserved10[4];
    unsigned int m_flags14;
};

//  0x6F497330, already real - Misc/misc_handle_resolvers.cpp.
struct SHandleRefAt0x68Owner { void* __thiscall ResolveHandle(); };

//  0x6F496EB0 - CBhPoProjectile::ReconcilePathTraceNotify, real now - see
//  Missile/bhpoprojectile_notifycore.cpp (a follow-up session's own work)
//  and bhpoprojectile.h's own declaration.

void CBhPoProjectile::NotifyVelocityDelta(int arg)
{
    SPoNotifyEventArg* evt = (SPoNotifyEventArg*)arg;
    if (evt->m_tag08 != 0x60627065)
        return;

    SPoNotifyTarget* target = (SPoNotifyTarget*)evt->GetTarget();
    target->m_fieldC = *(const CFloat*)&m_velMag - target->m_field4;
    target->m_field8 = *(const CFloat*)&m_velOri - target->m_field0;
    target->m_flags14 |= 2;

    void* resolved = ((SHandleRefAt0x68Owner*)this)->ResolveHandle();
    if (m_reserved1C != 0 && resolved != 0 &&
        *(unsigned int*)((char*)resolved + 0x10) == 0x60706375)
    {
        //  `test ah,5` confirms this is the same mask docs/msvc-vc8-idioms.md's
        //  own "A float comparison's status-word mask, decoded" table gives
        //  `a > b`/`!(a > b)` - both of that table's own precedented
        //  spellings land on `jnp`, though, and the shipped stream here
        //  uses `jp`: a third spelling of the same mask this session did
        //  not reverse-engineer, left as the one still-open instruction
        //  (see this function's own BEHAVIOUR entry).
        CFloat value = *(CFloat*)((char*)resolved + 0xC0);
        if (!(*(const float*)&value > *(const float*)&g_CFloatZero))
        {
            CGridVec2 point = ((CPathTrace*)resolved)->GetCurrentPosition();
            ReconcilePathTraceNotify(&point);
            target->m_flags14 |= 1;
        }
    }
}
