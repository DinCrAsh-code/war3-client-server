//============================================================================
//  0x6F403A30 - CTechTree::Load, CTechTree::Save's own counterpart
//  (techtreesave.cpp) - reached from CPlayerWar3::Load's own +0x2D0 field
//  the same way Save reaches ::Save there.  `retn 4`.
//
//  Its own first two calls (0x6F4028B0/0x6F402970) are redirects, not
//  reconstructions, for the same reason playerwar3_ctorsubobjects.cpp's own
//  header comment gives for CTechTree's constructor and CSelectionWar3's
//  own +0x34: they populate this class's two
//  TSHashTable<T,HASHKEY_4CC> instantiations (a tech-tree registration
//  table at +0x4, an ability-registration table at +0x2C - CTechTree's own
//  ctor comment already names both), a class this repo has never modelled
//  concretely.  Both are read off the stream as a word count and that many
//  hashtable inserts (Storm_401 alloc, an owned-string-copy helper at
//  0x6F4C8520, then one of two distinct TSHashTable::Insert-shaped bodies
//  per table) - real work, but into fields with no committed layout.
//  Redirected rather than guessed; each is its own `retn 0` (a stack-arg
//  count of zero, matching the `__fastcall(store, field)` shape every
//  other reader in this dump uses).
//
//  Own translation unit: four real calls (the two thunks plus the trailing
//  four ReadDwordAlt reads into +0x54/+0x58/+0x5C/+0x60, not otherwise
//  named - CTechTree stays as opaque past that point as CSelectionWar3
//  does).
//============================================================================
#include "playerwar3.h"
#include "cdatastore.h"

__declspec(naked) CDataStore* __fastcall CTechTreeReadTechRegistrations(
        CDataStore* store, void* field)
{
    __asm
    {
        mov     eax, 06F4028B0h
        jmp     eax
    }
}

__declspec(naked) CDataStore* __fastcall CTechTreeReadAbilRegistrations(
        CDataStore* store, void* field)
{
    __asm
    {
        mov     eax, 06F402970h
        jmp     eax
    }
}

void CTechTree::Load(CDataStore* store)
{
    CTechTreeReadTechRegistrations(store, (char*)this + 4);
    CTechTreeReadAbilRegistrations(store, (char*)this + 0x2C);

    store->ReadDwordAlt((unsigned int*)((char*)this + 0x54));
    store->ReadDwordAlt((unsigned int*)((char*)this + 0x58));
    store->ReadDwordAlt((unsigned int*)((char*)this + 0x5C));
    store->ReadDwordAlt((unsigned int*)((char*)this + 0x60));
}
