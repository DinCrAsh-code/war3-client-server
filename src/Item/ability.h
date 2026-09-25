//============================================================================
//  CAbility: the base class behind every `CAbilityXxx`/`CBuffAuraXxx` class
//  named by this build's own RTTI (`??_R4CAbility@@6B@`, live IDA symbol
//  search - `CAbilitySpellBook`, `CAbilityInventory`, `CAbilityAuraXxx`,
//  hundreds of per-spell classes).  This is the generic "a unit/item has
//  one of these" mechanism WC3 abilities, item-granted spells and auras
//  all share - CItem itself carries none of this data; it is attached to
//  whichever CAgent (almost always a unit) the ability belongs to.
//
//  Derives from CUnitRefList (unitreflist.h) - a correction to this
//  session's own earlier work (see that header's own note): the real
//  chain, read directly off this build's RTTI, is CAgent : CAgentWar3 :
//  CUnitRefList : CAbility, not CAbility : CAgent.  Byte-identical either
//  way in everything already verified (CAgentWar3/CUnitRefList's own
//  constructors are trivial enough to inline into whichever derived class
//  actually gets constructed), but the earlier model owned the wrong
//  offsets and would have landed any new CAbility-level virtual at the
//  wrong vtable slot.
//
//  Field layout from CAbility::CAbility (0x6F052190)'s own writes, now
//  correctly attributed - CAgentWar3::m_flags is zeroed here (inherited,
//  not CAbility's own field) and +0x24/+0x28 belong to CUnitRefList:
//    +0x2C  m_field2C   - 0, CAbility's own first field
//    +0x30  m_field30, m_field34 renamed m_typeId below - see its own
//           comment
//    +0x38..+0x54  eight reserved dwords, all zeroed
//    +0x58  m_timer     - an embedded CAgentTimer (agenttimer.h),
//                          placement-constructed the same way CItem embeds
//                          two of its own - ends the class at +0x6C.
//  None of the remaining reserved dwords or m_field2C are read by this
//  call tree, so - CLAUDE.md's own rule - they stay unnamed rather than
//  guessed at.
//
//  Vtable: all 189 slots (docs/targets/vtables/CAbility.txt) are declared
//  `virtual` below, in shipped order - CAbilityInterfaced (ability.h's own
//  sibling reconstruction) derives directly from this class and both
//  overrides several of these slots and adds one new slot of its own, so
//  every one of CAbility's own 189 has to occupy its real index or
//  CAbilityInterfaced's own additions land at the wrong slot.  36 of the
//  189 are slots CAbility does not touch at all (0-31 minus the ten
//  listed just below - CAgentWar3/CAgent/CObserver/TRefCnt's own
//  functions carry over unchanged, C++'s default behavior for an
//  unoverridden virtual) plus the ten it does override within that same
//  0-31 range; every slot at or past 32 is CAbility's own, declared once,
//  in order, further down.
//============================================================================
#ifndef ABILITY_H
#define ABILITY_H

#include "unitreflist.h"
#include "agenttimer.h"

//  Forward declarations for the override signatures below - same reason
//  agent.h forward-declares them.
class CDataStore;
class CDataStoreScratch;

class CAbility : public CUnitRefList
{
public:
    //  0x6F052190 - see ability.cpp.
    CAbility();

    //  0x6F038580 - see ability_dtor.cpp.  Reached only as DeleteSelf's own
    //  callee (below); an `__except_handler4`-shaped SEH frame wraps the
    //  real teardown (m_timer's own non-trivial destructor is enough on
    //  its own, the same "one non-trivial member is enough" shape
    //  agentwar3_dtor.cpp's own CDestructable::~CDestructable note
    //  documents) that this toolchain's fixed /GS- /EHs-c- cannot
    //  reproduce (docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped
    //  frame this toolchain cannot reproduce"). Not virtual - same
    //  reasoning as CAgent::~CAgent() (agent.h).
    ~CAbility();

    //  slot 1 / +0x4 (0x6F052A10)
    virtual CAbility* DeleteSelf(int flags);

    //  slot 7 / +0x1C (0x6F021200)
    virtual unsigned int GetAgileTypeId() const;

