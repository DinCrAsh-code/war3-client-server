//============================================================================
//  0x6F5C93A0 / 0x6F5C93E0 - ResolveFrameCreateHandler /
//  ResolveSimpleFrameCreateHandler: look `name` up in this path's own
//  name -> handler registry (g_frameCreateHandlers / g_simpleFrameCreateHandlers,
//  framereg.h); on a hit, hand back the resolved context too (when the
//  caller wants one) and answer the handler.  On a miss, `kind` indexes a
//  small default-handler table instead (g_defaultFrameCreateHandlers /
//  g_defaultSimpleFrameCreateHandlers) - neither table's real element
//  count or contents are read anywhere in this closure, so both stay
//  incomplete-sized `extern` arrays, the same convention
//  Config/preferences.h's g_preferenceKinds already uses for a shipped
//  table this repo never has to build.
//============================================================================
#include "framereg.h"

extern void* const g_defaultFrameCreateHandlers[];        // dword_6FA8C768
extern void* const g_defaultSimpleFrameCreateHandlers[];  // off_6FA8C780

//  0x6F5C93A0 - the non-simple-frame path's own resolver, ResolveSimpleFrameCreateHandler's
//  twin.  Same shape, same order of operations; declared here since
//  CreateFrameFromDef (its only caller) is out of this batch's scope and
//  never names it.
void* __fastcall ResolveFrameCreateHandler(const char* name, void* kind,
                                           void** outContext)
{
    FRAMENAMECREATEHANDLER* node = g_frameCreateHandlers.Ptr(name);
    if (node)
    {
        if (outContext)
            *outContext = node->m_context;
        return node->m_handler;
    }

    return g_defaultFrameCreateHandlers[(unsigned int)kind];
}

SimpleFrameCreateFn __fastcall ResolveSimpleFrameCreateHandler(
        const char* name, void* kind, void** outContext)
{
    SIMPLEFRAMENAMECREATEHANDLER* node = g_simpleFrameCreateHandlers.Ptr(name);
    if (node)
    {
        if (outContext)
            *outContext = node->m_context;
        return (SimpleFrameCreateFn)node->m_handler;
    }

    return (SimpleFrameCreateFn)
        g_defaultSimpleFrameCreateHandlers[(unsigned int)kind];
}
