//============================================================================
//  The generic TSHashTable<T,K>/TSGrowableArray<T>/TSExplicitList<T> members
//  both of CPreferencesWar3's tables reach: CPreferencesWar3 is itself
//  TSHashTable<PrefString,HASHKEY_NONE> and its +0x28 cache is
//  TSHashTable<SPreferenceEntry,int> (Config/preferences.h says why each
//  layout is confirmed).  Every function here is Containers/tshash.inl's or
//  Containers/tsarray.inl's own body, reused verbatim - none of it is
//  preference-specific, which is why the two tables' Rehash/Initialize/
//  CheckGrow/UnlinkAll/SetCount pairs are byte-identical apart from which
//  UnlinkAll/SetCount/ComputeChunk/SetAlloc instantiation each calls.
//
//  Traced from CPreferencesWar3::Rehash's own callee (UnlinkAll at
//  0x6F003990) and SPreferenceCache::Initialize's (UnlinkAll at 0x6F003A00,
//  reached through SetLinkOffset) to tell the two tables' instantiations
//  apart - the bodies are otherwise indistinguishable.
//============================================================================
#include "preferences.h"
#include "tshash.inl"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<PrefString>        PrefStringBucket;
typedef TSExplicitList<SPreferenceEntry>  PrefValueBucket;

//----------------------------------------------------------------------------
//  CPreferencesWar3's own table (the name -> id side).
//----------------------------------------------------------------------------
//  0x6F004E10 - CPreferencesWar3::Rehash.
template void TSHashTable<PrefString, HASHKEY_NONE>::Rehash(unsigned int);
//  0x6F0045D0 - the bucket array's own SetCount.
template void TSGrowableArray<PrefStringBucket>::SetCount(unsigned int);
//  0x6F003200 - ...and its ComputeChunk.
template unsigned int TSGrowableArray<PrefStringBucket>::ComputeChunk(unsigned int);
//  0x6F0040B0 - ...and its SetAlloc.
template void TSGrowableArray<PrefStringBucket>::SetAlloc(unsigned int);
//  0x6F003990 - one bucket's own UnlinkAll (SetLinkOffset's, and Rehash's
//  parked list's).
template void TSExplicitList<PrefString>::UnlinkAll();
//  0x6F0051B0 - CPreferencesWar3's own Initialize: first use of the table,
//  four buckets, m_mask=3.
template void TSHashTable<PrefString, HASHKEY_NONE>::Initialize();
//  0x6F005210 - CPreferencesWar3's own CheckGrow.
template int TSHashTable<PrefString, HASHKEY_NONE>::CheckGrow(unsigned int);
//  0x6F0043F0 - the bucket array's own destructor.  Every live bucket is a
//  TSExplicitList<PrefString>, whose own destructor (UnlinkAll() plus the
//  terminator's own inlined Unlink()) is non-trivial, so ~TSGrowableArray's
//  generic loop in tsarray.inl actually runs it per element here rather than
//  optimising down to the single Storm call the trivial-T instantiations
//  get - this is the ~CPreferencesWar3 half that calls it (0x6F0049D0,
//  still a thunk: see Misc/spritehandlethunks.cpp for why the containing
//  ~TSHashTable itself is not reconstructible, even though this one member
//  of it is).
template TSGrowableArray<PrefStringBucket>::~TSGrowableArray();

//----------------------------------------------------------------------------
//  The id -> value cache (SPreferenceCache, +0x28).
//----------------------------------------------------------------------------
//  0x6F0055A0 - SPreferenceCache::New calls this directly (preferencestore.cpp
//  passes the preference id as the hash value and the caller re-stores it
//  into the node's own +0x00 right after, which is why New() itself never
//  needs the id a second time).
template SPreferenceEntry* TSHashTable<SPreferenceEntry, int>::NewNode(
    unsigned int, void*, void*);
//  0x6F004FE0 - SPreferenceCache::Rehash.
template void TSHashTable<SPreferenceEntry, int>::Rehash(unsigned int);
//  0x6F0052B0 - SPreferenceCache::Initialize (lazily called out of NewNode
//  the first time the cache is used - preferencesctor.cpp pre-sizes it
//  through Rehash instead, so this is reached only via CheckGrow).
template void TSHashTable<SPreferenceEntry, int>::Initialize();
//  0x6F005310 - SPreferenceCache::CheckGrow.
template int TSHashTable<SPreferenceEntry, int>::CheckGrow(unsigned int);
//  0x6F0046C0 - the bucket array's own SetCount.
template void TSGrowableArray<PrefValueBucket>::SetCount(unsigned int);
//  0x6F003260 - ...and its ComputeChunk.
template unsigned int TSGrowableArray<PrefValueBucket>::ComputeChunk(unsigned int);
//  0x6F0041F0 - ...and its SetAlloc.
template void TSGrowableArray<PrefValueBucket>::SetAlloc(unsigned int);
//  0x6F003A00 - one bucket's own UnlinkAll.
template void TSExplicitList<SPreferenceEntry>::UnlinkAll();
//  0x6F004470 - the bucket array's own destructor, same reason as
//  0x6F0043F0 just above: called out of SPreferenceCache::~SPreferenceCache
//  (0x6F004CD0, still a thunk for the same "declared-never-defined
//  AllocNode" reason the two ConstructPreference* redirects are).
template TSGrowableArray<PrefValueBucket>::~TSGrowableArray();
