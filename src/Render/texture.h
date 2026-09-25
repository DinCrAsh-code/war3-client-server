//============================================================================
//  `.\Texture.cpp` - the texture cache, its two hash tables and the three
//  file-format classes that hang off it.
//
//  Every name below is read out of the binary rather than invented:
//
//      ??_7CTexture@@6B@        0x6F954F78    the base
//      ??_7CTexTexture@@6B@     0x6F954F90    the engine's own .tex
//      ??_7CBlpTexture@@6B@     0x6F954FA8    .blp
//      ??_7CTgaTexture@@6B@     0x6F954FC0    .tga
//
//      ??_7?$TSHashTable@UCTextureHash@@VHASHKEY_TEXTUREFILE@@@@6B@     0x6F95505C
//      ??_7?$TSHashTable@UCSolidTextureHash@@VHASHKEY_NONE@@@@6B@      0x6F955070
//
//  and the two bucket arrays' Storm allocation tags are the element types'
//  own RTTI descriptor names,
//  `.?AV?$TSExplicitList@UCTextureHash@@$0?CCCCCCCD@@@` and its
//  CSolidTextureHash twin, which is where the two node type names come from.
//  The objects really in the two table globals are
//  `TSHashTableReuse<CTextureHash,HASHKEY_TEXTUREFILE,1>` and
//  `TSHashTableReuse<CSolidTextureHash,HASHKEY_NONE,1>` - a derived class
//  whose own members are outside this closure - so the globals below are
//  declared at the base, which is at offset 0 and is all this closure
//  touches.  ("HTEXTURE" is the Storm allocation tag every texture object
//  and every key copy carries, and `??_R0PAUHTEXTURE__@@@8` says it is the
//  handle typedef's own struct tag.)
//
//  What the module does: a texture is asked for by file name plus a dword of
//  load options.  `TextureCacheFind` hashes the name, looks the pair up and
//  hands back a reference; `TextureCacheAdd` puts one in.  A miss classifies
//  the extension (`.tga`, `.blp`, anything else) and builds the matching
//  CTexture subclass.  The second table is the same idea for the one-pixel
//  solid-colour textures, keyed on the colour alone - which is why its key
//  policy is HASHKEY_NONE and its lookup never compares a key.
//============================================================================
#ifndef TEXTURE_H
#define TEXTURE_H

#include "allochashtable.h"
#include "renderdevice.h"

//: The module tag every SMemAlloc/SMemFree in `.\Texture.cpp` passes.
extern const char kTextureCpp[];
//: The Storm allocation tag every texture object and key copy carries.
extern const char kHTexture[];

//----------------------------------------------------------------------------
//  The cache key: a heap copy of the file name and the load options.
//----------------------------------------------------------------------------
class HASHKEY_TEXTUREFILE
{
public:
    //  Inline: the shipped code has the duplication written out in each of
    //  the two functions that build a key (0x6F4E1410 and 0x6F4E3AE0), each
    //  with its own __LINE__.
    HASHKEY_TEXTUREFILE(const char* path, TEXTUREOPTIONS options)
    {
        m_path = Storm_507(path, kTextureCpp, 0x45);
        m_options = options;
    }

    ~HASHKEY_TEXTUREFILE()
    {
        if (m_path != 0)
            SMemFree(m_path, kTextureCpp, 0x4B, 0);
    }

    //  0x6F4DEA30 - Render/texturekeycompare.cpp.
    int operator==(const HASHKEY_TEXTUREFILE& other) const;

    //  0x6F4E3AE0 has this written out: free the old copy, take a new one.
    HASHKEY_TEXTUREFILE& operator=(const HASHKEY_TEXTUREFILE& other)
    {
        if (m_path != 0)
            SMemFree(m_path, kTextureCpp, 0x50, 0);
        m_path = Storm_507(other.m_path, kTextureCpp, 0x51);
        m_options = other.m_options;
        return *this;
    }

    char*           m_path;         // +0x00
    TEXTUREOPTIONS  m_options;      // +0x04

private:
    HASHKEY_TEXTUREFILE(const HASHKEY_TEXTUREFILE&);
};

//  HASHKEY_NONE itself moved to Storm/storm.h (Config/preferences.h needs
//  the same policy for CPreferencesWar3, and two definitions of the same
//  name from two headers included together do not compile).