    //  slot 9 / +0x24 (0x6F02DF20)
    virtual int Method_0x24(int context);

    //  slot 10 / +0x28 (0x6F02E390)
    virtual int GetRecordVersion(int context);

    //  slot 16 / +0x40 (0x6F02E4B0).  Named RefreshOwningPlayerColor, not
    //  Method_6F02E4B0: CAgent::RefreshOwningPlayerColor (agent.h) already
    //  owns this exact slot, and C++ only treats a derived virtual as an
    //  override - landing it at the base's own index - when the name and
    //  signature match exactly.  A differently-named virtual here would
    //  compile clean but silently append as a brand new slot at the end of
    //  the table instead of overriding slot 16, shifting every slot after
    //  it - exactly the check_vtables.py-catchable defect this comment
    //  exists to keep from recurring.  The real body (ability_slots.cpp)
    //  does not refresh any player color; the name is the base class's own,
    //  not a description of what this override does.
    virtual void RefreshOwningPlayerColor();

    //  slot 17 / +0x44 (0x6F02E3A0)
    virtual void DumpState(void* sink);

    //  slot 18 / +0x48 (0x6F02E410)
    virtual void SaveFlags(CDataStoreScratch* store);

    //  slot 19 / +0x4C (0x6F040AE0).  Named LoadFlags, not
    //  MigrateLegacyTypeId, for the identical reason RefreshOwningPlayerColor
    //  above is: CAgentWar3::LoadFlags (agentwar3.h) already owns this slot,
    //  and only an exact name+signature match overrides it in place. The
    //  real body (ability_typemigrate.cpp) does not load flags - it is a
    //  legacy FourCC type-id migration - the name is the base class's own.
    virtual void LoadFlags(class CDataStore* store);

    //  slot 22 / +0x58 (0x6F0211B0)
    virtual const char* GetClassName();

    //  slot 23 / +0x5C (0x6F052260)
    virtual void Method_0x5C();

    unsigned int m_field2C;      // +0x2C
    //  +0x30..+0x57 - ten dwords, all zeroed individually by the
    //  constructor (not a `rep stosd`/memset in the dump) and never read
    //  back by this call tree; named rather than left an opaque byte
    //  block only because the constructor genuinely writes each one.
    unsigned int m_field30;      // +0x30
    //  +0x34 - the ability's own type rawcode.  Not a guess: this is the
    //  same offset unitinventory.h's own CAbilityInventory::m_typeId
    //  independently names ("the ability's own type rawcode, the key
    //  GetLevelField looks its record up by") - that struct is a raw-
    //  offset view onto the same object this class models, reached before
    //  this session's own CAbility::GetTypeIdString (ability_gettypeidstring.cpp,
    //  0x6F02E130, vtable slot 111) read it back out as a packed FourCC.
    unsigned int m_typeId;       // +0x34
    unsigned int m_field38;      // +0x38
    unsigned int m_field3C;      // +0x3C
    unsigned int m_field40;      // +0x40
    unsigned int m_field44;      // +0x44
    unsigned int m_field48;      // +0x48
    unsigned int m_field4C;      // +0x4C
    unsigned int m_field50;      // +0x50
    unsigned int m_field54;      // +0x54
    CAgentTimer  m_timer;        // +0x58

    //========================================================================
    //  Full 189-slot vtable, in shipped order (docs/targets/vtables/CAbility.txt).
    //  C++ assigns vtable index strictly from declaration order, so every slot
    //  from the first CAbility-level override through the last CAbility-level
    //  slot has to be declared virtual here, in this exact order - a slot
    //  skipped or reordered silently shifts every slot after it, which
    //  check_vtables.py catches but no individual function score can. Bodies
    //  for the named ones live in ability_slots.cpp/ability_*.cpp as before;
    //  this only adds `virtual` and fixes declaration order - no signature
    //  changed. Slots 0-31/CAgentWar3-and-earlier that CAbility does not
    //  override are not declared here at all - C++ inherits the base class's
    //  own function pointer at that index automatically.
    //========================================================================

    //  slot 32 / +0x80 (0x6F052B10)
    virtual void Method_6F052B10(void* arg0, void* recordField);

    //  slot 33 / +0x84 (0x6F052AF0)
    virtual void Method_6F052AF0(void* target);

