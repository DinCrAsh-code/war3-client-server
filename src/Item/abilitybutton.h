//============================================================================
//  CAbilityButton: adds the command-card icon WC3 shows for an ability -
//  the base every usable (button-bearing) ability class derives from,
//  `CAbilitySpell`'s own constructor (0x6F068DF0) included.
//
//  Real RTTI (docs/targets/class_informer_dump.tsv): `CAbilityButton:
//  CPower, CAbilityInterfaced, CAbility, CUnitRefList, CAgentWar3, CAgent,
//  CObserver, TRefCnt`.  This file used to declare `: public CAbility`
//  directly, skipping both CPower and CAbilityInterfaced entirely - fixed
//  once CPower itself was reconstructed (power.h).  Neither intermediate
//  class adds a field (power.h/abilityinterfaced.h's own header comments),
//  so this fix is a pure hierarchy correction: CAbilityButton's own field
//  layout below is unchanged and still begins at +0x6C, exactly where
//  CAbility's own layout ends.
//
//  Header-inline and trivial enough that /Ob2 inlines it into every one
//  of its callers rather than keeping it as its own out-of-line function
//  (the same shape floatmini.h's own FloatMini is in) - there is no
//  separate `CAbilityButton::CAbilityButton` address in this call tree,
//  only its effect inlined into CAbilitySpell::CAbilitySpell's own body.
//
//  Field layout from that inlined effect:
//    +0x6C  m_defaultIconId  - dword_6F92ED9C (a shared default-icon
//                              constant; not read back by this call tree,
//                              so its own type beyond "a dword" is
//                              unconfirmed)
//    +0x70..+0x7C  four reserved dwords, all zeroed
//
//  Vtable: 210 slots (docs/targets/vtables/CAbilityButton.txt), 16 more
//  than CPower's own 194.  Diffed index-by-index against CPower's own
//  committed dump (agent_worktrees/classes/0x6F8861DC.json vs
//  0x6F87EA44.json): CAbilityButton overrides exactly 13 of CPower's own
//  slots and adds exactly 16 new ones at the end (194-209).  See each
//  method's own comment below for its slot/offset/address and, for the
//  three substantial ones, its own .cpp file's header for the derivation.
//============================================================================
#ifndef ABILITYBUTTON_H
#define ABILITYBUTTON_H

#include "power.h"
#include "observer.h"   // SAgentMessage - slot 3's real base signature

extern const unsigned int g_defaultAbilityIconId;   // dword_6F92ED9C

class CAbilityButton : public CPower
{
public:
    CAbilityButton()
        : m_defaultIconId(g_defaultAbilityIconId),
          m_field70(0), m_field74(0), m_field78(0), m_field7C(0)
    {
    }

    //  slot 1 / +0x04 (0x6F068900) - covariant return, same shape as
    //  CPower::DeleteSelf.  See abilitybutton_deleteself.cpp.
    virtual CAbilityButton* DeleteSelf(int flags);

    //  slot 3 / +0x0C (0x6F024210), overriding CObserver::Method_0x0C
    //  (observer.h, same signature - what lands the override at the
    //  base's own slot instead of appending a new one): "does this
    //  message's own id (+0x08) match this instance's own current
    //  icon-like answer" comparator - tries Method_0x308() first, falls
    //  back to Method_0x318() (skipping the default-icon sentinel), and
    //  applies the matching new leaf slot (0x330/0x334) on a hit.
    //  See abilitybutton_slots.cpp.
    virtual int Method_0x0C(const struct SAgentMessage* msg);

    //  slot 7 / +0x1C (0x6F0241F0) - `mov eax, 41416274h; retn`, the
    //  multichar literal 'AAbt'.  See abilitybutton.cpp.
    virtual unsigned int GetAgileTypeId() const;

    //  slots 14/15 / +0x38/+0x3C (0x6F042C90/0x6F042D50), overriding
    //  CAgentWar3::Save/Load (agentwar3.h): base call first, then three
    //  version-gated groups for this class's own four fields.
    //  See abilitybutton_saveload.cpp.
    virtual void Save(class CDataStoreScratch* store);
    virtual void Load(class CDataStore* store);

    //  slot 22 / +0x58 (0x6F0241C0).  See abilitybutton.cpp.
    virtual const char* GetClassName();

    //  slots 32/33 / +0x80/+0x84 (0x6F054860/0x6F054840), overriding
    //  CAbility::Method_6F052B10/Method_6F052AF0 (ability.h, same
    //  signatures - what lands the override at the base's own slot):
    //  reset this instance's own four button fields to their construction
    //  defaults, then tail-call the base version unchanged.
    //  See abilitybutton_slots.cpp.
    virtual void Method_6F052B10(void* arg0, void* recordField);
    virtual void Method_6F052AF0(void* target);

