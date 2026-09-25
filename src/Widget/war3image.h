//============================================================================
//  CWar3Image - the class between CAgentWar3 and CWidget.
//
//  It is not a guess.  `??_R2CWidget@@8`, CWidget's own RTTI base-class
//  array, lists exactly six entries in order:
//
//      CWidget -> CWar3Image -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  and `??_R2CWar3Image@@8` lists the same chain from CWar3Image down.  It
//  has a vtable of its own at 0x6F877D3C (`??_7CWar3Image@@6B@`, 39 slots)
//  which extends CAgentWar3's 30 by nine and overrides seven of them, and
//  CWidget's 96 extend *those* 39.  Every one of those numbers was read off
//  the live database with `tools/ida_query.py vtable`, not inferred.
//
//  Its two fields come straight out of CWidget's own constructor
//  (0x6F2ACAE0): between `call sub_6F001730` (CAgent::CAgent) and the
//  single `mov [esi], offset ??_7CWidget@@6B@`, the shipped code zeroes
//  +0x20, +0x24 and +0x28 - and +0x20 is CAgentWar3's m_flags
//  (agentwar3.h).  Stores that land *before* the derived vtable stamp come
//  from base constructors, so +0x24 and +0x28 are this class's, not
//  CWidget's, which is where they used to live.
//
//  That +0x28 is the sprite pointer is the part worth saying out loud: a
//  class named "War3 Image" owning the CSprite* is not a coincidence, and
//  it explains why the nine slots this class adds (30-38) are all sprite
//  and animation work - CWar3Image::PlaySpriteAnimation (0x6F26B820) and
//  friends sit here, not in CWidget.
//
//  Only one vtable store survives in the shipped constructor because MSVC
//  collapses the CAgentWar3, CWar3Image and CWidget stamps into the last
//  one when the intermediate constructors are inlined and nothing between
//  them can observe the pointer.  Adding this class therefore costs the
//  constructor nothing; it only changes which symbol that one store names.
//============================================================================
#ifndef WAR3IMAGE_H
#define WAR3IMAGE_H

#include "agentwar3.h"
#include "sprite.h"

//  Forward declarations for the slot signatures below - the vtable
//  block declares slots whose parameter types are defined further
//  down the include graph, and a pointer only needs the name.
class CDataStore;
class CDataStoreScratch;
struct SWidgetArtDescriptor;

class CWar3Image : public CAgentWar3
{
public:
    //====================================================================
    //  CWar3Image's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 1-38; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  slot 1 / +0x04 (0x6F03FAC0, sub_6F03FAC0)
    virtual CWar3Image* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F26B7B0, sub_6F26B7B0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 13 / +0x34 (0x6F26B7D0)
    virtual void Deactivate();
    //  slot 14 / +0x38 (0x6F26BAF0)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F26BB20)
    virtual void Load(class CDataStore* store);
    //  slot 16 / +0x40 (0x6F26BB70)
    virtual void RefreshOwningPlayerColor();
    //  slot 22 / +0x58 (0x6F26B760, sub_6F26B760)
    virtual const char* GetClassName();
    //  slot 30 / +0x78 (0x6F26BCD0)
    virtual void CreateSpriteFromDescriptor(const SWidgetArtDescriptor* desc);
    //  slot 31 / +0x7C (0x6F26B920)
    virtual void SetModelByName(const char* modelName, unsigned int colourSlot);
    //  slot 32 / +0x80 (0x6F26BEA0)
    virtual void ClearPendingAndNotify();
    //  slot 33 / +0x84 (0x6F0520B0)
    virtual void SetPending();
    //  slot 34 / +0x88 (0x6F26B9E0)
    virtual void SetModelByHandle(unsigned int modelHandle, int notify);
    //  slot 35 / +0x8C (0x6F26BA90)
    virtual float PlayAnimation(unsigned int name, int flags, int notify);
    //  slot 36 / +0x90 (0x6F26BAC0)
    virtual float PlayAnimationLooping(unsigned int name, int flags, int notify);
    //  slot 37 / +0x94 (0x6F26B820)
    virtual void SetSpriteVisible(int force, int visible);
    //  slot 38 / +0x98 (0x6F03A9A0, nullsub_198)
    virtual void NotifyWidgetVisibilityChanged();

    //  Inlined at every call site, the same way CAgentWar3's own is: two
    //  stores and nothing else.
    CWar3Image() : m_reserved24(0), m_pSprite(0) {}

    //====================================================================
    //  Not vtable slots, but this class's all the same: every one of the
    //  three sits in the 0x6F26Bxxx neighbourhood CWar3Image's own slots
    //  occupy, and CWar3Image::SetModelByName / SetModelByHandle call
    //  them - a base cannot reach a derived class's members, so they
    //  were never CWidget's.
    //====================================================================
    //  0x6F26B8C0 / 0x6F26B890 - widgetplayer.cpp / widgetplayerstore.cpp.
    //  Both are genuine thiscall members, not __fastcall free functions:
    //  each takes every argument on the stack (`retn 4` and `retn 8`) with
    //  only `this` in ecx, where a __fastcall free function would have put
    //  the first of them in edx.
    void SetOwningPlayer(unsigned int player);
    void StoreOwningPlayer(unsigned int player, unsigned int slot);
    //  0x6F26B860 - the out-of-line half both of those call: nothing to do
    //  if this widget has no sprite yet.  widget_spriteplay.cpp.
    float PlaySpriteAnimation(const unsigned int* name, int flags, int extra);

    //  +0x24.  Whole-dword here because the constructor zeroes it as one
    //  store, but its low byte is an 8-bit bitfield: StoreOwningPlayer
    //  writes it as `mov byte ptr [ecx+24h],0` followed by a *32-bit*
    //  `or [ecx+24h],edx`, which is the read-modify-write MSVC emits for a
    //  bitfield at the bottom of a dword and never for a byte store or for
    //  `x = (x & ~0xFF) | v` (that one narrows the mask to
    //  `and dword ptr [...], 0FFFFFF00h`).  See SWidgetColorBits in
    //  widgetplayerstore.cpp, which is where that spelling lives - putting
    //  the bitfield in the class itself would split this field in two and
    //  cost the constructor its single store.
    unsigned int  m_reserved24;      // +0x24 - zeroed by the constructor

    //  +0x28 - zeroed by the constructor; CWidget::CreateSpriteFromDescriptor
    //  fills it in.  This class's own field, not CWidget's: see the header
    //  comment.
    CSprite*      m_pSprite;         // +0x28
};

#endif