    //  slot 34 / +0x88 (None, nullsub_83) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x88(int) { }

    //  slot 35 / +0x8C (None, nullsub_84) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x8C(int) { }

    //  slot 36 / +0x90 (0x6F052220)
    virtual int   Method_6F052220();

    //  slot 37 / +0x94 (None, nullsub_85) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x94() { }

    //  slot 38 / +0x98 (0x6F052230)
    virtual int   Method_6F052230(int);

    //  slot 39 / +0x9C (None, nullsub_199) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x9C(int) { }

    //  slot 40 / +0xA0 (0x6F052250)
    virtual int   Method_6F052250();

    //  slot 41 / +0xA4 (0x6F05F550)
    virtual void Method_6F05F550();

    //  slot 42 / +0xA8 (0x6F03FF40)
    virtual void  SetTypeId(unsigned int typeId);

    //  slot 43 / +0xAC (None, nullsub_200) - real empty body in the shipped image
    //  (a bare `retn 0xc`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xAC(int, int, int) { }

    //  slot 44 / +0xB0 (None, nullsub_201) - real empty body in the shipped image
    //  (a bare `retn 0xc`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xB0(int, int, int) { }

    //  slot 45 / +0xB4 (None, nullsub_202) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xB4(int) { }

    //  slot 46 / +0xB8 (None, nullsub_203) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xB8(int) { }

    //  slot 47 / +0xBC (0x6F03FFA0)
    virtual void  Method_6F03FFA0(int delta);

    //  slot 48 / +0xC0 (0x6F03FFD0)
    virtual void  Method_6F03FFD0(int delta);

    //  slot 49 / +0xC4 (None, nullsub_204) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xC4() { }

    //  slot 50 / +0xC8 (None, nullsub_205) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xC8() { }

    //  slot 51 / +0xCC (0x6F0522E0)
    virtual int Method_6F0522E0() { return 1; }

    //  slot 52 / +0xD0 (0x6F052990)
    virtual int HasPendingTypeUpgrade();

    //  slot 53 / +0xD4 (0x6F0522F0)
    virtual int Method_6F0522F0() { return 1; }

    //  slot 54 / +0xD8 (0x6F052300)
    virtual int Method_6F052300() { return 0; }

    //  slot 55 / +0xDC (0x6F052310)
    virtual int Method_6F052310() { return 0; }

    //  slot 56 / +0xE0 (None, nullsub_86) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xE0() { }

    //  slot 57 / +0xE4 (None, nullsub_87) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xE4() { }

    //  slot 58 / +0xE8 (None, nullsub_88) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xE8() { }

    //  slot 59 / +0xEC (None, nullsub_89) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xEC() { }

    //  slot 60 / +0xF0 (0x6F052320)
    virtual int   Method_6F052320();

    //  slot 61 / +0xF4 (None, nullsub_206) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0xF4() { }

    //  slot 62 / +0xF8 (0x6F052340)
    virtual int   Method_6F052340();

    //  slot 63 / +0xFC (0x6F052350)
    virtual int   Method_6F052350(int);

    //  slot 64 / +0x100 (None, nullsub_90) - real empty body in the shipped image
    //  (a bare `retn 0x8`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x100(int, int) { }

    //  slot 65 / +0x104 (0x6F052360)
    virtual int   Method_6F052360();

    //  slot 66 / +0x108 (0x6F052370)
    virtual int   Method_6F052370(int);

    //  slot 67 / +0x10C (0x6F052380)
    virtual int   Method_6F052380();

    //  slot 68 / +0x110 (0x6F052390)
    virtual int   Method_6F052390(int);

    //  slot 69 / +0x114 (0x6F0523A0)
    virtual int   Method_6F0523A0();

    //  slot 70 / +0x118 (0x6F0523B0)
    virtual int   Method_6F0523B0(int);

    //  slot 71 / +0x11C (0x6F0523C0)
    virtual class CFloat* Method_6F0523C0(class CFloat* out);

    //  slot 72 / +0x120 (0x6F0523D0)
    virtual class CFloat* Method_6F0523D0(class CFloat* out);

