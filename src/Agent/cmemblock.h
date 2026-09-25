//============================================================================
//  NTempest::CMemBlock (`??_7CMemBlock@NTempest@@6B@`, 0x6F953D38) - one
//  level up from NTempest::CEntity in the chain (see centity.h).  One
//  vtable slot only (index 0), confirmed the same direct-read way as
//  CEntity's own.
//
//  Unlike CEntity's own trivial version, this scalar deleting destructor
//  DOES call a real base teardown first - Pathfinding/cellbuffer.h's own
//  `ReleaseCMemBlockBase` (0x6F4B3770, already reconstructed as a naked
//  redirect there: stamp CMemBlock's own vtable, FreeBlock(), restamp
//  CEntity's) - and it DOES carry the `this != 0` guard on the free that
//  CEntity's own body omits, the same shape CAgentBaseAbs's own
//  ScalarDeletingDestructor (Agent/cagentbaseabs_module_batch2.cpp) and
//  NIpse::CRlAgent's own (Pathfinding/crlagent_scalardtor.cpp) use whenever
//  a real destructor call precedes the free.
//============================================================================
#ifndef CMEMBLOCK_H
#define CMEMBLOCK_H

namespace NTempest {

class CMemBlock
{
public:
    //  0x6F4B38F0 - vtable slot 0.
    void* ScalarDeletingDestructor(unsigned int flags);
};

}  // namespace NTempest

#endif  // CMEMBLOCK_H
