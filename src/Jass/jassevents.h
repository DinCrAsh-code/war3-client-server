//============================================================================
//  The JASS "current event" layer the event-reading natives sit on.
//
//  The game keeps one push-down stack per event-context slot inside the
//  object dword_6FAB65F4's holder (SItemSlotTableHolder::m_eventSlots,
//  itemhandletable.h).  Three of those slots are all this family touches:
//
//      slot 4   the current CScriptEvent's handle
//      slot 5   the current CScriptEventData's handle
//      slot 6   the current event id
//
//  **The id is compared biased.**  Every function here adds 0x80200 to the
//  slot-6 value before comparing it, and every constant it is compared
//  against is a five-digit one in the same range - `add eax, 80200h`
//  followed by `sub eax, <0x802xx>` is the shape at the top of all seven
//  bodies.  The bias is written out here rather than folded into the
//  constants because that is what the shipped code does: fold it and the
//  two adds collapse into one and every comparison loses an instruction.
//
//  Two disjoint id ranges reach the same answer by different routes, which
//  is why each accessor comes in pairs:
//
//      0x80226..0x80228 / 0x80310..0x80314   the event's own owner is
//          followed - slot 4, then the owner handle-ref at +0x24, then that
//          object's own event-data ref at +0x44;
//      0x8024B..0x8024D / 0x80321..0x80325   the CScriptEventData is taken
//          straight off slot 5.
//============================================================================
#ifndef JASSEVENTS_H
#define JASSEVENTS_H

#include "itemhandleresolve.h"

//  The bias every comparison in this family is made under - see above.
const unsigned int kEventIdBias = 0x80200;

//----------------------------------------------------------------------------
//  The JASS `event` object.  Only the handle ref the accessors follow is
//  named; everything before it, its vftable included, is untouched here.
//----------------------------------------------------------------------------
class CScriptEvent
{
public:
    char               m_reserved00[0x24];
    //  +0x24 - the object the event happened to.
    SOptionalHandleRef m_ownerRef;
    char               m_reserved2C[0x40 - 0x2C];
    //  +0x40 - a small state enum (0/1/2), read straight back by
    //  GetEventGameState (jasseventgamestate.cpp).
    int                m_gameState;
    //  +0x44 - a second state value, range-checked against 0x1A by
    //  GetEventPlayerState.
    int                m_playerState;
    char               m_reserved48[0x50 - 0x48];
    //  +0x50 - the matched chat string, a handle-bearing object of the
    //  same 8-byte SHandleHolder shape handleobject.h already names
    //  (GetHandleOrZero's own receiver).  Read by
    //  GetEventPlayerChatStringMatched.
    char               m_reservedChatString[8];
};

//----------------------------------------------------------------------------
//  The object that ref names.  Two identical bodies 0x20 apart read its
//  +0x44 ref, one for the order natives and one for the spell natives; they
//  are separate symbols in the shipped image (which was linked without
//  /OPT:ICF - see agentdefaults.cpp), so they are two classes here rather
//  than one class with a method named twice.  Nothing in these call trees
//  says whether the two receivers are really the same type.
//----------------------------------------------------------------------------
//  The two words of the ref are named separately rather than as one
//  SOptionalHandleRef, in these three classes only: the shipped bodies load
//  them off `this` and only *then* form the ref's address, in place
//  (`mov eax,[ecx+48h]` / `and eax,[ecx+44h]` / `add ecx,44h`).  Reaching
//  them through a named sub-object makes the address the common
//  subexpression instead, and MSVC forms it first and shuffles `this` into
//  another register.
struct SOrderEventOwner
{
    char m_reserved00[0x44];
    int  m_eventDataHandle;    // +0x44
    int  m_eventDataTypeTag;   // +0x48

    //  0x6F40B620 - `retn 0`, tail-jumps into QueryHandleField0x54.
    void* GetEventData();
};

struct SSpellEventOwner
{
    char m_reserved00[0x44];
    int  m_eventDataHandle;    // +0x44
    int  m_eventDataTypeTag;   // +0x48

    //  0x6F40B640.
    void* GetEventData();
};

//----------------------------------------------------------------------------
//  What the order natives read the point out of: the object
//  CScriptEventData's +0x38 ref names.  Its two coordinates are eight bytes
//  apart, not four.
//----------------------------------------------------------------------------
struct SEventOrder
{
    char  m_reserved00[0x48];
    CFloat m_pointX;    // +0x48
    char  m_reserved4C[4];
    CFloat m_pointY;    // +0x50
};