    //  slot 73 / +0x124 (None, nullsub_207) - real empty body in the shipped image
    //  (a bare `retn 0xc`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x124(int, int, int) { }

    //  slot 74 / +0x128 (None, nullsub_208) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x128(int) { }

    //  slot 75 / +0x12C (None, nullsub_209) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x12C(int) { }

    //  slot 76 / +0x130 (0x6F052410)
    virtual int   Method_6F052410();

    //  slot 77 / +0x134 (0x6F052420)
    virtual int   Method_6F052420(int, int);

    //  slot 78 / +0x138 (0x6F052430)
    virtual int   Method_6F052430();

    //  slot 79 / +0x13C (0x6F0524A0)
    virtual int   Method_6F0524A0(int, int);

    //  slot 80 / +0x140 (0x6F052490)
    virtual int   Method_6F052490(int, int);

    //  slot 81 / +0x144 (0x6F052480)
    virtual int DefaultIntFieldQueryA(int, int);

    //  slot 82 / +0x148 (0x6F052470)
    virtual int DefaultIntFieldQueryB(int, int);

    //  slot 83 / +0x14C (0x6F052460)
    virtual int DefaultIntFieldQueryC(int, int);

    //  slot 84 / +0x150 (0x6F052450)
    virtual int DefaultIntFieldQueryD(int, int);

    //  slot 85 / +0x154 (0x6F052440)
    virtual int DefaultIntFieldQueryE(int, int);

    //  slot 86 / +0x158 (0x6F0524B0)
    virtual class CFloat* DefaultFloatFieldQueryA(class CFloat*, int, int);

    //  slot 87 / +0x15C (0x6F0524C0)
    virtual class CFloat* DefaultFloatFieldQueryB(class CFloat*, int, int);

    //  slot 88 / +0x160 (0x6F0524D0)
    virtual class CFloat* DefaultFloatFieldQueryC(class CFloat*, int);

    //  slot 89 / +0x164 (0x6F0524E0)
    virtual int DefaultIntFieldQueryF(int, int);

    //  slot 90 / +0x168 (0x6F0524F0)
    virtual int DefaultIntFieldQueryG(int, int);

    //  slot 91 / +0x16C (0x6F052500)
    virtual int DefaultIntFieldQueryH(int);

    //  slot 92 / +0x170 (None, nullsub_210) - real empty body in the shipped image
    //  (a bare `retn 0xc`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x170(int, int, int) { }

    //  slot 93 / +0x174 (None, nullsub_211) - real empty body in the shipped image
    //  (a bare `retn 0x18`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x174(int, int, int, int, int, int) { }

    //  slot 94 / +0x178 (None, nullsub_212) - real empty body in the shipped image
    //  (a bare `retn 0x18`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x178(int, int, int, int, int, int) { }

    //  slot 95 / +0x17C (0x6F052540)
    virtual int DefaultFieldTypeTag();

    //  slot 96 / +0x180 (0x6F052550)
    virtual int DefaultIntFieldQueryI(int, int, int);

    //  slot 97 / +0x184 (0x6F052560)
    virtual class CFloat* DefaultFloatFieldQueryD(class CFloat*);

    //  slot 98 / +0x188 (0x6F052570)
    virtual int DefaultIntFieldQueryJ();

    //  slot 99 / +0x18C (0x6F052580)
    virtual int DefaultIntFieldQueryK();

    //  slot 100 / +0x190 (0x6F052590)
    virtual int DefaultIntFieldQueryL();

    //  slot 101 / +0x194 (0x6F0525A0)
    virtual int DefaultIntFieldQueryM();

    //  slot 102 / +0x198 (0x6F0525B0)
    virtual int DefaultIntFieldQueryN();

    //  slot 103 / +0x19C (0x6F0525C0)
    virtual int DefaultIntFieldQueryO();

    //  slot 104 / +0x1A0 (0x6F0525D0)
    virtual int DefaultIntFieldQueryP();

    //  slot 105 / +0x1A4 (None, nullsub_213) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1A4(int) { }

    //  slot 106 / +0x1A8 (None, nullsub_214) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1A8(int) { }

    //  slot 107 / +0x1AC (None, nullsub_215) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1AC() { }

    //  slot 108 / +0x1B0 (None, nullsub_91) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1B0() { }

