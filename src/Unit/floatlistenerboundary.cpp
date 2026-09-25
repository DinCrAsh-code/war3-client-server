//============================================================================
//  0x6F480F80 - FloatListener::SetBoundary: publish which side of the
//  boundary fires and then the boundary itself, onto the object the maker
//  built.
//
//  The twin of FloatModifier::SetSpan / SetTarget (floatpropmodifier.h):
//  same resolve of this listener's own {handle, typeTag} pair through
//  LookupHandle, same "the resolved object is the sink" shape.  What is new
//  is what the sink turns out to be - a CDrivenRelation (agentrelation.h),
//  reached through its +0x54 mode word and its +0x50 CFloat target, which
//  is the same object CDrivenRelation::Update samples against.
//
//  The two calls are ordered and their `resample` flags are not the same:
//  the mode is set with the flag clear and the target with it set, so the
//  relation re-samples exactly once, after both halves of the boundary are
//  in place.  Setting the mode with the flag set would sample against the
//  *old* target.
//
//  Own translation unit: FloatListener::Init reaches it with a real call.
//============================================================================
#include "floatlistener.h"
#include "agentrelation.h"

void FloatListener::SetBoundary(int above, const CFloat* boundary)
{
    CDrivenRelation* sink = (CDrivenRelation*)LookupHandle(m_handle, m_typeTag);

    sink->SetMode(above, 0);
    sink->SetTarget(boundary, 1);
}