    //  slot 108 / +0x1B0 (0x6F0312A0) - a real, substantial (~180 own dump
    //  lines) icon-registration accessor built on top of
    //  CAbility::GetCachedFieldRecord/LookupAbilityFieldRecord
    //  (ability_slots.cpp/ability_typefieldrecord.cpp - both already
    //  reconstructed and reused directly), reaching this instance's own
    //  new leaf slots (0x308/0x30C/0x318/0x320/0x324/0x328/0x32C) and two
    //  further command-order-shaped leaves (sub_6F332A50, sub_6F332A10).
    //  **Still a THUNK, not yet a full reconstruction** - but not for the
    //  ABI reason an earlier session recorded here.  A full byte-level
    //  stack-effect trace (abilitybutton_bigslots.cpp's own header) found
    //  every call site in this function balances exactly against its
    //  callee's own `retn`; what is left is simply real work this session
    //  did not finish - matching MSVC's exact instruction order across a
    //  dozen-plus vtable dispatches in one ~180-line body.  See
    //  abilitybutton_bigslots.cpp for the full trace.
    virtual void Method_0x1B0();

    //  slot 112 / +0x1C0 (0x6F045950) - same shape and same status as slot
    //  108 above (~207 own dump lines, no ABI issue, not yet written out).
    //  See abilitybutton_bigslots.cpp.
    virtual void Method_0x1C0(int, int);

    //  slot 135 / +0x21C (0x6F0228F0), overriding
    //  CAbility::DefaultIntFieldQueryAO(int) (ability.h, same signature):
    //  answers whether the incoming code equals this instance's own
    //  Method_0x308() answer.  See abilitybutton_slots.cpp.
    virtual int DefaultIntFieldQueryAO(int code);

    //  slot 166 / +0x298 (0x6F045BE0) - same shape and same status as slot
    //  108 above (~182 own dump lines, no ABI issue, not yet written out).
    //  See abilitybutton_bigslots.cpp.
    virtual void Method_0x298(int);

    //  slot 193 / +0x304 (0x6F031490), overriding CPower::Method_0x304.
    //  See abilitybutton_slots.cpp.
    virtual int Method_0x304(int);

    //  slots 194-205/208 / +0x308..+0x330/+0x340, twelve of CAbilityButton's
    //  own sixteen new slots past CPower's own 194 - plain constant
    //  answers or one-line tail-forwards to another of this same group.
    //  See abilitybutton_newslots.cpp.
    virtual unsigned int Method_0x308();   // 194, +0x308 (0x6F068830)
    virtual int          Method_0x30C();   // 195, +0x30C (0x6F068840)
    virtual unsigned int Method_0x310();   // 196, +0x310 (0x6F068850) - forwards to Method_0x308
    virtual int          Method_0x314();   // 197, +0x314 (0x6F068860)
    virtual unsigned int Method_0x318();   // 198, +0x318 (0x6F068870)
    virtual int          Method_0x31C();   // 199, +0x31C (0x6F068880)
    virtual unsigned int Method_0x320();   // 200, +0x320 (0x6F068890)
    virtual int          Method_0x324();   // 201, +0x324 (0x6F0688A0)
    virtual unsigned int Method_0x328();   // 202, +0x328 (0x6F0688B0)
    virtual int          Method_0x32C();   // 203, +0x32C (0x6F0688C0)
    //  204/205 each take one stack argument (`retn 4`) but never forward
    //  it anywhere - confirmed off both raw_asm bodies, no push before
    //  either's own `call edx` - so the incoming pointer is only ever
    //  cleaned off the stack, never read.  See abilitybutton_slots.cpp.
    virtual void Method_0x330(void* rec);  // 204, +0x330 (0x6F022980) - calls Method_0x338()
    virtual void Method_0x334(void* rec);  // 205, +0x334 (0x6F0688D0) - calls Method_0x33C()

    //  slots 206/207 / +0x338/+0x33C (0x6F054880/0x6F054950) - real,
    //  substantial command-order bodies (~76/~53 own dump lines), `this`
    //  only (`retn 0`).  Reconstructed this session - every instruction,
    //  register and call site is right (0.605/0.722 verify.py scores);
    //  what is left is a compiler block-layout choice (which branch of a
    //  three-test OR-chain becomes the physical fall-through) that does
    //  not follow source order here - the same gap Method_0x304 above
    //  already has, DIFFERS not EXACT.  See abilitybutton_bigslots.cpp
    //  for the full derivation.  `void`, not `int`:
    //  every real return path either falls straight through to a `retn`
    //  with no explicit `mov eax` (a genuinely unused return value, not
    //  one this call tree ever reads) or tail-forwards into another slot
    //  through the vtable, so nothing here is a meaningful `int`.
    virtual void Method_0x338();
    virtual void Method_0x33C();

    //  slot 208 / +0x340 (0x6F0688E0) - tail-forwards to Method_0x318.
    virtual unsigned int Method_0x340();

    //  slot 209 / +0x344 (0x6F0688F0) - `nullsub_257`, genuinely empty
    //  (confirmed via `worktree_store.py bytes 0x6F0688F0`: a bare `retn`).
    //  See abilitybutton_newslots.cpp.
    virtual void Method_0x344() { }

    unsigned int m_defaultIconId;  // +0x6C
    unsigned int m_field70;        // +0x70
    unsigned int m_field74;        // +0x74
    unsigned int m_field78;        // +0x78
    unsigned int m_field7C;        // +0x7C
};

#endif
