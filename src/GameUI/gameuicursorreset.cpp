//============================================================================
//  0x6F38E050 - CWorldFrameWar3::ResetTrack.
//
//  Drop whatever the cursor is tracking - posting one CSpriteTrackEvent
//  through the cursor's own vtable slot 0x10 first, so the tracked sprite
//  hears about it - and then re-decide which of the three pause-highlight
//  modes the world should be in.
//
//  Two things about the event are deliberate:
//
//    * it is a plain local record and *not* a class with a `virtual` in it.
//      Declaring one would make this build stamp its own
//      `??_7CSpriteTrackEvent@@6B@`, which the instruction score cannot see
//      (both sides canonicalise to `mov [reg], SYM`) and which
//      tools/abi_audit.py reports as VTABLE SYMBOL / WRONG CLASS.  The two
//      real vtable addresses go in as literals instead, the way
//      Config/CMiscCustom.cpp already does it - 0x6F9413E8 from
//      `ida_query names TrackEvent`, and TRefCnt's own 0x6F8765F8 from
//      docs/targets/vtables/TRefCnt.txt.  The cost is two instructions:
//      the shipped operands are named symbols and these are numbers;
//    * the second vtable store is the inlined `~TRefCnt`, which is why it
//      is the *last* thing the guarded block does.
//
//  The three-option tail is one short-circuiting `||` per flag, not three
//  independent tests: when the first lookup answers, neither of the other
//  two is made at all, which is what the shipped code's single `jnz` into
//  the shared `mov esi, 1` says.
//
//  The whole body sits inside the `__except_handler4`-shaped frame this
//  toolchain cannot reproduce (docs/msvc-vc8-idioms.md).
//
//  Own translation unit: five real calls out of it.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"

//  The scoped event the cursor posts.  Only the fields the shipped code
//  writes are named; +0x10 and +0x28 are never touched here.
struct SSpriteTrackEvent
{
    void* m_vtable;             // +0x00
    int   m_field04;            // +0x04
    int   m_kind;               // +0x08
    int   m_field0C;            // +0x0C
    int   m_reserved10;         // +0x10
    float m_f14;                // +0x14
    float m_f18;
    float m_f1C;
    float m_f20;
    float m_f24;
    int   m_reserved28;         // +0x28
    void* m_pTarget;            // +0x2C
    void* m_pOwner;             // +0x30
    int   m_field34;            // +0x34
};

//  slot 0x10 of the cursor: post one track event.
typedef void (__thiscall *PostSpriteEventFn)(void* self, SSpriteTrackEvent* ev);

//  A reference-counted object: slot 0 of the vtable is ReleaseSelf and the
//  count is at +0x04 - the same shape agenttypedslots.h's Release() spells.
struct SRefCountedTarget
{
    void* m_vtable;
    int   m_refcount;
};

enum
{
    kUiOptionHighlightAny     = 0x002,
    kUiOptionHighlightAllies  = 0x112,
    kUiOptionHighlightEnemies = 0x113,
    kSpriteTrackEventKind     = 0x1A0066
};

void CWorldFrameWar3::ResetTrack()
{
    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);

    void* target = m_pTrackTarget;

    if (target != 0)
    {
        SSpriteTrackEvent ev;

        ev.m_field04 = 0;
        ev.m_f14 = 0.0f;
        ev.m_kind = kSpriteTrackEventKind;
        ev.m_f18 = 0.0f;
        ev.m_field0C = 0;
        ev.m_f1C = 0.0f;
        ev.m_vtable = (void*)0x6F9413E8;
        ev.m_f20 = 0.0f;
        ev.m_f24 = 0.0f;
        ev.m_pTarget = target;

        SRefCountedTarget* owner = (SRefCountedTarget*)m_pTrackOwner.m_ptr;

        ev.m_pOwner = owner;
        ev.m_field34 = 0;
        m_pTrackTarget = 0;

        if (owner != 0)
        {
            owner->m_refcount += -1;
            if (owner->m_refcount == 0)
                ((void (__thiscall *)(void*))((void**)owner->m_vtable)[0])(owner);

            m_pTrackOwner.m_ptr = 0;
        }

        ((PostSpriteEventFn)(*(void***)this)[0x10 / 4])(this, &ev);

        //  ~TRefCnt, inlined: the base's destructor stamps its own vtable
        //  back over the event's on the way out.
        ev.m_vtable = (void*)0x6F8765F8;
    }

    int any = IsEventKeyRegistered((void*)kUiOptionHighlightAny);

    int showAllies  = (any != 0) || (IsEventKeyRegistered((void*)kUiOptionHighlightAllies) != 0);
    int showEnemies = (any != 0) || (IsEventKeyRegistered((void*)kUiOptionHighlightEnemies) != 0);

    if (ui->m_cursorLocked != 0)
    {
        showAllies  = (showAllies == 0) ? 1 : 0;
        showEnemies = (any == 0) ? 1 : 0;
    }

    UpdatePauseHighlights((showAllies != 0 || showEnemies != 0) ? 1 : 0,
                          showAllies, showEnemies);
}
