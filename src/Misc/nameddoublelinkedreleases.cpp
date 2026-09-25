//============================================================================
//  Three separate, byte-identical bodies at 0x6F4EB980 / 0x6F4EDCE0 /
//  0x6F4EDD70: unlink two embedded TSLink<T> memberships (at +0x04 and
//  +0x0C of `this`) and, if a name string at +0x14 was ever set, free it
//  through Storm tagged with a fixed internal Storm source location -
//  `push 0, push 0A29h, push offset "e:\Drive1\temp\buildwar3x\Storm\H..."`
//  - rather than the caller-supplied RTTI-descriptor tag the CHandleObject/
//  CBaseManaged family (Misc/ccamera.cpp) uses.  That fixed tag is Storm's
//  own internal free call, not this object's, which is what tells the two
//  families apart even though the two-TSLink-plus-name shape is otherwise
//  the same idea.
//
//  None of the three has a caller in this batch's own dump, so nothing
//  beyond these three fields is asserted about any of the three owners'
//  layout, and the three are kept as three distinct real bodies (matching
//  three distinct real shipped addresses) rather than folded into one
//  shared helper - nothing here says the three are actually the same
//  class.  `retn 0` each.
//============================================================================
#include "storm.h"

static const char s_stormInternalTag[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\Hxxxx.cpp";

//  The two-TSLink-plus-name shape all three share.  Only the first 0x18
//  bytes are read by any of the three bodies; nothing past that is
//  established.
struct SDoubleLinkedNamed
{
    char        m_reserved00[0x04];   // +0x00
    TSLink<SDoubleLinkedNamed> m_link04;   // +0x04
    TSLink<SDoubleLinkedNamed> m_link0C;   // +0x0C
    char*       m_pName;               // +0x14

    void ReleaseLinksAndName();
};

//  Three separate, self-contained copies of the same body - deliberately not
//  factored through one shared helper.  Each address needs the full
//  instruction sequence inline; a real call to a shared out-of-line helper
//  would show up as an extra call/ret pair none of the three dumps have,
//  and relying on /O2 to inline a three-times-called helper back out again
//  is not something to bet a match on.

//  0x6F4EB980.
void SDoubleLinkedNamed::ReleaseLinksAndName()
{
    if (m_pName)
        SMemFree(m_pName, s_stormInternalTag, 0xA29, 0);

    m_link0C.Unlink();
    m_link04.Unlink();
}

//  0x6F4EDCE0.
struct SDoubleLinkedNamedB : SDoubleLinkedNamed { void ReleaseLinksAndName2(); };
void SDoubleLinkedNamedB::ReleaseLinksAndName2()
{
    if (m_pName)
        SMemFree(m_pName, s_stormInternalTag, 0xA29, 0);

    m_link0C.Unlink();
    m_link04.Unlink();
}

//  0x6F4EDD70.
struct SDoubleLinkedNamedC : SDoubleLinkedNamed { void ReleaseLinksAndName3(); };
void SDoubleLinkedNamedC::ReleaseLinksAndName3()
{
    if (m_pName)
        SMemFree(m_pName, s_stormInternalTag, 0xA29, 0);

    m_link0C.Unlink();
    m_link04.Unlink();
}
