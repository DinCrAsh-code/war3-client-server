//============================================================================
//  0x6F5C9610 - CreateSimpleFrameByName: the frame library's one-call
//  "give me the frame this FDF name defines".
//
//  Three things have to hold, and the shipped body checks them in this
//  order: the name is in the definition registry at all, and the definition
//  carries the simple-frame bit (0x8000000) in its flag word at +0x48.  Only
//  then does the node's own create path run.  A definition that exists but
//  belongs to the *other* create path answers null rather than being built
//  the wrong way.
//
//  A genuine __fastcall: the name arrives in ecx and the parent in edx, and
//  only the context is pushed (`retn 4`).  The name is pushed straight back
//  out as the registry lookup's own stack argument, which is the shipped
//  `push ecx` in the prologue.
//
//  Own translation unit: CGameUI's constructor calls it eleven times for
//  real.
//============================================================================
#include "framereg.h"

CSimpleFrame* __fastcall CreateSimpleFrameByName(const char* name,
                                                 CSimpleFrame* parent,
                                                 void* context)
{
    BASEFRAMEHASHNODE* def = g_frameDefs.Ptr(name);

    if (def != 0 && (def->m_flags & kFrameDefSimpleCreatable) != 0)
        return CreateSimpleFrameFromDef(def, parent, context);

    return 0;
}
