//============================================================================
//  CMessageFrame - the on-screen message log the system-message sink posts
//  into.
//
//  The class name and both node type names are the shipped binary's own,
//  out of the strings its own allocations carry: ".\\CMessageFrame.cpp" for
//  the text duplication (0x6F623830) and the RTTI descriptors
//  ".?AUTEXTMSG@@" and ".?AUTIMERDATA@@" for the two nodes.  (IDA renders
//  the descriptor labels with the `AU` of the `struct` mangling prefix glued
//  to the name - `aAutextmsg` - which is why the temptation is to call them
//  "AUTextMsg"; the bytes at 0x6FA9B064 and 0x6FA9B07C say TEXTMSG and
//  TIMERDATA.)
//
//  Only what the system-message path reaches is modelled.
//============================================================================
#ifndef MESSAGEFRAME_H
#define MESSAGEFRAME_H

#include "storm.h"

//----------------------------------------------------------------------------
//  One posted line.  0x38 bytes, every one of which the allocation site
//  zeroes except the timer it is about to store.  Both intrusive links live
//  somewhere in the last 0x20 bytes; nothing here reads either directly -
//  the lists carry their own offsets - so neither is placed.
//----------------------------------------------------------------------------
struct TEXTMSG
{
    char*        m_text;      // +0x00 - Storm-duplicated
    int          m_field04;   // +0x04
    int          m_colour;    // +0x08 - 0xFF000000 | r<<16 | g<<8 | b
    int          m_field0C;   // +0x0C
    float        m_timer;     // +0x10
    unsigned int m_flags;     // +0x14 - bit 0 cleared and bit 1 set on post
    char         m_reserved18[0x38 - 0x18];
};

//----------------------------------------------------------------------------
//  One expiry.  Its own link sits at +0x10, which is what the two stores the
//  allocation site makes (+0x10 and +0x14, and nothing else) say.
//----------------------------------------------------------------------------
struct TIMERDATA
{
    void*           m_owner;   // +0x00 - the frame's own +0x190 sub-object
    TEXTMSG*        m_message; // +0x04
    float           m_when;    // +0x08
    int             m_field0C; // +0x0C
    TSLink<TIMERDATA> m_link;  // +0x10 / +0x14
};

class CMessageFrame
{
public:
    //  0x6F623830 - post one line.  `retn 14h`: five stack words, and the
    //  second of them is a `float`, which is why the shipped call sites
    //  reserve its slot with a `push` and then `fstp` over it.
    void AddMessage(const char* text, float timer, int field04,
                    const int* colour, int insertByPriority);

    //  0x6F622670 - splice a message into the display list after a given
    //  one.  `retn 0Ch`; not reconstructed.
    void InsertAfter(TEXTMSG* message, int one, TEXTMSG* after);

    char                     m_reserved000[0x178];
    TSExplicitList<TEXTMSG>  m_messages;   // +0x178 - every live line
    TSExplicitList<TEXTMSG>  m_display;    // +0x184 - the drawn order
    char                     m_reserved190[0x1A8 - 0x190];
    TSExplicitList<TIMERDATA> m_timers;    // +0x1A8
};

//  0x6F623ED0 - make one, and 0x6F6067F0 / 0x6F622350 / 0x6F6C5CB0 /
//  0x6F5FB960 - the four things the lazy creation path does to it.  All
//  five are naked redirects in Frame/messageframethunks.cpp; every argument
//  count is the shipped call site's own push count.
//  Three arguments, not two: 0x6F623ED0 keeps ecx and edx (`mov esi,edx` /
//  `mov edi,ecx`) and reads one more off the stack (`[esp+20h+arg_0]`),
//  then ends `retn 4`.  Its one call site leaves ecx holding the host's own
//  `this` and only writes edx, which is why the owner is spelled out here -
//  declared __fastcall(int, void*) the thunk would push nothing into a
//  callee that pops four bytes, and every caller's frame would shift.
CMessageFrame* __fastcall MakeMessageFrame(void* owner, void* parent,
                                           int height);

//  Storm ordinal 507 - duplicate a string.
char* __stdcall Storm_507(const char* text, const char* logfilename,
                          int logline);

#endif
