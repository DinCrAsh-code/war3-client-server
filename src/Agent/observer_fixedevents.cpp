//============================================================================
//  CUnit batch-15 (slot 16 / RefreshOwningPlayerColor closure).
//
//  0x6F4269E0 / 0x6F426A20 - CObserver::PostEvent80264/PostEvent80265: two
//  fixed-id wrappers over CObserver::PostEvent (0x6F62A570,
//  observerpostevent.cpp).  See observer.h for why the ids are not folded
//  into the day/night names despite matching them numerically.
//============================================================================
#include "observer.h"

void CObserver::PostEvent80264(void* target)
{
    PostEvent(0x80264, target);
}

void CObserver::PostEvent80265(void* target)
{
    PostEvent(0x80265, target);
}
