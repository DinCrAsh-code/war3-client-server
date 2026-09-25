//============================================================================
//  See cdatastorecache128.cpp for the class-level notes.
//============================================================================
#ifndef CDATASTORECACHE128_H
#define CDATASTORECACHE128_H

#include "cdatastore.h"

//  The same pointer, called the way the shipped code calls it - with `this`
//  in ecx.  CDataStore::~CDataStore never has to load ecx (it is already
//  `this` on entry), so nothing there shows the convention; every site that
//  *inlines* that destructor does, and all of them set ecx first.
typedef void(__thiscall* RecycleBufferFn)(void* self, void** pField4, int* pField8, int* pField0xC);

class CDataStoreCache128 : public CDataStore
{
public:
    //  0x6F654B70
    CDataStoreCache128();
    //  0x6F654DD0
    ~CDataStoreCache128();

    //  vtable slot 0, called from the constructor on this object's own
    //  vtable - the same not-yet-fully-constructed-vtable call
    //  CDataStoreScratch and CDataStoreCache<1460> both make.
    virtual void m_reservedSlot0() { OnConstruct(&m_field4, &m_field8, &m_field0xC); }

    //  0x6F654BF0 - not in any of these dumps; its body is the only reading
    //  of m_inlineBuffer that makes the class work, exactly as the 1460
    //  instantiation's own OnConstruct entry argues.
    void OnConstruct(void** pField4, int* pField8, int* pField0xC);

    char m_inlineBuffer[128];   // +0x18
};

#endif
