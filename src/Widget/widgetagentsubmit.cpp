//============================================================================
//  0x6F48AB80 - SubmitWidgetAgentQuery: hand a filled-in request block
//  (widgetagentquery.h) to the global maker and then run two of the made
//  object's own slots against it.
//
//  The maker is a *function pointer* at +0x10 of the object g_pTimeSync
//  names (game.h, 0x6FAB73D8), not a vtable slot: the shipped code loads
//  the global, loads +0x10 out of it and calls that with the request block
//  still in ecx and a zero in edx - a __fastcall through a stored pointer,
//  where a vtable call would have moved the object into ecx first.  Nothing
//  else in this codebase reaches +0x10 of that object, so no name is
//  invented for what it really is beyond what this call site shows.
//
//  A __fastcall free function, not a member: `wantPrepare` arrives in edx
//  and only `wantFinish` is on the stack (`retn 4`), where a thiscall
//  member would have pushed both and cleaned eight.
//
//  Both gates are independent, so a caller can ask for either half alone.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "widgetagentquery.h"
#include "game.h"

//  The maker itself: (request block, 0) -> the object it made.
typedef void* (__fastcall *MakeAgentFn)(SWidgetAgentQuery* query, int zero);

//  g_pTimeSync's own +0x10, reached through an overlay rather than by
//  adding a member to CTimeSync: game.h is included nearly everywhere, and
//  re-touching it shifts every already-matching listing's $Tnnnn temporary
//  names (widgetpathrefadjust.h says the same about SPathRefHandle).
struct SAgentMakerHost
{
    char        m_reserved00[0x10];
    MakeAgentFn m_pMake;    // +0x10
};

typedef void (__thiscall *QuerySlotFn)(void* self, SWidgetAgentQuery* query);

void* __fastcall SubmitWidgetAgentQuery(SWidgetAgentQuery* query,
                                        int wantPrepare, int wantFinish)
{
    void* made = ((SAgentMakerHost*)g_pTimeSync)->m_pMake(query, 0);

    if (wantPrepare)
        ((QuerySlotFn)(*(void***)made)[0x0C / 4])(made, query);
    if (wantFinish)
        ((QuerySlotFn)(*(void***)made)[0x44 / 4])(made, query);

    return made;
}
