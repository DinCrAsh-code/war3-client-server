//============================================================================
//  0x6F472990 - CAgent's vtable slot 23 (+0x5C).  See agent.h.
//
//  Own translation unit: 0x6F472xxx is the agile-type/handle module, a long
//  way from agent.cpp's own cluster, and both of its calls (LookupHandle and
//  the gate below) are real out-of-line calls in the shipped code - the
//  second of them a tail jump.
//============================================================================
#include "agent.h"
#include "game.h"

//  0x6F4A6970 - the rendering-side gate the resolved object is handed to.
//  Reconstructed in agentslot5cgate.cpp; declared, not defined, here so
//  this slot reaches it with the real tail jump the shipped code has.
void __fastcall AgentSlot5CGate(CHandleObject* obj);

//  'l','g','a','+' read low byte first - the same reversed-FourCC spelling
//  as agiletype.h's kAgileRootTypeId ('+aga'), one letter along.
static const int kAgileLTag = 0x2B61676C;

void CAgent::Method_0x5C()
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    if (obj == 0)
        return;

    //  The branchless `xor/cmp/setnz/sub/and` docs/msvc-vc8-idioms.md
    //  records for `return p->tag == t ? p : 0;`, written as the ternary
    //  and tested afterwards rather than as a plain `if (obj->m_kindTag !=
    //  kAgileLTag) return;` - the latter compiles to a compare and a
    //  conditional jump with no mask arithmetic at all.
    CHandleObject* checked = (obj->m_kindTag == kAgileLTag) ? obj : 0;
    if (checked == 0)
        return;

    AgentSlot5CGate(checked);
}
