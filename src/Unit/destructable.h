//============================================================================
//  CDestructable - the class at 0x6F92EAF4 (`??_7CDestructable@@6B@`,
//  109 slots).
//
//  **Derives from CSelectable.**  CSelectable has 107 slots, CDestructable
//  has 109, and CDestructable's first 107 are CSelectable's 107 at the
//  same index, with 32 of them overridden and the rest inherited verbatim -
//  the single-inheritance override pattern, confirmed index by index
//  against user_knowledge.json's own two vtable dumps (identical to the
//  methodology docs/targets/CWidget_CSelectable_CItem__vtables.md and
//  unit.h already used for CItem/CUnit).  Slots 107 and 108 are two new
//  slots CDestructable adds on top.
//
//  Two independent confirmations, the same two CUnit's own header uses:
//
//    * slot 22 (0x6F266140) returns the literal string "CDestructable";
//    * slot 7 (0x6F266190, already reconstructed before this session in
//      unit_destructable_typeid.cpp) returns '+w3d' = 0x2B773364, the same
//      GetAgileTypeId shape every class in this family uses.
//
//  Only slot 7's body predates this header; it is wired in below as the
//  override the class declaration says it has to be, rather than rewritten.
//
//  ## Fields CDestructable adds at +0x54
//
//  CSelectable ends at +0x54 (selectable.h).  From there:
//
//    m_reserved54/m_reserved55  - two bytes, Save's/Load's own order
//                                 (destructable_save.cpp) - nothing else in
//                                 this sweep reads them back, so they carry
//                                 no name beyond their offset.
//    m_ref58        +0x58 - a `CAgentTimer*`.  Deactivate (slot 13) cancels
//                            it and drops a reference through the same
//                            "test/decrement/dispatch slot 0" shape
//                            unit_dtorbody.cpp's own ReleaseRef uses, and
//                            ~CDestructable (slot 1) drops the same
//                            reference again on the way out - see
//                            agentwar3_dtor.cpp (with CAgentWar3::~CAgentWar3, for the inlining).
//    m_reservedFloat5C +0x5C - a plain hardware float (not a CFloat -
//                            Save/Load `fld`/`fstp`/ReadFloat/WriteFloat it
//                            directly, never through WriteCFloat/ReadCFloat
//                            the way m_floatB's three fields are).
//    m_floatB       +0x60 - a FloatMiniB (floatmini.h), the same
//                            {value, min, max} "life" triple CItem's own
//                            +0x54 is, just at a different absolute offset
//                            because CDestructable's own fields start
//                            later.  Slots 73-77/80/81 (SetLife, AddLife,
//                            GetLife, GetMaxLife, GetMinLife, SetMinLife,
//                            SetMaxLife) are its accessors, mirroring
//                            item_valueset.cpp/item_load.cpp/item_save.cpp
//                            almost verbatim.
//    m_handleObject +0x70 - an SHandleWithType (handlemoved.h), the same
//                            "GetHandleObject" sub-object CWidget/CItem/
//                            CUnit each carry at their own offset (slot 46).
//                            It really is TRefCnt-derived (8-byte header,
//                            not just the 8-byte "reserved" handlemoved.h
//                            already declares): ~CDestructable's own body
//                            stamps `??_7TRefCnt@@6B@` over it on the way
//                            out.  Slots 8/9 (Method_0x20/Method_0x24) also
//                            reach it through `Position::RecordSlot8`/
//                            Position's own vtable slot 3
//                            (0x6F473980/[vtbl+0xC]) - the same two shipped
//                            functions CItem's own slots 8/9 call on
//                            `m_position` (item_vtable2.cpp).  Those two
//                            functions only ever touch fields inside the
//                            first sixteen bytes of whatever object they
//                            are handed, which is why the same compiled
//                            body works unmodified on an object this small;
//                            reached here through a reinterpret cast to
//                            `Position*`, not a second, real `Position`
//                            member (there is no room for one - the whole
//                            sub-object is only 0x10 bytes).
//    m_reservedFloat80 +0x80 - another plain hardware float (Save/Load
//                            again touch it directly, and slot 108
//                            (0x6F267250) hands it straight back through
//                            the FPU with a covariant-looking `retn 0Ch`
//                            that just means three dead stack dwords - see
//                            destructable_vtable_slots.cpp).
//    m_positionNotifyHandle +0x84 - a dword, this class's own
//                            GetPositionNotifyHandle/SetPositionNotifyHandle
//                            pair (slots 92/93), the same property CWidget's
//                            own slots 92/93 expose for `m_position` in the
//                            CItem/CUnit chain - just, again, at a
//                            different offset because there is no real
//                            Position member here to read it off.
//
//  ## The type-data table slots (66/69/94/95)
//
//  Four slots (66, 69, 94, 95) tail-jump into a small shared lookup that
//  hashes `m_footprintType` (CWidget's own +0x30, agiletype.h's
//  AGILE_TYPE_ID::Hash - the exact function CFootprintTypeData's own
//  registry uses, agileid.cpp) and looks the result up in a second,
//  distinct TSHashTable<CDestructableTypeData, AGILE_TYPE_ID> instance at
//  dword_6FAB4218 - the *same* template storm.h's TSHashTable<T,K>::Ptr
//  already gives a body for, instantiated for a new node type rather than
//  reusing CFootprintTypeData's own instantiation (which is how the
//  lookup lands at its own new address, 0x6F2686C0, instead of at
//  CFootprintTypeData's 0x6F319810).  See destructabletypedata.h.
//============================================================================
#ifndef DESTRUCTABLE_H
#define DESTRUCTABLE_H

