//============================================================================
//  SWidgetAgentQuery - the 0x2C-byte request block CWidget's vtable slot 68
//  builds on its stack and hands to the global agent factory.
//
//  Its first word is the literal 'lga+' (0x2B61676C), the same '+'-suffixed
//  four-character tag family as the agile type ids (agiletype.h), so the
//  block identifies itself to whatever reads it.
//============================================================================
#ifndef WIDGETAGENTQUERY_H
#define WIDGETAGENTQUERY_H

class CAgent;

struct SWidgetAgentQuery
{

    unsigned int m_tag;         // +0x00 - 'lga+'
    unsigned int m_typeId;      // +0x04
    unsigned int m_reserved08;  // +0x08 - 0
    void*        m_pTypeObject; // +0x0C - QueryAgileTypeObject's answer
    void*        m_pPool;       // +0x10
    unsigned int m_reserved14;  // +0x14 - 0
    unsigned int m_reserved18;  // +0x18 - 0
    unsigned int m_reserved1C;  // +0x1C - 0
    unsigned int m_reserved20;  // +0x20 - 0
    unsigned int m_reserved24;  // +0x24 - -1
    unsigned int m_reserved28;  // +0x28 - -1
};

//  0x6F472C20 - fill the whole block in from a type id and a pool.  A
//  __fastcall free function and not a member for the same reason
//  SubmitWidgetAgentQuery is one: the type id arrives in edx and only the
//  pool is on the stack (`retn 4`).
SWidgetAgentQuery* __fastcall InitWidgetAgentQuery(SWidgetAgentQuery* query,
                                                   unsigned int typeId,
                                                   void* pool);

//  0x6F48AB80 - hand a filled-in block to the global maker and run two of
//  the made object's own slots against it.  A __fastcall free function, not
//  a member: see widgetagentsubmit.cpp.
//  Returns the object the maker built, not the agent inside it: the one
//  caller here reaches through its +0x54 to find the CAgent it actually
//  wants (widget_slot68.cpp), so the return type stays opaque.
void* __fastcall SubmitWidgetAgentQuery(SWidgetAgentQuery* query,
                                        int wantPrepare, int wantFinish);

#endif
