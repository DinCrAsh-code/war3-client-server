//============================================================================
//  0x6F5CB490 / 0x6F5CB540 - fill the two create-handler registries from a
//  static table of {name, handler, context} triples.
//
//  Two identical bodies over two node types, one per registry.  Each row is
//  looked up first and only made when it is missing, so registering the
//  same name twice replaces the handler rather than growing the table.
//
//  The `count == 0` test really is emitted twice - once as the early
//  return 0 and once as the loop's own guard, which answers 1.  The two are
//  a different comparison on the same register (`== 0` against `0 <
//  count`) and the shipped compiler did not fold them together either.
//
//  Own translation unit: the lookup and NewNode it calls are both real
//  calls, and both live in other files.
//============================================================================
#include "framereg.h"

unsigned int __stdcall Storm_590(const char* s);

int __fastcall RegisterFrameCreateHandlers(const FRAMECREATEHANDLERENTRY* table,
                                           unsigned int count)
{
    if (count == 0 || table == 0)
        return 0;

    for (unsigned int i = 0; i < count; i++)
    {
        FRAMENAMECREATEHANDLER* node = g_frameCreateHandlers.Ptr(table[i].m_name);
        if (node == 0)
        {
            //  The name named here and not read out of the row three more
            //  times: it has to survive the NewNode call, and naming an
            //  intermediate is what parks it in a callee-saved register
            //  (docs/msvc-vc8-idioms.md).
            const char* name = table[i].m_name;
            unsigned int hash = Storm_590(name);

            node = g_frameCreateHandlers.NewNode(hash, 0, 0);
            node->m_hashval = hash;
            AssignHashKeyStr(&node->m_key, name);
        }

        node->m_handler = table[i].m_handler;
        node->m_context = table[i].m_context;
    }

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F5CB540 - the same body over the simple-frame registry.
//----------------------------------------------------------------------------
int __fastcall RegisterSimpleFrameCreateHandlers(
                                    const FRAMECREATEHANDLERENTRY* table,
                                    unsigned int count)
{
    if (count == 0 || table == 0)
        return 0;

    for (unsigned int i = 0; i < count; i++)
    {
        SIMPLEFRAMENAMECREATEHANDLER* node =
            g_simpleFrameCreateHandlers.Ptr(table[i].m_name);
        if (node == 0)
        {
            const char* name = table[i].m_name;
            unsigned int hash = Storm_590(name);

            node = g_simpleFrameCreateHandlers.NewNode(hash, 0, 0);
            node->m_hashval = hash;
            AssignHashKeyStr(&node->m_key, name);
        }

        node->m_handler = table[i].m_handler;
        node->m_context = table[i].m_context;
    }

    return 1;
}