//----------------------------------------------------------------------------
//  CScriptEventData - the name is the binary's own
//  (??_7CScriptEventData@@6B@, whose slot 7 returns the 'wscd' tag
//  ResolveEventDataHandle checks).  Two refs are reached out of it.
//
//  The event-unit-getter family (0x6F3C2xxx, jasseventunitgetters.cpp)
//  reaches three more optional-handle refs on this same object, at +0x20,
//  +0x2C/+0x38 (again) and +0x44 - each resolved by a duplicate of
//  GetAbility/GetOrder's own body at a distinct address (the image is
//  linked without /OPT:ICF, agentdefaults.cpp).  Which JASS concept each
//  ref really holds is not settled by this call tree alone - every one of
//  them is read by several different natives across several different
//  event-id ranges - so each duplicate keeps the name of one representative
//  caller rather than a semantic guess; see eventfieldresolve.cpp and
//  jasseventowner.cpp for the bodies.
//----------------------------------------------------------------------------
class CScriptEventData
{
public:
    //  0x6F26FD10 - the order this event is about, or null.  `retn 0`.
    SEventOrder* GetOrder();
    //  0x6F26FD30 / 0x6F26FD60 - that order's target point, written into the
    //  caller's own buffer and also returned.  Both `retn 4`, and both fall
    //  back to g_CFloatZero when the order does not resolve.
    CFloat* GetOrderPointX(CFloat* out);
    CFloat* GetOrderPointY(CFloat* out);
    //  0x6F26FDB0 - the ability this event is about, or null.  `retn 0`.
    void* GetAbility();
    //  0x6F4332A0 - a byte-identical twin of GetAbility above, at a
    //  distinct address (the image was linked without /OPT:ICF, see
    //  agentdefaults.cpp) - the shipped source reuses the same +0x2C/+0x30
    //  ability ref to name whichever dialog button was clicked.  Reached
    //  from GetClickedButton (jassnativesdialog.cpp).  `retn 0`.
    void* GetClickedButtonAbility();

    //  0x6F285F00 - the +0x20 ref (m_ownerRef below) resolved and then
    //  revalidated a second time through IsAgentHandleLive, both wrapped
    //  in a scoped SCheckedUnitSlot - see eventsubjectresolve.cpp.
    //  Called by GetChangingUnit, GetLearningUnit, GetLevelingUnit,
    //  GetLoadedUnit, GetManipulatingUnit and GetSellingUnit's second id
    //  range each - one more event-kind-specific interpretation of the
    //  same field GetExpiredTimer/GetClickedDialog/
    //  GetTournamentFinishNowRule below already read three other ways.
    void* ResolveSubjectUnit();

    //  Duplicates of GetAbility (+0x2C/+0x30) at their own addresses.
    //  eventfieldresolve.cpp / jasseventowner.cpp.
    void* GetAbilityRefForKillEvent();        // 0x6F26FC10
    void* GetAbilityRefForManipulateEvent();  // 0x6F26FC70
    void* GetAbilityRefForTransportEvent();   // 0x6F26FC90
    void* GetAbilityRefForTrainEvent();       // 0x6F40B6A0

    //  Duplicates of GetOrder (+0x38/+0x3C) at their own addresses.
    void* GetOrderRefForBuyEvent();           // 0x6F26FC30
    void* GetOrderRefForSellEvent();          // 0x6F26FC50
    void* GetOrderRefForAttackEvent();        // 0x6F40B6C0

    //  A third optional ref, +0x44/+0x48, with no single canonical use -
    //  no native in either this batch or an earlier one reaches it often
    //  enough on its own to call one of the three "the" one.
    void* GetTargetRef();                     // 0x6F40B6E0
    void* GetTargetRefForSellEvent();         // 0x6F40B700
    void* GetTargetRefForManipulateEvent();   // 0x6F40B720

    char m_reserved00[0x20];
    //  +0x20 - a second nullable handle ref, reused across event kinds:
    //  GetExpiredTimer (jassnatives_expiredtimer.cpp) reads its +0x00
    //  handle word directly as a live object pointer; GetClickedDialog
    //  (jassnativesdialog.cpp) resolves it the ordinary way through
    //  QueryHandleField0x54; GetTournamentFinishNowRule/Player
    //  (jasstournamentfinishnow.cpp) read its two words as a plain rule id
    //  and a player index; ResolveSubjectUnit above resolves it and then
    //  revalidates the result through IsAgentHandleLive.  One field,
    //  several event-kind-specific interpretations - the shipped source's
    //  own event-data union.
    SOptionalHandleRef m_ownerRef;
    char m_reserved28[4];
    int  m_abilityHandle;    // +0x2C
    int  m_abilityTypeTag;   // +0x30
    char m_reserved34[0x38 - 0x34];
    int  m_orderHandle;      // +0x38
    int  m_orderTypeTag;     // +0x3C
    char m_reserved40[0x44 - 0x40];
    int  m_targetHandle;     // +0x44
    int  m_targetTypeTag;    // +0x48
};

//  0x6F3BE130 / 0x6F3BE250 - the two resolvers this layer needs
//  (jassresolveeventhandle.cpp, jassresolveeventdatahandle.cpp).
CScriptEvent*     __fastcall ResolveEventHandle(int hEvent);
CScriptEventData* __fastcall ResolveEventDataHandle(int hEventData);

//  0x6F3BB2C0 - the current event id, unbiased (jasseventid.cpp).
int GetCurrentEventId();

//  0x6F3C1D40 / 0x6F3C1D60 - slots 4 and 5, resolved (jasseventcontext.cpp).
CScriptEvent*     GetCurrentScriptEvent();
CScriptEventData* GetCurrentScriptEventData();

//  0x6F3C2BA0 / 0x6F3C2BE0 - the order natives' two routes
//  (jasseventaccessors.cpp).
void* GetOrderEventOwner();
void* GetOrderEventData();

//  0x6F3C2C40 / 0x6F3C2C10 - the spell natives' two routes.
void* GetSpellEventOwner();
void* GetSpellEventData();

//  0x6F3C3310 - the ability the current spell event is about
//  (jassspelleventability.cpp).
void* GetSpellEventAbility();

#endif
