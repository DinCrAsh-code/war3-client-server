//============================================================================
//  CGameDataContext::CGameDataContext (0x6F007D90) - the real constructor
//  for the object at thread-local slot 0x0D (gamecontext.h).  Found inside
//  0x6F008420, the engine bring-up function
//  Game/gamemaininitsequence.cpp's own `RegisterNotifyTimer(1,
//  (void*)0x6F008420, 1, (void*)0x6F007A50, 3)` already names as a raw
//  address pair - 0x6F008420 (far too large to reconstruct as a unit: it
//  is the whole engine startup sequence, of which allocating and
//  constructing this one object is a handful of instructions) is the
//  startup handler, 0x6F007A50 its paired shutdown handler.  0x6F007A50
//  is itself a ~45-instruction shutdown orchestrator for a good dozen
//  unrelated subsystems, not a dedicated teardown for this object alone -
//  its own middle third (`mov ecx,0Dh; call GetThreadLocalSlot; ...`) frees
//  the provider array and the context exactly as the second half of this
//  file describes, but reconstructing the surrounding orchestrator was
//  judged out of this session's scope, so it stays undecoded outside this
//  comment and carries no funcmap entry of its own.
//
//  This settles two things checksum-provider-registry.md left open:
//
//  1. What `context->m_pCache` (gamecontext.h's `CGameDataCache*`) really
//     is.  The Storm allocation tag on both the resize helper below and on
//     this object's own teardown free is `.PAVCSynchronousData@@` - a
//     pointer to CSynchronousData (synchronousdata.h) - so the field is
//     really a flat `CSynchronousData**`, one pointer per provider slot,
//     not a pointer to some dedicated `CGameDataCache` struct type; that
//     name stays in gamecontext.h only because renaming the field itself
//     across Config/miscdata.cpp, Net/checksumproviderdebugprint.cpp and
//     every other already-scored reader was judged not worth the risk for
//     what is a purely cosmetic type-name mismatch (the byte layout every
//     one of those files already assumes is exactly right).
//
//  2. `m_providerCount`'s real value and meaning.  It is not "how many of
//     the leading slots are live checksum providers" - it is a fixed
//     constant, 0x14 (20), written here once and never touched again
//     anywhere else this session could find.  `context->m_providerCount`
//     and `context->m_pCache` are, byte for byte, the `m_count`/`m_data`
//     fields of an embedded `TSFixedArray<CSynchronousData*>` (storm.h)
//     occupying this object's own +0x08..+0x14 - `m_alloc` (the array's
//     third field, at +0x08, one word ahead of `m_providerCount`) was
//     simply nameless dead space in gamecontext.h's struct until this
//     session, because nothing had ever needed to write it. This body is
//     exactly `TSFixedArray<CSynchronousData*>::SetCount(0x14)` (see
//     tsfixedarray.inl's own comment on why the per-element construction
//     loop drops out entirely for a scalar pointer T) inlined flat - one
//     caller, a literal count, nothing left for /Ob2 to keep out of line -
//     followed by an explicit `memset` over the live region that SetCount
//     itself does not do (a pointer T's default-initialising placement
//     `new` leaves the slot untouched; the checksum-provider array needs
//     every slot to start null, since GameDataContextChecksumProviderHash
//     and the debug print alike test each entry against zero before
//     dereferencing it).
//
//     The live per-tick walk (CNetData::FireTickNotification, via
//     GameDataContextChecksumProviderHash) and the dead debug print
//     (ChecksumProviderDebugPrint's own hard-coded 20) therefore cover the
//     *exact same* 20 slots, always - there is no smaller live subset. Any
//     of the 20 slots that is non-null when FireTickNotification runs
//     contributes to the "net" leg of the per-tick sync value, including
//     the three repurposed lazy-singleton slots (CAbilityDatabase at index
//     5, CAbilityCustomData at 16, CMiscCustom at 19) once whichever of
//     them has been lazily created for the session - not just the six
//     categories the debug print names.
//============================================================================
#include <string.h>

#define TSFIXEDARRAY_TYPENAME ".PAVCSynchronousData@@"
#include "gamecontext.h"
#include "synchronousdata.h"
#include "tsfixedarray.inl"

template void TSFixedArray<CSynchronousData*>::SetAlloc(unsigned int);
template void TSFixedArray<CSynchronousData*>::SetCount(unsigned int);

//  gamecontext.h's own struct, restated with the embedded
//  TSFixedArray<CSynchronousData*> named rather than split into
//  m_providerCount/m_pCache - byte-for-byte the same object, used only
//  inside this TU so gamecontext.h's own field names stay untouched for
//  every other file that already reads them. +0x08 (TSFixedArray::m_alloc)
//  is one field-width before m_providerCount (+0x0C, i.e. m_count) - the
//  gap gamecontext.h's own struct never named because nothing had reason
//  to write it until now.
struct SGameDataContextLayout
{
    CConfigFile* m_pConfig;
    CConfigFile* m_pConfigOverride;
    TSFixedArray<CSynchronousData*> m_providers;
};

CGameDataContext* __fastcall ConstructGameDataContext(CGameDataContext* context,
                                                       unsigned int)
{
    SGameDataContextLayout* self = (SGameDataContextLayout*)context;
    self->m_pConfig = 0;
    self->m_pConfigOverride = 0;

    TSFixedArray<CSynchronousData*>* providers = &self->m_providers;

    //  TSFixedArray declares no constructor of its own (storm.h) - value-
    //  initialising a temporary zeroes the aggregate, matching the trivial
    //  default state every other reader of this array assumes before
    //  anything has been registered into it.
    *providers = TSFixedArray<CSynchronousData*>();
    providers->SetCount(0x14);

    memset(providers->m_data, 0, providers->m_count * sizeof(CSynchronousData*));

    return context;
}

//----------------------------------------------------------------------------
//  The teardown half of 0x6F007A50 (see the file header above for why the
//  surrounding orchestrator is not reconstructed here).  Not given a
//  funcmap entry of its own - it is not a real, separately-callable
//  function in the shipped binary, just this file's record of what that
//  middle third does: fetch the context, free the provider array itself
//  (tagged with CSynchronousData's own pointer-type descriptor, confirming
//  the type above) and then the context object, using two different Storm
//  free-with-tag idioms - an array-shaped free (-2) for the provider
//  block, a plain scalar free (-1, tagged just "delete") for the context -
//  then null the slot. Neither individual provider is freed here -
//  whatever still owns a live one at shutdown leaks it, exactly as the
//  dump shows.
//----------------------------------------------------------------------------
#if 0
void TeardownGameDataContext_NotReconstructed()
{
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    if (context != 0)
    {
        if (context->m_pCache != 0)
            SMemFree(context->m_pCache, ".PAVCSynchronousData@@", -2, 0);
        SMemFree(context, "delete", -1, 0);
    }

    SetThreadLocalSlot(kThreadLocalGameData, 0);
}
#endif
