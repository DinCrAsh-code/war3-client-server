//============================================================================
//  Redirect for the string-reference registry's node maker.  Ptr() (its
//  sibling, 0x6F4F09C0) is reconstructed in Widget/modelhashtablec.cpp.
//
//  NOT a reconstruction: it dispatches through the table's own AllocNode
//  virtual, whose real body is below this dump's depth cut.  The stack
//  argument count is the one its shipped `retn 0Ch` cleans, fixed by the
//  call site: the node maker is handed three dwords and the caller does
//  not adjust esp afterwards.
//============================================================================
#include "stringref.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(SStringRef* SStringRefTable::NewNode(unsigned int, void*, void*),
           0x6F4F1680)

#undef ADDR_THUNK
