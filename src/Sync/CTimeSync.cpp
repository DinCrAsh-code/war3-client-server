//============================================================================
//  Time synchronisation front end.
//============================================================================
#include "game.h"

//============================================================================
//  DELIBERATE BEHAVIOUR CHANGE - uncomment to make F3 teleport every loose
//  ground item to the local player's selected unit + (5.0, 0).
//  Implementation and the full write-up are in src/feature_itemdebug.cpp.
//
//  The switch is here, in the .cpp, and not in a header on purpose:
//  tools/build.sh fingerprints only the compile flags, so a header edit
//  does not force the TUs that include it to rebuild and the define would
//  go stale in every file but the one that happened to be recompiled.
//
//  This function is the polling site only - it is the hottest hooked
//  function in the build (every object whose position is queried goes
//  through it), so it stands in for a per-frame tick, and none of its own
//  arguments are used by the poll.  Nothing about the extrapolation itself
//  changes.
//
//  Needs FEATURE_ITEM_CREATE_LOG in src/item_place.cpp switched on as well:
//  that is what populates the item table this walks, so with only this one
//  on, F3 will report "0 tracked" and move nothing.
//
//  While it is commented out this file compiles to exactly what it did
//  before and keeps its score; uncommenting it adds a real call and will
//  show up as extra instructions against the shipped body.
//============================================================================
//#define FEATURE_ITEM_TELEPORT_F3

#ifdef FEATURE_ITEM_TELEPORT_F3
//  Declared here rather than in a header - see the note above.
void FeatureItemDebug_PollHotkey();
#endif

//----------------------------------------------------------------------------
//  0x6F4885A0 - carry an object's sample forward to the channel's own sample
//  point.
//----------------------------------------------------------------------------
CFloat CTimeSync::Extrapolate(const CHandleObject* object,
                              const CFloat& value, int time) const
{
#ifdef FEATURE_ITEM_TELEPORT_F3
    FeatureItemDebug_PollHotkey();
#endif

    const CSyncChannel* channel = GetChannel(object);
    return channel->Extrapolate(channel->m_value, channel->m_time, value, time);
}

//----------------------------------------------------------------------------
//  0x6F483430 - hand the object's current channel sample back to the
//  caller, e.g. so it can refresh its own cached copy of it.
//----------------------------------------------------------------------------
void CTimeSync::GetChannelSample(const CHandleObject* object, CFloat* outValue,
                                 int* outTime) const
{
    const CSyncChannel* channel = GetChannel(object);
    *outValue = channel->m_value;
    *outTime = channel->m_time;
}
