//============================================================================
//  jassvm-batch-4 - one small helper reached from the flagship AST
//  evaluator's own call tree.
//
//  `this` is some larger JASS engine/script-root object; the only field
//  this dump touches is +0x2874, a fixed-stride handle table (count at
//  +4, entries at +8, each entry 0x10 bytes with its own refcount at
//  +0xC) - the same [count,entries] shape the per-type handle resolvers
//  elsewhere in src/Jass use.  This is JassInstance's own +0x2874 field
//  (jassinstance.h - jassvm-core-A's own JassInstance::ReleaseCompiledFunc
//  reads the identical shape).  The struct itself moved to
//  jassenginehandlerefs.h so jassnativecalldispatch.cpp's own
//  DispatchWithArg (0x6F459750) can call AddHandleRef under this same C++
//  name instead of a second mangled symbol for the same address.
//============================================================================
#include "jassenginehandlerefs.h"

//  0x6F459660
void JassEngineHandleOwner::AddHandleRef(int id)
{
    if (id == 0)
        return;
    JassHandleTableHeader* table = m_handleTable;
    if (!table)
        return;
    if ((unsigned int)id >= table->m_count)
        return;
    char* entry = (char*)table->m_entries + id * 0x10;
    ++*(int*)(entry + 0xC);
}

extern "C" void JassEngineHandleOwner_Reference(JassEngineHandleOwner* e, int id)
{
    e->AddHandleRef(id);
}
