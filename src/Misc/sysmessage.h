//============================================================================
//  AUMsgBuffer - one queued system message, and the list they hang on.
//
//  The class name and the two source line numbers are the shipped binary's
//  own: every allocation in this family tags itself either with the RTTI
//  descriptor name "AUMsgBuffer" (the node) or with the literal
//  ".\\SysMessage.cpp" and a line number (the text).  Nothing here is
//  guessed at from behaviour.
//
//  Node layout is fixed by its own allocator call site (0x6F4CBF60, which
//  asks Storm for 0x14 bytes and initialises every field of them):
//
//      +0x00 / +0x04   the TSLink pair every list element in this binary has
//      +0x08           the message text, Storm-duplicated and Storm-freed
//      +0x0C           a float, zeroed on construction
//      +0x10           an int, 3 on construction
//============================================================================
#ifndef SYSMESSAGE_H
#define SYSMESSAGE_H

#include "storm.h"

class AUMsgBuffer;

class AUMsgBuffer
{
public:
    //  0x6F4CB8D0 - replace the message text, freeing whatever was there.
    //  A null `text` clears the field rather than duplicating anything, and
    //  `type` is stored either way - which is why the shipped code has two
    //  separate `retn 8` tails that both write +0x10.
    void SetText(const char* text, int type);

    //  The ordinary Storm list membership every element in this binary
    //  carries.  Named as a TSLink rather than two dwords because
    //  AUMsgList::FreeAll inlines TSLink::Unlink over it verbatim -
    //  including the recover-the-link-offset-from-the-successor step that
    //  only makes sense for this type (storm.h).
    TSLink<AUMsgBuffer> m_link;   // +0x00 / +0x04
    char*  m_text;       // +0x08
    float  m_timer;      // +0x0C
    int    m_type;       // +0x10
};

//----------------------------------------------------------------------------
//  The list they hang on - a plain TSExplicitList's own layout, which is
//  what `this + 8` being the tail link and TSLink::Unlink being inlined over
//  the elements together establish.
//----------------------------------------------------------------------------
class AUMsgList
{
public:
    //  0x6F4CBE70 - drain the list from the tail, releasing each message's
    //  text and then the node itself.  Not TSExplicitList::UnlinkAll
    //  (0x6F46B560): that one only unlinks, and this one also hands both
    //  allocations back to Storm, so the walk has to re-read the tail every
    //  iteration rather than stepping a cursor.
    void FreeAll();

    int                 m_linkoffset;   // +0x00
    TSLink<AUMsgBuffer> m_terminator;   // +0x04 / +0x08
};

#endif
