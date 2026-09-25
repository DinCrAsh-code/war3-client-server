//============================================================================
//  0x6F2BB4F0 - CBulletBase's own vtable slot 3, DispatchMessage.  RENAMED
//  this session from the mis-attributed "SMissileThunderBolt::
//  DispatchMessage" - this address has nothing to do with
//  CMissileThunderBolt specifically; it is CBulletBase's own slot 3, and
//  every subclass (CBullet, CMissile, CMissileThunderBolt) inherits it
//  unchanged.  See Missile/bulletbase.h.
//
//  Switching on the same SAgentMessage::m_id (observer.h) CObserver::
//  Method_0x10 already reads elsewhere in this closure.  The confirmed
//  live case (msg->m_id 852374/852380 -> vtable+0xB4, sub_6F2C42D0, frame
//  [0] of the impact pipeline) and the two sibling cases (vtable+0x5C,
//  vtable+0xB8) are all reachable, but a plain `switch` over the 15-value
//  dense range doesn't reproduce the shipped compiler's two-level
//  byte-remap-table-then-jump-table shape (byte_6F2BB554 - a data table
//  this dump doesn't hand over the contents of) - DIFFERS, not EXACT, for
//  that reason alone; the recognised/unrecognised behaviour itself is
//  unchanged.  See docs/targets/cmissilethunderbolt_impact_pipeline.md and
//  docs/targets/CBulletBase.md.
//============================================================================
#include "bulletbase.h"

typedef void (__thiscall *MissileNoArgHandlerFn)(void* self);
typedef void (__thiscall *MissileMsgHandlerFn)(void* self, const SBulletDispatchMsg* msg);

int CBulletBase::DispatchMessage(const SBulletDispatchMsg* msg)
{
    switch (msg->m_id)
    {
    case 852374:
    case 852380:
        //  vtable+0xB4 - the arrival/impact handler, sub_6F2C42D0 (frame [0]).
        ((MissileNoArgHandlerFn)(*(void***)this)[0xB4 / 4])(this);
        break;

    case 852376:
    case 852383:
        //  vtable+0x5C - not otherwise identified in this closure.
        ((MissileNoArgHandlerFn)(*(void***)this)[0x5C / 4])(this);
        break;

    case 852388:
        //  vtable+0xB8 - the one case that forwards `msg` itself.
        ((MissileMsgHandlerFn)(*(void***)this)[0xB8 / 4])(this, msg);
        break;

    default:
        break;
    }

    return 1;
}