    //  slot 109 / +0x1B4 (None, nullsub_216) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1B4() { }

    //  slot 110 / +0x1B8 (0x6F052620)
    virtual int DefaultIntFieldQueryQ();

    //  slot 111 / +0x1BC (0x6F02E130)
    virtual char* GetTypeIdString(char* buffer, int size);

    //  slot 112 / +0x1C0 (None, nullsub_92) - real empty body in the shipped image
    //  (a bare `retn 0x8`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1C0(int, int) { }

    //  slot 113 / +0x1C4 (None, nullsub_93) - real empty body in the shipped image
    //  (a bare `retn 0x8`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1C4(int, int) { }

    //  slot 114 / +0x1C8 (None, nullsub_217) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1C8() { }

    //  slot 115 / +0x1CC (None, nullsub_218) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1CC() { }

    //  slot 116 / +0x1D0 (None, nullsub_219) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1D0() { }

    //  slot 117 / +0x1D4 (None, nullsub_220) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x1D4() { }

    //  slot 118 / +0x1D8 (0x6F052670)
    virtual int DefaultIntFieldQueryR();

    //  slot 119 / +0x1DC (0x6F052680)
    virtual int DefaultIntFieldQueryS();

    //  slot 120 / +0x1E0 (0x6F052690)
    virtual int DefaultIntFieldQueryT();

    //  slot 121 / +0x1E4 (0x6F0526A0)
    virtual int DefaultIntFieldQueryU();

    //  slot 122 / +0x1E8 (0x6F0526B0)
    virtual int DefaultIntFieldQueryV();

    //  slot 123 / +0x1EC (0x6F0526C0)
    virtual int DefaultIntFieldQueryW();

    //  slot 124 / +0x1F0 (0x6F0526D0)
    virtual int DefaultIntFieldQueryX();

    //  slot 125 / +0x1F4 (0x6F0526E0)
    virtual int DefaultIntFieldQueryY();

    //  slot 126 / +0x1F8 (0x6F0526F0)
    virtual int DefaultIntFieldQueryZ();

    //  slot 127 / +0x1FC (0x6F052700)
    virtual int DefaultIntFieldQueryAA();

    //  slot 128 / +0x200 (0x6F052710)
    virtual int DefaultIntFieldQueryAB();

    //  slot 129 / +0x204 (0x6F052720)
    virtual int DefaultIntFieldQueryAJ();

    //  slot 130 / +0x208 (0x6F052730)
    virtual int DefaultIntFieldQueryAK();

    //  slot 131 / +0x20C (0x6F0529F0)
    virtual int Method_6F0529F0();

    //  slot 132 / +0x210 (0x6F052740)
    virtual int DefaultIntFieldQueryAL();

    //  slot 133 / +0x214 (0x6F052750)
    virtual int DefaultIntFieldQueryAM();

    //  slot 134 / +0x218 (0x6F052760)
    virtual int DefaultIntFieldQueryAN();

    //  slot 135 / +0x21C (0x6F052770)
    virtual int DefaultIntFieldQueryAO(int);

    //  slot 136 / +0x220 (0x6F052790)
    virtual int Method_6F052790(int, int, int);

    //  slot 137 / +0x224 (0x6F052780)
    virtual int Method_6F052780(int, int);

    //  slot 138 / +0x228 (0x6F020B30)
    virtual int   Method_6F020B30(int);

    //  slot 139 / +0x22C (0x6F0527A0)
    virtual int DefaultIntFieldQueryAP(int);

    //  slot 140 / +0x230 (0x6F021710)
    virtual int   Method_6F021710();

    //  slot 141 / +0x234 (0x6F021720)
    virtual class CFloat* Method_6F021720(class CFloat* out);

    //  slot 142 / +0x238 (0x6F0527B0)
    virtual int DefaultIntFieldQueryAQ();

    //  slot 143 / +0x23C (0x6F0527C0)
    virtual int DefaultIntFieldQueryAR(int, int);

    //  slot 144 / +0x240 (None, nullsub_221) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x240(int) { }

    //  slot 145 / +0x244 (0x6F0527E0)
    virtual int Method_6F0527E0(int, int, int);

