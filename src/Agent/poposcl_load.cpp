//============================================================================
//  0x6F495FC0 - NIpse::CPoPosCl::Load, vtable slot 10, SHARED with
//  NTempest::CLinkedList<NIpse::CPoPosCl>. See poposcl_saveload.cpp's own
//  header comment (left as the class-level TODO note for this exact
//  address) for the full shape this closes:
//
//  Base CPoPos::Load first, then +0x9C/+0xA0 (ReadDword), +0xA4
//  (ReadDwordAlt3), the seven CFloats +0xB0..+0xCC (ReadCFloat, chaining
//  each call's own returned CDataStore* the way Save's own WriteCFloat
//  chain already does), +0xD0/+0xD4/+0xD8 (ReadDwordAlt3), one flag via
//  ReadDwordAlt2 (the "linked" flag Save wrote via WriteDwordAlt3 - an
//  asymmetric reader/writer pairing this whole family's own save/load
//  helpers already mix freely, e.g. WriteDwordAlt2/ReadDwordAlt3
//  elsewhere in this same function), then the STORE object's own vtable
//  slot 5 (not this object's - a genuine indirect dispatch on `store`
//  itself, meaning not established by this cluster) compared against a
//  fixed threshold (0x1771) to decide whether a SECOND flag is read at
//  all (also via ReadDwordAlt2); reads `m_relatedAgent` back via
//  `SPathRefReader::ReadEntry` (already real, THUNK); conditionally links
//  `this` into the shared `g_pTimeSync+0x438` global list via
//  `SPathOrderLink::InsertAfter` (already real) when not already linked;
//  unconditionally calls `TeardownAndOptionallyRecreatePoSeparate` with
//  the second flag as `create` (the SAME helper `Slot4` uses,
//  poposcl_relatedagentslot4_teardown.cpp); and only when BOTH the
//  threshold flag and the second flag are set, dispatches the freshly
//  (re)created `m_poSeparate`'s own vtable slot 12, Load.
//
//  Written as a naked, instruction-for-instruction transcription (not a
//  real C++ call sequence): the store object's own vtable-slot-5 dispatch
//  has no established signature to declare a typedef against, and the
//  heavy register-caching-across-calls shape (each ReadCFloat chaining
//  the prior call's own returned `CDataStore*`, three different stack
//  slots reused for three different purposes at different points) is the
//  same "would not reproduce through an ordinary C++ call sequence"
//  reasoning Unit/unit_notifymodelchanged.cpp's and
//  Agent/cpopointerflis_notifycore.cpp's own headers already document.
//============================================================================
#include "poposcl.h"
#include "pathorder.h"
#include "cdatastore.h"
#include "game.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
struct SPathRefReader { void* ReadEntry(); };

#pragma warning(disable : 4035)   // naked: no return value, leaves via the
                                  // transcribed asm's own retn