//----------------------------------------------------------------------------
//  CTexture - the base of the three file-format classes.  Twenty bytes: the
//  vtable, TRefCnt's count, the format, the engine-side image the device
//  fills in and the flag word.  CTexTexture (0x6F4E3C80) allocates exactly
//  0x14 and is the proof of the length.
//
//  Nothing in this closure calls a virtual on one except slot 0 (the scalar
//  deleting destructor), so no `virtual` is declared here and every
//  constructor stamps the vtable address as data, the way
//  Frame/controlframe.cpp does.
//----------------------------------------------------------------------------
class CTexture
{
public:
    void*         m_vtable;         // +0x00  ??_7CTexture@@6B@ and friends
    int           m_refcount;       // +0x04  TRefCnt's
    unsigned int  m_format;         // +0x08
    void*         m_image;          // +0x0C  what the device hands back
    unsigned int  m_flags;          // +0x10  bit 0: no alpha channel
};

//----------------------------------------------------------------------------
//  The two file-backed textures.  They carry the identical five extra
//  fields, and the binary has no RTTI descriptor for any class between them
//  and CTexture, so the fields are spelled out in each rather than given an
//  invented common base.  The path is stored *inside* the object: the
//  allocation is `SStrLen(path) + 0x25`, which is 0x24 of members plus the
//  name and its NUL, and m_path is set to point at m_pathText.
//----------------------------------------------------------------------------
class CStatus;
class CTgaFile;
class CBlpFile;

class CTgaTexture : public CTexture
{
public:
    //  0x6F4DF230 - Render/tgatexturecreate.cpp.
    static CTgaTexture* __fastcall Create(const char* path, CStatus* status,
                                          TEXTUREOPTIONS options);

    void*         m_decoder;        // +0x14  the CTgaFile, while loading
    unsigned int  m_reserved18;     // +0x18
    unsigned int  m_reserved1C;     // +0x1C
    char*         m_path;           // +0x20  points at m_pathText
    char          m_pathText[1];    // +0x24
};

class CBlpTexture : public CTexture
{
public:
    //  0x6F4DF470 - Render/blptexturecreate.cpp.
    static CBlpTexture* __fastcall Create(const char* path, CStatus* status,
                                          TEXTUREOPTIONS options, int arg8);

    void*         m_decoder;        // +0x14  the CBlpFile, while loading
    unsigned int  m_reserved18;     // +0x18
    unsigned int  m_reserved1C;     // +0x1C
    char*         m_path;           // +0x20
    char          m_pathText[1];    // +0x24
};

//  0x6F4E3C80 - the one-pixel solid-colour texture.  0x14 bytes, so it adds
//  nothing to CTexture.
class CTexTexture : public CTexture
{
};

extern void* const g_vftCTexture;       // ??_7CTexture@@6B@,    0x6F954F78
extern void* const g_vftCTexTexture;    // ??_7CTexTexture@@6B@, 0x6F954F90
extern void* const g_vftCBlpTexture;    // ??_7CBlpTexture@@6B@, 0x6F954FA8
extern void* const g_vftCTgaTexture;    // ??_7CTgaTexture@@6B@, 0x6F954FC0

//----------------------------------------------------------------------------
//  The two node types.  Both are TSHashObject's layout - hash value, bucket
//  link, table-wide link, then the key at +0x14 - with the texture behind
//  it.
//----------------------------------------------------------------------------
struct CTextureHash
{
    unsigned int          m_hashval;        // +0x00
    TSLink<CTextureHash>  m_hashlink;       // +0x04
    char                  m_reserved0C[8];  // +0x0C  the table-wide link
    HASHKEY_TEXTUREFILE   m_key;            // +0x14
    CTexture*             m_texture;        // +0x1C
};

struct CSolidTextureHash
{
    unsigned int               m_hashval;       // +0x00
    TSLink<CSolidTextureHash>  m_hashlink;      // +0x04
    char                       m_reserved0C[8]; // +0x0C
    HASHKEY_NONE               m_key;           // +0x14
    CTexture*                  m_texture;       // +0x18
};

typedef TSHashTable<CTextureHash, HASHKEY_TEXTUREFILE>  CTextureTable;
typedef TSHashTable<CSolidTextureHash, HASHKEY_NONE>    CSolidTextureTable;

