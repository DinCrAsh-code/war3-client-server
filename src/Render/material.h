//============================================================================
//  HMATERIAL - the material a model's geosets are drawn with, its texture
//  layers, and the model-side walks that swap a texture into every layer
//  that names a given id.
//
//  The names are the shipped ones where the binary carries them:
//  `??_7CMaterial@@6B@` (0x6F95539C) is stamped by the clone at 0x6F505130,
//  "HMATERIAL" is its Storm allocation tag, `??_7CMaterialShared@@6B@`
//  (0x6F95538C) is the object hanging off +0x38, and the layer array's own
//  Storm tag is the element type's RTTI descriptor name, `.?AUCTexLayer@@`.
//  The model side is `CModelInstance`, which Widget/sprite.h already
//  declares as what CSprite holds at +0x20; this batch adds the fields the
//  three texture walks read - a material list at +0x2C/+0x30, a child list
//  at +0xC4/+0xC8, a ribbon list at +0xE8/+0xEC, a particle array at
//  +0xF4/+0xF8 and the flag word at +0x94 whose bit 4 picks between the deep
//  walk and the shallow one - rather than inventing a second class for one
//  shipped type.
//============================================================================
#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "dataallocator.h"
#include "sprite.h"

//: "HMATERIAL", the Storm allocation tag every CMaterial carries.
extern const char kHMaterial[];

//  0x6F4DE1E0 - Misc/trivialaccessors_04.cpp.  A texture's own +0x0C.
//  Spelled exactly as that file defines it, `int` return and `const`
//  included: MSVC puts both in the mangled name.
int __fastcall GetField0xC_6F4DE1E0(const void* self);
//  0x6F502880 - Misc/trivialpredicates.cpp.  Is this the only reference?
int   __fastcall IsField0x4One_6F502880(const void* self);
//  0x6F514820 - Misc/trivialpredicates.cpp, spelled as that file defines it.
int __fastcall QueryByte0x58_6F514820(const void* self, int* out);

//----------------------------------------------------------------------------
//  One texture layer: seven plain dwords and two reference-counted textures.
//  0x24 bytes, which is the stride every walk over the array uses.
//----------------------------------------------------------------------------
class CTexLayer
{
public:
    //  0x6F4F6B80 - Render/texlayercopy.cpp.
    CTexLayer(const CTexLayer& other);

    unsigned int m_reserved00[4];   // +0x00
    int          m_alphaMode;       // +0x10  set from the texture's +0x0C
    unsigned int m_reserved14[2];   // +0x14
    CTexture*    m_textures[2];     // +0x1C
private:
    CTexLayer& operator=(const CTexLayer&);
};

//----------------------------------------------------------------------------
//  The layer array, with room for one layer inside itself.  m_data points at
//  m_inline while m_alloc is 0 or 1 and at the heap above that, which is why
//  SetAlloc has two arms that both end in the same two stores.
//----------------------------------------------------------------------------
class CTexLayerArray
{
public:
    //  0x6F502750 - Render/texlayeralloc.cpp.
    void SetAlloc(unsigned int alloc);
    //  0x6F503700 - Render/texlayerassign.cpp.
    void Assign(unsigned int count, const CTexLayer* src);

    unsigned int m_alloc;           // +0x00
    unsigned int m_count;           // +0x04
    CTexLayer*   m_data;            // +0x08
    char         m_inline[0x24];    // +0x0C  one layer's worth
};

//----------------------------------------------------------------------------
//  The shared, immutable half of a material: the texture *ids* each layer
//  names, two per entry, in an array of 0x2C-byte entries.
//----------------------------------------------------------------------------
struct MATERIALTEXTUREIDS
{
    unsigned int m_reserved00;      // +0x00
    int          m_ids[2];          // +0x04
    unsigned int m_reserved0C[8];   // +0x0C
};

class CMaterialShared
{
public:
    char                m_reserved00[0x0C]; // +0x00
    unsigned int        m_count;            // +0x0C
    MATERIALTEXTUREIDS* m_entries;          // +0x10
};

//----------------------------------------------------------------------------
//  CMaterial itself.  0x3C bytes: TRefCnt's two, the layer array and the
//  shared half.
//----------------------------------------------------------------------------
class CMaterial
{
public:
    void*            m_vtable;      // +0x00  ??_7CMaterial@@6B@
    int              m_refcount;    // +0x04
    CTexLayerArray   m_layers;      // +0x08
    CMaterialShared* m_shared;      // +0x38
};

extern void* const g_vftCMaterial;      // ??_7CMaterial@@6B@, 0x6F95539C

//: unk_6FAB8268 - TAllocatedHandleObject<CMaterial,512>, the pool every
//: CMaterial comes out of.
extern CDataAllocator g_materialPool;

//  0x6F505130 - a private copy of a material, out of the pool.
CMaterial* __stdcall CloneMaterial(const CMaterial* src);

//  0x6F502840 - does any layer of this material name that texture id?  The
//  shipped build takes the material in eax and the id in edi.
int __fastcall MaterialUsesTextureId(const CMaterial* material, int id);

//----------------------------------------------------------------------------
//  A slot holding one CMaterial pointer.  It is a class rather than a free
//  function taking `CMaterial**` because of the ABI: 0x6F505330 takes the
//  slot's address in a register and cleans eight bytes, which is
//  `__thiscall` with two stack arguments and nothing else.
//----------------------------------------------------------------------------
class CMaterialRef
{
public:
    //  0x6F505330 - swap `texture` into every layer whose shared entry names
    //  `id`, copying the material first if it is shared.
    void ReplaceTexture(int id, CTexture* texture);

    CMaterial* m_material;          // +0x00
};

//  0x6F5053F0 - the shallow walk: every material this model owns.  Declared
//  as a CModelInstance member (Widget/sprite.h) rather than as a free
//  function taking the id in ecx: the shipped body takes the id in eax and
//  cleans *eight*, and the only spelling that cleans eight with a receiver
//  in a register is `__thiscall`.  What that costs is the id moving from
//  eax to a stack slot at both call sites; what it buys is the byte count,
//  which is an abi_audit FATAL when it is wrong.

//  0x6F506A70 - the deep walk: this model, its emitters and every child.
void __fastcall ReplaceTextureDeep(CModelInstance* model, int id,
                                   CTexture* texture, int recurse);
//  0x6F506B20 - pick between the two on the model's own flag bit 4.
void __fastcall ReplaceModelTexture(CModelInstance* model, int id,
                                    CTexture* texture, int recurse);

//----------------------------------------------------------------------------
//  One ribbon emitter, as far as this closure reaches it.
//----------------------------------------------------------------------------
class CRibbonEmitter
{
public:
    //  0x6F4F1F90 - Render/ribbonsettexture.cpp.
    void SetTexture(CTexture* texture);
    //  0x6F4F2140 - the same file.  IDA calls it
    //  `?GetNumberOfBoundContexts@SchedulerBase@details@Concurrency@@IBEKXZ_5`
    //  because its two instructions fold onto a ConcRT method of the same
    //  shape; what it reads is a texture id and what compares it is
    //  CModelInstance::ReplaceRibbonTexture.
    unsigned int GetTextureId() const;

    char         m_reserved000[0x178]; // +0x000
    int          m_alphaMode;          // +0x178
    char         m_reserved17C[0x10];  // +0x17C
    CTexture*    m_texture;            // +0x18C
    unsigned int m_textureId;          // +0x190
};

#endif
