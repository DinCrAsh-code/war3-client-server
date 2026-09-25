//============================================================================
//  0x6F4EBA10 - drop one reference on a shared SprAnimList, and take it out
//  of the registry when the last one goes.
//
//  The count is decremented in place and tested for zero (`add [esi+18h],
//  0FFFFFFFFh` / `jnz`), which is `--m_refs != 0` and not `m_refs -= 1;
//  if (m_refs)`.
//
//  The second step is a hand-written slot dispatch and has to be: the
//  registry's free-a-node member is reached through its own vftable
//  (`mov eax, dword_6FAB80FC` reads the table object's first dword, which
//  is the vftable, and `mov edx,[eax]` takes slot 0), with the table itself
//  as the receiver.  Nothing in this dump reconstructs that slot, so what
//  is recovered here is the call - the receiver, the one argument and the
//  `retn 4` that argument implies - and not the method.
//
//  Own translation unit: both calls out of it are real.
//============================================================================
#include "sprresource.h"

void __fastcall ReleaseSprAnimList(SSprAnimList* list)
{
    if (--list->m_refs != 0)
        return;

    g_sprAnimLists.Unlink(list);

    typedef void (__thiscall *FreeSprAnimListFn)(SSprAnimListRegistry*,
                                                 SSprAnimList*);
    ((FreeSprAnimListFn)(*(void***)&g_sprAnimLists)[0])(&g_sprAnimLists, list);
}
