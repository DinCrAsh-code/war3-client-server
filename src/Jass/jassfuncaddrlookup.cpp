//============================================================================
//  0x6F45E8C0 - look a native's address up in this instance's own
//  TAllocatorHashTable<FuncAddr,HASHKEY_CONSTSTR,256> (the table the
//  constructor builds and RTTI-stamps at +0x2884 - see jassinstance.h).
//  `retn 4`.  The node's own +0x18 is presumably FuncAddr's resolved
//  address field, going by the class name, but FuncAddr's own layout is
//  jassvm-batch-2's reconstruction and out of this session's reach.
//============================================================================
#include "jassinstance.h"

//  0x6F45AE80 - kept a naked redirect, not a reconstruction: this session's
//  own read of the disassembly (this instance's own FuncAddr table, called
//  right after +0x2884) conflicts with jassvm-batch-2's own claim on this
//  same address (TAllocatorHashTable<JASSTYPENODE2,HASHKEY_STR,64>::
//  PtrInterned, an unrelated table) - either misattributed here, or, like
//  this repo's own ComputeChunk/UnlinkAll instantiations, the two tables'
//  Ptr bodies happen to compile byte-identical and share the address.  No
//  funcmap.FUNCTIONS row claims this name for it either way.
__declspec(naked) void* JassFuncAddrTableView::Ptr(unsigned int)
{
    __asm
    {
        mov eax, 06F45AE80h
        jmp eax
    }
}

void* JassInstance::FindFuncAddr(unsigned int key)
{
    void* node = ((JassFuncAddrTableView*)m_funcAddrTable)->Ptr(key);
    void* result = 0;
    if (node)
        result = *(void**)((char*)node + 0x18);
    return result;
}
