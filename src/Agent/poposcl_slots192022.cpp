//============================================================================
//  NIpse::CPoPosCl - vtable slots 19 (0x6F4945F0), 20 (0x6F494620) and 22
//  (0x6F4954F0), shared with NTempest::CLinkedList<NIpse::CPoPosCl>. See
//  poposcl.h for the class-level header comment. Slot 21 (0x6F495140)
//  stays `TODO` - see poposcl.h's own comment there.
//============================================================================
#include "poposcl.h"
#include "game.h"
#include "pathorder.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4945F0 - vtable slot 19, own override. Base `CPoPos::Slot19(arg)`
//  first (THUNK - see popos_slots1920_thunk.cpp), then - only if `this` is
//  not already linked into the shared +0x438 global list (own `m_prev`
//  m_next` at +4/+8 both null) - inserts it there.
//----------------------------------------------------------------------------
void CPoPosCl::Slot19(unsigned int arg)
{
    ((CPoPos*)this)->Slot19(arg);

    char* base = (char*)this;
    if (*(void**)(base + 4) == 0 && *(void**)(base + 8) == 0)
    {
        ((SPathOrderLink*)((char*)g_pTimeSync + 0x438))->InsertAfter((SPathOrderLink*)this);
    }
}

//----------------------------------------------------------------------------
//  0x6F494620 - vtable slot 20, own override. Same shape as Slot19 above,
//  no argument.
//----------------------------------------------------------------------------
void CPoPosCl::Slot20()
{
    ((CPoPos*)this)->Slot20();

    char* base = (char*)this;
    if (*(void**)(base + 4) == 0 && *(void**)(base + 8) == 0)
    {
        ((SPathOrderLink*)((char*)g_pTimeSync + 0x438))->InsertAfter((SPathOrderLink*)this);
    }
}

//----------------------------------------------------------------------------
//  0x6F4954F0 - vtable slot 22, own override. Copies the base `m_fieldE`
//  (CPoPos's own +0x8C) into this class's own +0xC8 (`m_lor`), stamps
//  +0xCC (`m_lod`) with the shared `dword_6FAAE470` CFloat-zero-shaped
//  constant, then unlinks `this` from whatever doubly-linked list it is
//  threaded on.
//----------------------------------------------------------------------------
void CPoPosCl::Slot22()
{
    char* base = (char*)this;
    *(unsigned int*)(base + 0xC8) = *(const unsigned int*)(base + 0x8C);
    *(unsigned int*)(base + 0xCC) = g_CFloatZero.m_bits;

    void* prev = *(void**)(base + 4);
    void* next = *(void**)(base + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(base + 8) = 0;
    *(void**)(base + 4) = 0;
}

}  // namespace NIpse
