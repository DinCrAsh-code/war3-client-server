//============================================================================
//  CSprite - the visual model instance a CWidget owns through its own +0x28
//  (widget.h's m_reserved28, the object itemplacementdispatch.cpp already
//  reaches through vtable slots 0x18/0x1C to set position and orientation).
//
//  What it is was settled by CreateSpriteForDetailLevel (0x6F4D9830,
//  spritecreate.cpp): the two things it can build are
//  TAllocatedHandleObjectLeaf<CSpriteUber_,128> and
//  TAllocatedHandleObjectLeaf<CSpriteMini_,256>, registered under the Storm
//  handle-type tags "HSPRITEUBER" and "HSPRITEMINI".  So the placement
//  sub-object those earlier item targets could only describe by offset is a
//  sprite, and the "orientation matrix" and "position" they push into it are
//  the model instance's world transform.
//
//  Only the members and vtable slots this call tree reaches are named.  The
//  vtable is reached by raw cast rather than by inventing virtuals in front
//  of the slots, the same way widget.cpp and itemplacementdispatch.cpp
//  already do for this object.
//============================================================================
#ifndef SPRITE_H
#define SPRITE_H

//  The object CSprite::m_pModel points at.  Its own animation state hangs
//  off +0x98; only what modelinstance.cpp reaches is named.
class CModelAnim;
class CSprite;

class CTexture;

class CModelInstance
{
public:
    //  0x6F5053F0 / 0x6F502910 / 0x6F502950 - Render/material.h declares
    //  the three texture walks; they are members here so that one shipped
    //  class keeps one C++ name.
    void ReplaceTexture(int id, CTexture* texture);
    void ReplaceParticleTexture(int id, int alphaMode);
    void ReplaceRibbonTexture(int id, CTexture* texture);

    char         m_reserved00[0x2C];    // +0x00
    unsigned int m_materialCount;       // +0x2C
    class CMaterial** m_materials;      // +0x30
    //  +0x34, not +0x38: m_materials ends at 0x34, and the block that
    //  follows it had been declared 0x5C bytes from 0x38, which put every
    //  member after it four bytes early.  SetModelTrackValue reads the
    //  shipped `[ecx+98h]` and this build was emitting `[ecx+94h]` - one
    //  instruction in a fourteen-instruction function, and the only symptom.
    char         m_reserved34[0x60];    // +0x34
    unsigned int m_flags;               // +0x94
    CModelAnim*  m_pAnim;               // +0x98
    char         m_reserved9C[0x28];    // +0x9C
    unsigned int m_childListCount;      // +0xC4
    void*        m_childLists;          // +0xC8
    char         m_reservedCC[0x1C];    // +0xCC
    unsigned int m_ribbonCount;         // +0xE8
    void**       m_ribbons;             // +0xEC
    char         m_reservedF0[4];       // +0xF0
    unsigned int m_particleCount;       // +0xF4
    char*        m_particles;           // +0xF8
};

//  0x6F50D970 - hand this model instance, and its children, back for one
//  sprite.  A free `__fastcall` and not the thiscall member it used to be
//  declared as: the shipped body reads a *second register argument* out of
//  edx and passes it on, which a member declaration cannot name.  See
//  modelinstancerelease.cpp.  ReleaseSpriteModel's own call site is
//  unchanged either way - it pushes the sprite and leaves edx alone,
//  because edx already holds the value being forwarded.
void __fastcall ModelInstanceReleaseForSprite(CModelInstance* instance,
                                              void* notify, CSprite* sprite);

class CSprite
{
public:
    void*           m_vtable;           // +0x00
    char            m_reserved04[0x1C]; // +0x04
    CModelInstance* m_pModel;           // +0x20  null until a model is loaded
    char            m_reserved24[0x04]; // +0x24
    unsigned int    m_stateBits;        // +0x28  the flag word every setter below writes
    char            m_reserved2C[0x04]; // +0x2C
    void*           m_pModelData;       // +0x30  animation tables (see StopAnimationsOfType)
    //  +0x34..+0x36 - three flag bytes ClearModel (0x6F4D2C90,
    //  Widget/spriteclearmodel.cpp) zeroes one at a time, in that order.
    //  Nothing in these call trees reads any of them back.
    unsigned char   m_flag34;           // +0x34
    unsigned char   m_flag35;           // +0x35
    unsigned char   m_flag36;           // +0x36
    char            m_reserved37[0x29]; // +0x37
    //  +0x60 - a second counted reference ClearModel releases after the two
    //  slot calls; nothing else here says what it points at.
    void*           m_pRef60;           // +0x60
};

