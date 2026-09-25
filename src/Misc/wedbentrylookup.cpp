//============================================================================
//  0x6F706B40 - CItem vtable slot 32/104 closure (depth 7).
//  TSHashTable<WEDBEntryHash, HASHKEY_STRI>::Ptr(const char* key) - the
//  string-keyed lookup Containers/tshashstr.inl already defines, for a new
//  node type.
//
//  The instantiation is read straight off the vtable this table's own
//  constructors stamp (Misc/wedatabase_thunks.cpp, 0x6F70D070/0x6F70DAD0):
//  `??_7?$TSHashTable@UWEDBEntryHash@@VHASHKEY_STRI@@@@6B@`, i.e.
//  `struct WEDBEntryHash` keyed with Storm's case-insensitive string policy.
//  Nothing in this call tree reads a WEDBEntryHash field beyond the pair
//  Ptr() itself touches (m_hashval, m_key), so the node is defined as the
//  bare TSHashObject<T,K> layout with the key field typed `const char*` -
//  the same reasoning GameUI/gameui.h's CCachedFont comment gives for
//  leaving an element type otherwise empty, except this walk *does* read a
//  field, so the field has to be real rather than absent.
//============================================================================
#include "allochashtable.h"
#include "tshashstr.inl"

struct WEDBEntryHash : public TSHashObject<WEDBEntryHash, const char*> { };

template WEDBEntryHash* TSHashTable<WEDBEntryHash, HASHKEY_STRI>::Ptr(const char*);