    //  slot 146 / +0x248 (0x6F0527F0)
    virtual int DefaultIntFieldQueryAS(int);

    //  slot 147 / +0x24C (0x6F052860)
    virtual int Method_6F052860(int, int, int, int, int);

    //  slot 148 / +0x250 (0x6F052850)
    virtual int Method_6F052850(int, int, int, int);

    //  slot 149 / +0x254 (0x6F052840)
    virtual int Method_6F052840(int, int, int, int, int, int);

    //  slot 150 / +0x258 (0x6F052830)
    virtual int Method_6F052830(int, int, int, int, int);

    //  slot 151 / +0x25C (0x6F052820)
    virtual int Method_6F052820(int, int, int, int, int);

    //  slot 152 / +0x260 (0x6F052810)
    virtual int Method_6F052810(int, int, int, int);

    //  slot 153 / +0x264 (0x6F052800)
    virtual int Method_6F052800(int, int, int);

    //  slot 154 / +0x268 (0x6F052C30)
    virtual int EvaluateDepCheckAlias(int* outFlag);

    //  slot 155 / +0x26C (0x6F052870)
    virtual int DefaultIntFieldQueryAC(int, int);

    //  slot 156 / +0x270 (0x6F052880)
    virtual int DefaultIntFieldQueryAD(int);

    //  slot 157 / +0x274 (0x6F052890)
    virtual int DefaultIntFieldQueryAE();

    //  slot 158 / +0x278 (0x6F0528A0)
    virtual int DefaultIntFieldQueryAF();

    //  slot 159 / +0x27C (0x6F0528B0)
    virtual int DefaultIntFieldQueryAG();

    //  slot 160 / +0x280 (0x6F0528C0)
    virtual int DefaultIntFieldQueryAH();

    //  slot 161 / +0x284 (None, nullsub_222) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x284() { }

    //  slot 162 / +0x288 (None, nullsub_223) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x288() { }

    //  slot 163 / +0x28C (0x6F0528F0)
    virtual int IdentityIntFieldQuery(int value);

    //  slot 164 / +0x290 (0x6F02E150)
    virtual int   Method_6F02E150() const;

    //  slot 165 / +0x294 (0x6F02E160)
    virtual void NotifyAttachmentRender(void* a0, void* a4, void* a8, void* aC,
                                     void* a10, void* a14, void* a18,
                                     void* a1C, void* a20, unsigned int ownerIdArg);

    //  slot 166 / +0x298 (None, nullsub_94) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x298(int) { }

    //  slot 167 / +0x29C (0x6F040030)
    virtual void* GetEffectArt(int index);

    //  slot 168 / +0x2A0 (0x6F040080)
    virtual void* GetAreaEffectArt(int index);

    //  slot 169 / +0x2A4 (0x6F040110)
    virtual void* GetTargetArt(int index);

    //  slot 170 / +0x2A8 (0x6F0400D0)
    virtual void* GetEffectSound();

    //  slot 171 / +0x2AC (0x6F0400F0)
    virtual void* GetEffectSoundLooped();

    //  slot 172 / +0x2B0 (0x6F02E220)
    virtual void* GetPlacementModel();

    //  slot 173 / +0x2B4 (0x6F02E230)
    virtual const char* GetPlacementCursorTexture();

    //  slot 174 / +0x2B8 (0x6F052900)
    virtual class CFloat* DefaultFloatFieldQueryE(class CFloat* out);

    //  slot 175 / +0x2BC (0x6F052910)
    virtual class CFloat* DefaultFloatFieldQuery400(class CFloat* out);

    //  slot 176 / +0x2C0 (0x6F021300)
    virtual unsigned int* GetTargetLightColor(unsigned int* out);

    //  slot 177 / +0x2C4 (0x6F0407E0)
    virtual class CFloat* GetMissileArc(class CFloat* out);

    //  slot 178 / +0x2C8 (0x6F0407B0)
    virtual class CFloat* GetMissileSpeed(class CFloat* out);

    //  slot 179 / +0x2CC (0x6F040810)
    virtual int GetMissileHoming();

    //  slot 180 / +0x2D0 (0x6F052930)
    virtual int DefaultIntFieldQueryAI();