//  0x6F4D3200 - set or clear `bits` (only the low 16 are used) in +0x28.
void __fastcall SetSpriteStateBits(CSprite* self, unsigned short bits, int set);

//  0x6F4D3340 - set or clear the single bit 0x40000 in the same word.  A
//  separate entry point from the one above rather than a call to it, and it
//  takes its flag in edx with no stack argument at all.
void __fastcall SetSpritePausedBit(CSprite* self, int paused);

//  0x6F4D3190 - vtable slot 0x24.  The classic non-virtual-interface shape
//  itemplacementdispatch.cpp documents: a stable entry point forwarding to a
//  slot that is not resolvable at compile time.
void __fastcall SetSpriteAnimationScale(CSprite* self, float scale);

//  0x6F4D3070 - convert seconds to the model timeline's own integer units
//  and push them at the model instance, if there is one.
void __fastcall SetSpriteTimeScale(CSprite* self, float seconds);

//  The colour slot, as a one-dword class rather than a bare int.  Every
//  shipped call site reserves the argument with a `push` and then writes
//  the value *through a pointer* into the slot it just made
//  (`push ecx` / `mov eax, esp` / `mov [eax], ecx`), which is what MSVC
//  emits for a class passed by value and never for an int - an int is one
//  `push`.  The type carries no behaviour; it exists to get the copy.
class CTeamColorSlot
{
public:
    CTeamColorSlot(int value) : m_value(value) {}
    CTeamColorSlot(const CTeamColorSlot& other) : m_value(other.m_value) {}

    int m_value;
};

//  0x6F4D3220 - vtable slot 0x30 with the colour by address, then optionally
//  vtable slot 0x14 to make it take effect.
void __fastcall SetSpriteTeamColor(CSprite* self, int refresh,
                                   CTeamColorSlot color);

//  0x6F4D3270 - hand the model instance back to whatever owns it.
//
//  The second parameter is never read by this function's own body, and is
//  declared only because its one caller (CSprite::LoadModelWithTokens,
//  0x6F314CE0) loads edx immediately before the call.  It reaches
//  CModelInstance::ReleaseForSprite anyway: nothing between this function's
//  entry and that call touches edx, so the register is forwarded intact.
//  Naming it would be a lie about this body; leaving it out costs the
//  caller its `mov edx` and the callee nothing.
void __fastcall ReleaseSpriteModel(CSprite* self, void* /*notify*/);

//  0x6F4D2C90 - Widget/spriteclearmodel.cpp: drop everything the sprite
//  holds on behalf of a loaded model.
void __fastcall ClearSpriteModel(CSprite* self);

//  0x6F4D3160 - a bare tail jump into vtable slot 0x4C, whose answer this
//  family only ever uses as a byte.
int __fastcall QuerySpriteSlot4C(CSprite* self);

//  0x6F4D3250 - vtable slot 0x34 with `value`, then slot 0x14 to make it
//  take effect, only when `refresh` is set.  Same shape as
//  SetSpriteTeamColor above, one slot along.
void __fastcall SetSpriteSlot34(CSprite* self, int value, int refresh);

//  0x6F4D4BB0 - ask vtable slot 0x50 for a record and copy its first dword
//  into `out`.
void __fastcall QuerySpriteSlot50Word(CSprite* self, int* out);

//  0x6F4D4B20 - walk the model's animation tables and mark every track whose
//  own id list contains `type` as dirty.
void __fastcall StopSpriteAnimationsOfType(CSprite* self, int type);

//  0x6F50C4A0 - one IDA function in two chunks: the null guard sits at
//  0x6F50C4A0 and the body it jumps to at 0x6F5149B0.
int __fastcall MarkModelTrackDirty(CModelInstance* self, int track);

//  0x6F501D40 - the same two-chunk shape (0x6F501D40 / 0x6F514950).
int __fastcall RescaleModelTimeline(CModelInstance* self, int newLength);

#endif
