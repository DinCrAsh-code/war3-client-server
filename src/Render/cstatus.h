//============================================================================
//  CStatus - the stack-local error sink the texture loader is handed.
//
//  The class name is the shipped one: 0x6F4C3E90 stamps `??_7CStatus@@6B@`
//  into it, and the module's own allocation tag,
//  `e:\drive1\temp\buildwar3x\engine\source\base\Status.h`, is what says it
//  belongs to the engine's base layer rather than to Storm.  The nested
//  entry type is `CStatus::STATUSENTRY`, from the type descriptor
//  `??_R0?AUSTATUSENTRY@CStatus@@@8` at 0x6FA85CAC - the only nested struct
//  CStatus has.
//
//  Sixteen bytes: a vtable and one intrusive list of entries.  The list is a
//  `TSList<STATUSENTRY,0>` - its walk dereferences the tail value *directly*
//  as a node, which is storm.h's compile-time-link-offset template at
//  offset 0.
//============================================================================
#ifndef CSTATUS_H
#define CSTATUS_H

#include "storm.h"
#include "dataallocator.h"

unsigned int __stdcall Storm_506(const char* s);
void         __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);

class CStatus
{
public:
    //  One message.  The link is the entry's first member, which is what
    //  makes the list's link offset zero; the text is a Storm-allocated
    //  copy the list owns.
    struct STATUSENTRY
    {
        TSLink<STATUSENTRY> m_link;     // +0x00
        char*               m_text;     // +0x08
        //  +0x0C - a per-entry category/severity tag, read by BuildText's
        //  two still-unnamed-here callees (Render/statusmeasuretext.cpp,
        //  Render/statuscopytext.cpp) and compared `>=` against BuildText's
        //  own stack argument.  Not established by this call tree as
        //  anything more specific than "the filter BuildText applies" -
        //  see BuildText's own comment below for why its parameter is named
        //  `minType` rather than the `separator` this header used to guess.
        int                  m_type;    // +0x0C
    };

    //  A TSList with one extra member.  Derived rather than a member so the
    //  drain is a method of the *list* - which is what 0x6F4C3E80's
    //  `add ecx,4` / `jmp` says it is.
    class ENTRYLIST : public TSList<STATUSENTRY, 0>
    {
    public:
        //  0x6F4C3C40 - Render/statusfreeall.cpp.
        void FreeAll();
    };

    //  Inline because the shipped code has all four stores in
    //  CSimpleTexture::SetTexturePath's own body.
    CStatus() {}
    //  0x6F4C3E90 - Render/statusdtor.cpp.
    ~CStatus();

    //  0x6F4C3E80 - Render/statusclear.cpp.
    void ClearEntries();

    //  0x6F4C3660 - is the list empty?  Reads the tail link at +0x0C and
    //  masks it exactly as TSList::Tail() does, so an empty list's negative
    //  terminator answers "yes" rather than "here is a node".  `retn 0`;
    //  still a redirect.
    int IsEmpty();
    //  0x6F4C3A00 - join every entry into one Storm-allocated string, tagged
    //  ".\\Status.cpp" line 0xA9, and hand the caller the buffer to free.
    //  `retn 4`; still a redirect.  The parameter is named `minType` rather
    //  than the `separator` this header used to guess before its two
    //  callees below this dump's earlier depth cut were reachable: the
    //  shipped body pushes it straight through to both of them, which
    //  compare it against each STATUSENTRY's own +0x0C tag (`>=`) to decide
    //  whether that entry's text is measured/copied at all, and neither
    //  ever writes a separator character into the output buffer.
    char* BuildText(int minType);

    //  0x6F4C39D0 - BuildText's first call: the total byte length (Storm_506,
    //  i.e. strlen) of every entry whose +0x0C tag is >= minType, walking
    //  the list tail-to-head via each entry's own m_link.m_prevlink exactly
    //  as TSList's Tail()/Prev() do (storm.h) - not through those accessors
    //  themselves, because the shipped body reads m_list's own +0x0C field
    //  (`m_list.TailLink()`) directly rather than going through Tail()'s
    //  extra `> 0 ? ... : 0` mask, the same way IsEmpty's own comment
    //  already establishes for this same field.  Render/statusmeasuretext.cpp.
    int MeasureFilteredTextLength(int minType);

    //  0x6F4C3980 - BuildText's second call: copy the same filtered walk's
    //  text runs back-to-back into `dest` (null-terminating it first).
    //  The first entry whose text does not fit in the `capacity` bytes
    //  left stops the whole walk - it is not skipped in favour of a
    //  shorter entry further up the list.  No separator is ever written
    //  between two entries' text.  Render/statuscopytext.cpp.
    void CopyFilteredText(char* dest, int capacity, int minType);

    //  Slot 0.  Body in Frame/vtableboundary.cpp, for the reason
    //  FRAMENODE::Destroy gives in frame.h.
    virtual void Destroy(int deleteFlags);

    ENTRYLIST m_list;                   // +0x04
};

//: unk_6FAB7D00 - the pool every STATUSENTRY comes out of and goes back to.
extern CDataAllocator g_statusEntryPool;

#endif