//: The tables are the objects, not pointers to them - every reference is
//: `mov ecx, offset <sym>`.
extern CTextureTable       g_textureTable;      // dword_6FAB8028
extern CSolidTextureTable  g_solidTextureTable; // unk_6FAB8070
//: The one HASHKEY_NONE the solid table's lookup is handed.  It has no
//: members, so nothing is ever read out of it; 0x6F4E14B2 is its only
//: reference in the whole image.
extern HASHKEY_NONE        g_noTextureKey;      // unk_6FAB8020

//----------------------------------------------------------------------------
//  The module's own functions.
//----------------------------------------------------------------------------
//  0x6F4DDE20 - report a load failure through the CStatus the caller handed
//  down.  **The shipped build takes all three arguments in registers** - the
//  status in esi, the file name in ebx and the kind word in edi - and no
//  MSVC convention reaches any of them, so this takes the status in ecx and
//  the other two on the stack and each caller pays the moves.
void __fastcall ReportTextureLoadError(CStatus* status, const char* name,
                                       const char* kind);

//  0x6F4DDE80 - which decoder does this file name want?  0 is `.tga`, 1 is
//  `.blp` and 3 is "no extension, or one nothing here reads"; the shipped
//  code forms 1 and 3 out of the compare result with `neg`/`sbb`/`and 2`/
//  `add 1` rather than branching, which is what says they are 1 and 3 and
//  not 1 and 2.
int __fastcall ClassifyTextureExtension(const char* path);

//  0x6F4DDFB0 - halve both dimensions until they fit the device's maximum,
//  counting the halvings into `mipSkip`.  The height arrives in ebx in the
//  shipped build; it is ecx here and the one caller pays a move.  `width` is
//  a one-pointer struct so that it and `mipSkip` both land on the stack:
//  the shipped function cleans eight bytes, and a plain pointer would take
//  edx and leave it cleaning four.
//  *Both* trailing parameters have to be structs, not just the first:
//  `__fastcall` skips a non-eligible argument but still hands edx to the
//  next eligible one, so a plain `int*` third parameter puts itself in edx
//  and the function goes back to cleaning four.
struct TEXTUREDIMREF
{
    unsigned int* m_value;
};

struct TEXTUREMIPREF
{
    int* m_value;
};

void __fastcall ShrinkTextureToDeviceLimit(unsigned int* height,
                                           TEXTUREDIMREF width,
                                           TEXTUREMIPREF mipSkip);

//  0x6F4E1410 - the cache lookup: hash the name, build a key, and answer a
//  *new reference* on the texture behind it.
CTexture* __fastcall TextureCacheFind(const char* path,
                                      TEXTUREOPTIONS options);

//  0x6F4E3AE0 - put one in.
void __fastcall TextureCacheAdd(const char* path, TEXTUREOPTIONS options,
                                CTexture* texture);

//  0x6F4E14B0 - the same lookup for the solid-colour table, keyed on the
//  colour alone.  The shipped build takes the colour's *address* in eax.
CTexture* __fastcall SolidTextureCacheFind(const unsigned int* color);

//  0x6F4E3BB0 - and the same insert; the colour's address is in eax again,
//  and the texture is a one-pointer struct for the same reason
//  ShrinkTextureToDeviceLimit's `width` is: the shipped function cleans four
//  bytes and a plain pointer would take edx and leave it cleaning none.
struct CTEXTUREREF
{
    CTexture* m_texture;
};

void __fastcall SolidTextureCacheAdd(const unsigned int* color,
                                     CTEXTUREREF texture);

//  0x6F4E3BE0 - the module's public entry: hand back a texture for a file
//  name, out of the cache when `reload` is clear and by decoding it when it
//  is not.
CTexture* __fastcall LoadTextureFile(const char* path, CStatus* status,
                                     TEXTUREOPTIONS options, int reload,
                                     int arg8);

//  0x6F4E3C80 - a one-pixel texture of one colour, cached on the colour.
CTexture* __stdcall MakeSolidTexture(unsigned int color);

//  0x6F732360 - the same load driven by a bare path: strip whatever
//  extension it has and try `.tga` then `.blp`.
CTexture* __fastcall LoadTextureByPath(const char* path,
                                       const TEXTUREOPTIONS* override,
                                       int reload, int arg8);

//: dword_6FADAB90 - the microsecond counter LoadTextureByPath credits.
extern unsigned int g_textureLoadMicros;

#endif
