//============================================================================
//  0x6F47A540 - SBoolQueryOwner::ResolveActiveEntry.  A naked redirect in
//  missile_dealdamage.cpp until now (it had no name there at all - only
//  the note "its own body chains sub_6F47A540, deferred").
//
//  Resolve the handle pair, and if the resolved object's own count at
//  +0xB8 is non-zero, hand back the first element of the list its +0xA8
//  points at.  No null check on the resolve: the shipped code dereferences
//  what LookupHandle returned immediately.
//
//  `<= 0` on an *unsigned* count and not `== 0`.  The two are the same
//  test, but only the ordered spelling makes MSVC emit `cmp`/`ja` with the
//  zero return in the middle of the body, which is where the shipped code
//  has it; `== 0` gives `jne` and sinks the `xor eax, eax` past the tail.
//============================================================================
#include "boolqueryowner.h"

//  handle.cpp's, under its real name *and its real return type*: MSVC puts
//  the return type of a free function in the mangled name, so declaring it
//  `void*` here emitted `?LookupHandle@@YIPAXIH@Z` against the real
//  `?LookupHandle@@YIPAVCHandleObject@@IH@Z` - two symbols for one
//  function, an LNK2019 in the operator's build, and enough on its own to
//  make verify.py report the *real* LookupHandle as not compiled at all.
class CHandleObject;
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//  Only the two words this body reads.
struct SResolvedEntryList
{
    char   m_reserved00[0xA8];
    void** m_pEntries;      // +0xA8
    char   m_reservedAC[0x0C];
    unsigned int m_count;   // +0xB8
};

void* __thiscall SBoolQueryOwner::ResolveActiveEntry()
{
    SResolvedEntryList* list =
        (SResolvedEntryList*)LookupHandle(m_handle, m_typeTag);
    if (list->m_count <= 0)
        return 0;
    return *list->m_pEntries;
}
