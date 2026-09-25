//============================================================================
//  0x6F49CD10 - NIpse::CLrPath::~CLrPath().  CPathClient (pathmove.h) is
//  this repo's own pre-existing, provisionally-named reconstruction of
//  NIpse::CLrPath - see pathmove.h's own file header for the RTTI
//  cross-check this session made.  Declared here rather than in pathmove.h
//  itself, matching this file's own house convention of one TU per real
//  shipped module (CLAUDE.md).
//
//  Same `__CxxFrameHandler3` SEH-frame-not-reproducible shape as
//  Pathfinding/crlagent_dtor.cpp and Pathfinding/crlagent_ctor.cpp (and
//  docs/msvc-vc8-idioms.md's own writeup) - a call ahead of it is not
//  provably non-throwing, so the compiler wraps the whole body in a
//  cookie-XORed frame this build's fixed `/GS- /EHs-c-` cannot reproduce.
//  The body below matches the dump instruction for instruction past the
//  frame furniture.
//
//  Destroys the two-element `m_cache[2]` array (SPathPointCache, each a
//  CMemBlock-derived 0x20-byte object at +0x34/+0x54) through the standard
//  `eh vector destructor iterator` (Misc/crtseh4.cpp), then restamps
//  straight to NTempest::CPresence's own vtable - CPresenceTagged's and
//  CPrRelation's own destructor bodies are trivial enough to have folded
//  into this same function with no stamp of their own, the same shape
//  crlagent_dtor.cpp's own file header documents for that class.
//============================================================================
#include "pathmove.h"
#include "crtseh.h"

//  0x6F473A70 - a second jmp-alias into ReleaseCMemBlockBase's own real
//  target (0x6F4B3770, Pathfinding/cellbuffer.cpp), at its own distinct
//  address - this image was linked without /OPT:ICF, so an identical
//  one-instruction jmp thunk exists at more than one address (the same
//  shape cellbuffer.cpp's own ReleaseCMemBlockBaseAlias already documents
//  for 0x6F743320).  Needed as its own real symbol (rather than reusing
//  ReleaseCMemBlockBase directly) purely so `offset sub_6F473A70` in the
//  dump has something to canonicalise against at the address the dump
//  itself pushes.
//  A bare one-instruction `jmp` alias, matching this exact shape (E9 rel32,
//  no register load) - Pathfinding/cellbuffer.cpp's own 0x6F743320
//  (ReleaseCMemBlockBaseAlias) is the same one-instruction-jmp shape at a
//  third address for the same reason.
void __fastcall ReleaseCMemBlockBase(void*);   // Pathfinding/cellbuffer.cpp
__declspec(naked) void __fastcall ReleaseCMemBlockBaseAliasLrPath(void*)
{
    __asm { jmp ReleaseCMemBlockBase }
}

static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

CPathClient::~CPathClient()
{
    EhVectorDestructorIterator(&m_cache[0], 0x20, 2,
        (void(__thiscall*)(void*))ReleaseCMemBlockBaseAliasLrPath);

    StampVtable(this, (void*)0x6F951CA0);     // ??_7CPresence@NTempest@@6B@
}
