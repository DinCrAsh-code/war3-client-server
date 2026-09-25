//============================================================================
//  0x6F5C9560 - CreateFrameByName: the frame path's twin of
//  CreateSimpleFrameByName (0x6F5C9610).
//
//  Same registry lookup, the *other* flag bit (2 rather than 0x8000000), and
//  then one extra thing the simple path has no need of: a TABCHAINNODE list
//  built on the stack, handed to the create call as its last argument and
//  destroyed on the way out.  The list is what makes this function's own
//  frame an __except_handler4 one and is most of what does not reproduce.
//
//  The list is constructed *inside* the guarded arm, which is the shipped
//  order: the two tests come first and jump to a shared `xor eax, eax` tail
//  that never touches it.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "framereg.h"

CFrame* __fastcall CreateFrameByName(const char* name, CFrame* parent,
                                     void* reserved, void* handlerArg,
                                     void* context)
{
    BASEFRAMEHASHNODE* def = g_frameDefs.Ptr(name);

    if (def == 0 || (def->m_flags & kFrameDefFrameCreatable) == 0)
        return 0;

    STabChainList chain;

    return CreateFrameFromDef(def, parent, reserved, handlerArg, context,
                              &chain);
}
