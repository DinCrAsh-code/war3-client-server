//============================================================================
//  0x6F6323E0 - "is `key` registered in this table", asked under the table's
//  own lock.
//
//  A backwards walk of a TSExplicitList whose link offset sits at +0x1C8 and
//  whose terminator follows it at +0x1CC/+0x1D0 - the ordinary Storm list
//  layout (storm.h).  Each node is matched on the dword at its own +0x08.
//
//  Three shapes here are the shipped code's and not free choices:
//
//    * a null `this` is a real, handled case: it reports Storm error 0x57
//      and answers 0 without touching the lock, which is why the null test
//      is the very first thing and has its own `retn` ahead of the frame
//      the rest of the body needs;
//    * the walk enters on the *masked* Tail() and then steps on the raw
//      Prev(), testing its sign - the pattern docs/msvc-vc8-idioms.md
//      records for every TSExplicitList walk in this binary, and the reason
//      the loop's own condition is a `jg` on a value already known to be
//      non-negative;
//    * the answer is accumulated into a local that both exits return, so
//      the unlock is written once and MSVC tail-duplicates it, which is
//      exactly the two LeaveCriticalSection call sites the shipped stream
//      has.
//
//  What the table and its nodes are is not established by this call tree -
//  only the list's position, the key's offset and the lock's.  Named for
//  what it does.
//============================================================================
#include "storm.h"

//  0x6F6D83A0 / 0x6F6D83B0 - critsec.cpp.
struct CRITICAL_SECTION_TAG;
void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);

//  Storm.dll ordinal 465 - SErrSetLastError.
void __stdcall SErrSetLastError(unsigned int error);

namespace
{
    struct SKeyedNode
    {
        char  m_reserved00[8];
        void* m_key;        // +0x08
    };
}

//  The list's link offset is a run-time field here, so the template argument
//  is only the poison value every TSExplicitList is born with (storm.h); the
//  constructor never runs on this object in anything reconstructed here.
typedef TSExplicitList<SKeyedNode, TSLIST_LINKOFFSET_POISON> SKeyedNodeList;

class CLockedKeyTable
{
public:
    //  0x6F6323E0
    int __fastcall ContainsKey(void* key);

    char                    m_reserved0000[0x10];
    struct _RTL_CRITICAL_SECTION* m_lockStorage;   // +0x10 - the lock itself
    char                    m_reserved0014[0x1C8 - 0x14];
    SKeyedNodeList          m_nodes;               // +0x1C8
};

int __fastcall CLockedKeyTable::ContainsKey(void* key)
{
    if (this == 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    int found = 0;

    EnterCritSec((struct _RTL_CRITICAL_SECTION*)&m_lockStorage);

    int link = (int)m_nodes.Tail();
    while (link > 0)
    {
        SKeyedNode* node = (SKeyedNode*)link;
        if (node->m_key == key)
        {
            found = 1;
            break;
        }
        link = (int)m_nodes.Prev(node);
    }

    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)&m_lockStorage);
    return found;
}
