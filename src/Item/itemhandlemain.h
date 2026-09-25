//============================================================================
//  JASS_SetItemPosition's own hItem-to-CAgent* resolver (0x6F3BEB50) and
//  the lazy Storm-style singleton it depends on.  See itemhandlemain.cpp.
//============================================================================
#ifndef ITEMHANDLEMAIN_H
#define ITEMHANDLEMAIN_H

#include "itemhandletable.h"

extern void* g_unk6FAB65F4;   // dword_6FAB65F4 - the singleton's own `this`

//  The object dword_6FAB65F4 points at.  Nothing about it is observable
//  beyond the one member below; it exists as a type purely so that member
//  is spelled __thiscall, which is what it is.
class CItemSlotSingletonHost
{
public:
    //  0x6F3A8060 - a lazy singleton bootstrap: a FourCC into the agile
    //  type registry, that type's allocator into the same 0x2C-byte
    //  request block CWidget::ClearPlayerMaskBits builds, and the made
    //  holder into the slot at +0x1C.  Reconstructed in
    //  itemslotsingleton.cpp - it was a redirect, and as a redirect it was
    //  also a *self-redirect* (hooking it patched the very address its own
    //  body jumps to), which the operator confirmed live as a hang.  A
    //  real __thiscall member (`this` = g_unk6FAB65F4, no stack args, bare
    //  `retn`).
    //
    //  It used to be a free function that loaded ecx from g_unk6FAB65F4
    //  inside its own naked body.  That hid one instruction from every
    //  caller: the shipped code loads dword_6FAB65F4 at the *call site*
    //  (0x6F3BEB50, 0x6F3BDCB0 and 0x6F3C74F0 all do), so the load belongs
    //  to the caller's instruction stream, not to the thunk's.
    SItemSlotTableHolder* GetSlotTable();

    //  0x6F39F5F0 - intern a C string into the shared hashed-string table
    //  and hand back its JASS string handle.  `this` = g_unk6FAB65F4, one
    //  stack argument, `retn 4`.  Deep (tail-jumps into a Storm string-hash
    //  table chain, sub_6F44B6E0/6F44B2E0/6F45D210, none dumped in this
    //  batch's own scope) - thunked; see jassnatives_thunks.cpp.  The one
    //  caller in this batch is MakeJassStringHandleFromCString
    //  (jassmakestringhandle.cpp), which is the null check every caller of
    //  this member goes through first in the shipped code.
    unsigned int InternCString(const char* str);
};

//  The same pointer g_unk6FAB65F4 already names, typed.  Spelled as a cast
//  at each use rather than as a second `extern` so that funcmap.py's DATA
//  table keeps exactly one C++ name for dword_6FAB65F4.
#define g_pItemSlotHost ((CItemSlotSingletonHost*)g_unk6FAB65F4)

//  0x6F3BEB50 - resolve a JASS item handle to its underlying CAgent*:
//  look the handle up in the singleton's own index table, type-check the
//  result against 'item', then re-validate it through the *global* handle
//  table (LookupHandle on the candidate's own m_handle/m_typeTag) and two
//  more guards (a kind tag, a pending flag) before accepting it.  Returns
//  null if any step fails.
CAgent* __fastcall ResolveItemHandle(int hItem);

#endif
