//============================================================================
//  A cluster of small JassInstance accessors sitting right after
//  SetField48/GetField48 (jassinstancespawn.cpp) and GetGlobalSlot
//  (jassinstanceglobalslot.cpp) - plain getters for fields those two files
//  already named, plus InitSpawnFields, the one real setter in the group.
//  See jassinstance.h for what little this closure resolves about each
//  field.
//============================================================================
#include "jassinstance.h"

//  0x6F459610
void JassInstance::InitSpawnFields(JassInstance* parent)
{
    m_cachedGlobalSlotIndex = -1;
    m_field18 = 0;
    m_field1C = parent->m_field1C - 1;
}

//  0x6F459580
void** JassInstance::GetGlobalsBase()
{
    return m_field2858;
}

//  0x6F4596B0
int JassInstance::GetGlobalSlotIndex(void* slot)
{
    void** base = (void**)*m_field2858;
    return (int)((void**)slot - base);
}

//  0x6F459640
void* JassInstance::GetHandleTableEntry(unsigned int index)
{
    return (char*)m_field2874->m_entries + index * 0x10;
}

//  0x6F459690
void* JassInstance::GetField2888Element(unsigned int index)
{
    return (*m_field2888)[index];
}

//  0x6F459560
void* JassInstance::GetNativeFuncPtr()
{
    return m_nativeFuncPtr;
}

//  0x6F459570
void* JassInstance::GetNativeFuncArg()
{
    return m_nativeFuncArg;
}