    //  slot 181 / +0x2D4 (None, nullsub_224) - real empty body in the shipped image
    //  (a bare `retn 0x8`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x2D4(int, int) { }

    //  slot 182 / +0x2D8 (None, nullsub_225) - real empty body in the shipped image
    //  (a bare `retn 0x4`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x2D8(int) { }

    //  slot 183 / +0x2DC (0x6F052960)
    virtual class CFloat* DefaultFloatFieldQueryF(class CFloat* out, int);

    //  slot 184 / +0x2E0 (0x6F052970)
    virtual class CFloat* DefaultFloatFieldQueryG(class CFloat* out, int);

    //  slot 185 / +0x2E4 (0x6F041700)
    virtual void IncrementLevel();

    //  slot 186 / +0x2E8 (0x6F041720)
    virtual void DecrementLevel();

    //  slot 187 / +0x2EC (0x6F02E890)
    virtual class CFloat* QueryLevelRowField14(class CFloat* out, int level);

    //  slot 188 / +0x2F0 (None, nullsub_226) - real empty body in the shipped image
    //  (a bare `retn 0`, confirmed via tools/ida_query.py bytes) - not a
    //  guessed placeholder. Named by offset, matching this file's own
    //  Method_0x24/Method_0x5C convention, since nothing in this call tree
    //  identifies what it once did.
    virtual void Method_0x2F0() { }

    //========================================================================
    //  Private helpers - not vtable slots themselves, reached only by an
    //  ordinary `call` from one or more of the slots above.
    //========================================================================

    //  0x6F02E6A0 - not a vtable slot itself (SetTypeId reaches it
    //  through an ordinary `call`), but real work: see ability_slots.cpp.
    //  Same record-caching shape as CAbilityInventory::GetLevelField
    //  (abilitylevelfield.cpp) - resolves m_typeId to its type record via
    //  LookupAbilityTypeRecord and caches it at m_field54 - but reads the
    //  level row's raw dword at offset 0 rather than the CFloat at +0x20.
    int QueryLevelRowRaw(int level);

    //  0x6F02E570/0x6F02E840 - HasPendingTypeUpgrade's (slot 52) own two
    //  blocking closure members, not vtable slots themselves - see
    //  ability_slots.cpp.
    unsigned int QueryUpgradeStepCount();
    int QueryLevelRowField10(int level);

    //  0x6F02E640 - the shared "resolve and cache the ability's generic
    //  field-lookup record" helper every one of GetEffectArt/
    //  GetAreaEffectArt/GetTargetArt/GetEffectSound/GetEffectSoundLooped/
    //  GetMissileSpeed/GetMissileArc/GetMissileHoming (ability_fieldart.cpp)
    //  calls first.  Not a vtable slot itself - reached only by an ordinary
    //  `call`, same as QueryLevelRowRaw above, but this one needs a real
    //  body since every one of those eight slots calls it for real and it
    //  is not otherwise covered.
    void* GetCachedFieldRecord();

    //  0x6F052A60 - a private helper (not itself a vtable slot): point
    //  this ability's tracked-target link (CAgent::SetTrackedTarget) at
    //  `target`, reset the +0x24/+0x28 handle-ref pair CUnitRefList owns,
    //  notify slot 57, remember the target at m_field30, flag+notify slot
    //  50 when the target's own +0x5C bit 2 is set, and - only when the
    //  type-record cache is still empty and both slot-120/121 gates answer
    //  0 - refresh it the same way SetTypeId's own tail does.  Reached by
    //  both slot 32 and slot 33.  See ability_slots.cpp.
    void Method_6F052A60(void* target);

    //  0x6F02E540 - a private helper (not itself a vtable slot), the same
    //  "refresh the type-record cache, hand back one word of it or the
    //  sentinel 1" shape as Method_6F02E5A0 below, reading +0x38 instead
    //  of +0x40.  See ability_slots.cpp.
    int Method_6F02E540();

    //  0x6F02E5A0 - a private helper (not itself a vtable slot): the same
    //  type-record-cache refresh Method_6F02E540 above makes, reading
    //  +0x40 instead of +0x38.  See ability_slots.cpp.
    int Method_6F02E5A0();
};

#endif