namespace NIpse {

__declspec(naked) void CPoPosCl::Load(CDataStore* /*store*/)
{
    __asm
    {
        sub     esp, 24h
        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     edi, [esp+38h]          ; store (arg_0)
        mov     esi, ecx                ; this
        push    edi
        mov     [esp+34h], esi
        call    CPoPos::Load
        xor     eax, eax
        lea     ebx, [esi+9Ch]          ; &m_clusT
        push    ebx
        mov     ecx, edi
        mov     [esp+18h], eax
        mov     [esp+14h], eax
        call    CDataStore::ReadDword
        lea     eax, [ebx+4]            ; &m_clusB
        push    eax
        mov     ecx, edi
        call    CDataStore::ReadDword
        add     ebx, 8                  ; &m_reservedA4
        push    ebx
        mov     ecx, edi
        call    CDataStore::ReadDwordAlt3
        lea     ecx, [esi+0D0h]         ; &m_pclX
        mov     [esp+2Ch], ecx
        lea     ecx, [esi+0C4h]         ; &m_ldl
        mov     [esp+20h], ecx
        lea     eax, [esi+0C8h]         ; &m_lor
        lea     edx, [esi+0CCh]         ; &m_lod
        lea     ecx, [esi+0B4h]         ; &m_acc
        mov     [esp+24h], eax
        mov     [esp+28h], edx
        lea     eax, [esi+0B8h]         ; &m_ang
        mov     [esp+18h], ecx
        lea     edx, [esi+0B0h]         ; &m_glr
        mov     ecx, edi
        lea     ebx, [esi+0C0h]         ; &m_lmg
        lea     ebp, [esi+0BCh]         ; &m_prw
        mov     [esp+1Ch], eax
        call    ReadCFloat              ; ReadCFloat(store, &m_glr)
        mov     ecx, [esp+18h]
        mov     edx, ecx
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_acc)
        mov     ecx, [esp+1Ch]
        mov     edx, ecx
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_ang)
        mov     edx, ebp
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_prw)
        mov     edx, ebx
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_lmg)
        mov     edx, [esp+20h]
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_ldl)
        mov     edx, [esp+24h]
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_lor)
        mov     edx, [esp+28h]
        mov     ecx, eax
        call    ReadCFloat              ; ReadCFloat(store, &m_lod)
        mov     edi, [esp+2Ch]          ; &m_pclX
        mov     esi, eax                ; store (chained)
        push    edi
        mov     ecx, esi
        call    CDataStore::ReadDwordAlt3
        add     edi, 4                  ; &m_pclY
        push    edi
        mov     ecx, esi
        call    CDataStore::ReadDwordAlt3
        mov     edi, [esp+30h]          ; this
        lea     edx, [edi+0D8h]         ; &m_opt
        push    edx
        mov     ecx, esi
        call    CDataStore::ReadDwordAlt3
        lea     eax, [esp+14h]          ; &linkedFlag
        push    eax
        mov     ecx, esi
        call    CDataStore::ReadDwordAlt2
        mov     esi, [esp+38h]          ; store, reloaded from the original arg
        mov     edx, [esi]              ; store's own vtable
        mov     eax, [edx+14h]          ; store's own vtable slot 5
        mov     ecx, esi
        call    eax
        xor     ebx, ebx
        cmp     eax, 1771h
        setge   bl                      ; ebx = (result >= 0x1771)
        test    ebx, ebx
        jz      skipThresholdFlag
        lea     ecx, [esp+10h]          ; &thresholdFlag
        push    ecx
        mov     ecx, esi
        call    CDataStore::ReadDwordAlt2
    skipThresholdFlag:
        mov     ecx, esi
        call    SPathRefReader::ReadEntry
        cmp     dword ptr [esp+14h], 0  ; linkedFlag
        mov     [edi+0A8h], eax         ; m_relatedAgent
        jz      skipListInsert
        cmp     dword ptr [edi+4], 0
        jnz     skipListInsert
        cmp     dword ptr [edi+8], 0
        jnz     skipListInsert
        mov     ecx, g_pTimeSync
        push    edi
        add     ecx, 438h
        call    SPathOrderLink::InsertAfter
    skipListInsert:
        mov     edx, [esp+10h]          ; thresholdFlag (0 if never read)
        push    0
        push    0
        push    0
        push    edx
        mov     ecx, edi
        call    CPoPosCl::TeardownAndOptionallyRecreatePoSeparate
        test    ebx, ebx
        jz      skipPoSeparateLoad
        cmp     dword ptr [esp+10h], 0
        jz      skipPoSeparateLoad
        mov     ecx, [edi+0ACh]         ; m_poSeparate
        mov     eax, [ecx]              ; its own vtable
        mov     edx, [eax+30h]          ; vtable slot 12, Load
        push    esi                     ; store
        call    edx
    skipPoSeparateLoad:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 24h
        retn    4
    }
}

}  // namespace NIpse