#include "selectable.h"
#include "war3image.h"
#include "timesyncbounds.h"   // SHandleWithType's fuller definition - handlemoved.h
                              // gives the same shape under the same name, and
                              // the two collide if both are ever included in
                              // one TU (agentwar3_dtor.cpp, which also needs
                              // CAgentWar3's own headers, is where this first
                              // came up); this one is already reachable from
                              // there, so it is the one used consistently.
#include "floatmini.h"

class CDataStore;
class CDataStoreScratch;
class CAgentTimer;
class Position;

class CDestructable : public CSelectable
{
public:
    //  0x6F267420 - agentwar3_dtor.cpp (see that file for why).  Not virtual: a derived class
    //  calls its base's destructor directly at compile time, and a
    //  virtual one here would add a slot and shift every one of
    //  CDestructable's own 109.
    ~CDestructable();

    //====================================================================
    //  CDestructable's vtable contribution.  Declaration order *is*
    //  vtable order - see selectable.h's own note.  Slots 1-108; see
    //  docs/targets/CDestructable__vtable.md.
    //
    //  Every name here is the base class's name for the same slot, per
    //  the same convention unit.h and item.h use; only the two slots this
    //  class adds (107-108) get new names.
    //====================================================================
    //  slot 1 / +0x04 (0x6F2674D0) - destructable_deleteself.cpp
    virtual CDestructable* DeleteSelf(int flags);
    //  slot 3 / +0x0C (0x6F26A4A0) - destructable_vtable_slots.cpp
    virtual int Method_0x0C(const struct SAgentMessage* msg);
    //  slot 7 / +0x1C (0x6F266190) - unit_destructable_typeid.cpp
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F2661E0) - destructable_vtable_slots.cpp
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F266820) - destructable_vtable_slots.cpp
    virtual int Method_0x24(int context);
    //  slot 10 / +0x28 (0x6F267370) - destructable_vtable_slots.cpp
    virtual int GetRecordVersion(int context);
    //  slot 13 / +0x34 (0x6F2668A0) - destructable_vtable_slots.cpp
    virtual void Deactivate();
    //  slot 14 / +0x38 (0x6F2662A0) - destructable_save.cpp
    virtual void Save(CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F267B20) - destructable_save.cpp
    virtual void Load(CDataStore* store);
    //  slot 16 / +0x40 (0x6F26A170) - destructable_ownercolor.cpp
    virtual void RefreshOwningPlayerColor();
    //  slot 17 / +0x44 (0x6F267280) - destructable_vtable_slots.cpp
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F267380) - destructable_save.cpp
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F2673C0) - destructable_save.cpp
    virtual void LoadFlags(CDataStore* store);
    //  slot 22 / +0x58 (0x6F266140) - destructable_vtable_slots.cpp
    virtual const char* GetClassName();
    //  slot 46 / +0xB8 (0x6F267240) - destructable_vtable_slots.cpp
    virtual SHandleWithType* GetHandleObject();
    //  slot 47 / +0xBC (0x6F266330) - destructable_vtable_slots.cpp
    virtual float Method_0xBC();
    //  slot 60 / +0xF0 (0x6F267490) - destructable_vtable_slots.cpp
    virtual int QueryVisible(int which);
    //  slot 61 / +0xF4 (0x6F266300) - destructable_vtable_slots.cpp
    virtual void RefreshSpriteVisibility();
    //  slot 65 / +0x104 (0x6F269120) - destructable_terraintint.cpp
    virtual void RefreshTerrainTint();
    //  slot 66 / +0x108 (0x6F269660) - destructabletypedata.cpp
    virtual float Method_0x108();
    //  slot 69 / +0x114 (0x6F269630) - destructabletypedata.cpp
    virtual int Method_0x114();
    //  slot 72 / +0x120 (0x6F26A4C0) - destructable_offset.cpp
    virtual void* NegateAndForwardOffset(void* target, struct SWidgetOffsetSource* src,
                                         int a3, int a4);
    //  slot 73 / +0x124 (0x6F26B100) - destructable_life.cpp
    virtual void SetLife(const CFloat* value);
    //  slot 74 / +0x128 (0x6F26B160) - destructable_life.cpp
    virtual CFloat AddLife(const CFloat* delta);
    //  slot 75 / +0x12C (0x6F2672E0) - destructable_vtable_slots.cpp
    virtual CFloat* GetLife(CFloat* out);
    //  slot 76 / +0x130 (0x6F2672F0) - destructable_vtable_slots.cpp
    virtual CFloat* GetMaxLife(CFloat* out);
    //  slot 77 / +0x134 (0x6F267300) - destructable_vtable_slots.cpp
    virtual CFloat* GetMinLife(CFloat* out);
    //  slot 80 / +0x140 (0x6F267310) - destructable_vtable_slots.cpp
    virtual void SetMinLife(const CFloat* value);
    //  slot 81 / +0x144 (0x6F267340) - destructable_vtable_slots.cpp
    virtual void SetMaxLife(const CFloat* value);
    //  slot 92 / +0x170 (0x6F267260) - destructable_vtable_slots.cpp
    virtual int GetPositionNotifyHandle();
    //  slot 93 / +0x174 (0x6F267270) - destructable_vtable_slots.cpp
    virtual void SetPositionNotifyHandle(int handle);
    //  slot 94 / +0x178 (0x6F269640) - destructabletypedata.cpp
    virtual unsigned int* Method_0x178(unsigned int* out);
    //  slot 95 / +0x17C (0x6F269620) - destructabletypedata.cpp
    virtual unsigned int Method_0x17C();
    //  slot 107 / +0x1AC (0x6F2672D0) - the first slot CDestructable adds.
    //  Bit 4 of CAgentWar3's own m_flags (+0x20), the same word slots 40-42
    //  (SetFlagBit2/4/8) already expose other bits of - destructable_vtable_slots.cpp
    virtual int IsFlagBit10Set();
    //  slot 108 / +0x1B0 (0x6F267250) - the second slot CDestructable adds.
    //  Hands back m_reservedFloat80 through the FPU; `retn 0Ch` is three
    //  dead stack dwords that this reconstruction has to declare and clean
    //  even though nothing in this call tree's one caller-less leaf shows
    //  what they would have been (abi_audit.py's ret-count check, not a
    //  claim about their real types) - See destructable_vtable_slots.cpp.
    virtual float GetReservedFloat80(int, int, int);

    //  0x6F266190 - unit_destructable_typeid.cpp's own body, wired in as
    //  this class's slot 7 override rather than rewritten.  Declared for
    //  reference only; unit_destructable_typeid.cpp's own minimal
    //  `class CDestructable : public CAgentWar3` is gone as of this
    //  header - see that file's own updated comment.

    //  +0x00-+0x53: CAgent/CAgentWar3/CWar3Image/CWidget/CSelectable (base
    //  classes above).  See this header's own class comment for what each
    //  field from +0x54 on is and the evidence for it.
    unsigned char m_reserved54;    // +0x54
    unsigned char m_reserved55;    // +0x55
    unsigned short m_reserved56;   // +0x56 - gap; nothing in this sweep touches it
    CAgentTimer*  m_ref58;         // +0x58
    float         m_reservedFloat5C; // +0x5C
    FloatMiniB    m_floatB;        // +0x60
    SHandleWithType m_handleObject; // +0x70
    float         m_reservedFloat80; // +0x80
    unsigned int  m_positionNotifyHandle; // +0x84
};

#endif
