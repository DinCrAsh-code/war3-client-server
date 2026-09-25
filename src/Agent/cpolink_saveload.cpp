//============================================================================
//  0x6F4AA170 / 0x6F4AA1B0 - NIpse::CPoLink::Save/Load, vtable slots 9/10.
//  CRlProperty::Save/Load first, then m_targetHandle/m_targetSub (each via
//  the raw CDataStoreScratch::WriteDword/CDataStore::ReadDword pair) and
//  m_field80 (via CDataStoreScratch::WriteDwordAlt2/CDataStore::ReadDwordAlt3).
//============================================================================
#include "cpolink.h"
#include "rlproperty.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

namespace NIpse {

void CPoLink::Save(void* writer)
{
    ((CRlProperty*)this)->Save(writer);

    CDataStoreScratch* s = (CDataStoreScratch*)writer;
    s->WriteDword(m_targetHandle);
    s->WriteDword(m_targetSub);
    s->WriteDwordAlt2(m_field80);
}

//  Written naked: two plain-C++ attempts (chained through each call's own
//  returned pointer, and a cached-reader-plus-running-field-pointer
//  rewrite) both reproduced the right instructions but with `esi`/`edi`
//  swapped relative to the shipped code (register allocation only) rather
//  than matching address-for-address, so this is transcribed directly
//  instead - `reader` cached in esi before the base call, a running field
//  pointer in edi advanced +4 then +8 across the three reads.
__declspec(naked) void CPoLink::Load(void* /*reader*/)
{
    __asm
    {
        push    esi
        mov     esi, [esp+8]
        push    edi
        push    esi
        mov     edi, ecx
        call    CRlProperty::Load
        add     edi, 78h
        push    edi
        mov     ecx, esi
        call    CDataStore::ReadDword
        lea     eax, [edi+4]
        push    eax
        mov     ecx, esi
        call    CDataStore::ReadDword
        add     edi, 8
        push    edi
        mov     ecx, esi
        call    CDataStore::ReadDwordAlt3
        pop     edi
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
