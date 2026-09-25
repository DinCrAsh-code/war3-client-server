//============================================================================
//  Net::CopyProviderConfig, 0x6F6602D0 - the one generic per-object
//  config-copy body all three network providers' own Initialize reach
//  through a class-local alias at `this+0x18`
//  (NetProviderBnetInitTarget::CopyConfig, netproviderbnet_lifecycle.cpp;
//  NetProviderLoopInitTarget::CopyConfig, netproviderloop_initialize.cpp;
//  NetProviderLtcpInitTarget::CopyConfig, netproviderltcp_initialize.cpp) -
//  one shipped address, three call sites, so the real body lives here once
//  and each caller's own file just declares the shared free function.
//
//  `this` in ecx (the object's +0x18 sub-block), one stack arg (the
//  caller-owned config struct to copy from), `retn 4`.  Mostly a plain
//  0x29C-byte field-by-field/block copy - written as a naked transcription
//  rather than a typed struct assignment because one region (the growable
//  array header at +0x128) is NOT blindly copied: the dword at +0x128
//  itself is left alone (presumably a pointer/count pair the realloc
//  helper below re-derives) and only +0x12C/+0x130 are read and handed to
//  it, so a plain struct-copy statement would either miss that gap or
//  fabricate a semantics for it this call tree does not establish - the
//  naked form reproduces exactly what the shipped stream does, no more and
//  no less.
//============================================================================
#include "storm.h"

static const char aAuFixedstringD[] = ".?AVAUFixedString@@";   // type descriptor name, content not score-relevant

//  0x6F65B460 - the nested fixed-size-array realloc+copy the +0x128
//  region's own two fields (source pointer, size) feed: `this` (ecx) is
//  the destination array header (a 3-dword block - [0] a source-array-header
//  pointer that gets overwritten with the new block from Storm_405, [4] the
//  live element count, [8] the allocated element array itself), and the two
//  stack args are the source's own count and element pointer (the call
//  site's `lea ecx,[ebx+128h]` / `mov edx,[eax+8]` / `mov eax,[eax+4]` /
//  `push edx` / `push eax` in CopyConfig above feeds exactly this: count
//  closest to the return address, source pointer one further out).
//  `__fastcall` with the spurious unused `edx` slot is the only C++ spelling
//  that gets `ecx` alone into a naked free function without a member
//  wrapper - `__thiscall` on a free function is C3865, a naked member
//  function is C2488 (same shape already used for
//  `CNetData_SenderFromKeyLookup` in netcommand_dispatch.cpp).  `retn 8`
//  cleans the two stack dwords fastcall leaves there.
//
//  Each 0x20-byte element is copied 8 dwords at a time via `rep movsd`
//  (ecx=8) - the array's real element type is opaque here (elements are
//  moved as raw bytes, never read field-by-field), so it stays void*-typed
//  rather than inventing a struct the call tree never names.
__declspec(naked) void __fastcall
ReallocProviderConfigArray(void* header, void* /*unused_edx*/,
                            unsigned int count, void* srcElements)
{
    __asm
    {
        push    ebx
        mov     ebx, ecx
        mov     eax, [ebx+8]
        test    eax, eax
        push    ebp
        mov     ebp, [esp+0Ch]
        mov     [ebx], ebp
        jnz     short has_old_block
        test    ebp, ebp
        jz      short store_count
has_old_block:
        push    0
        push    0FFFFFFFEh
        push    offset aAuFixedstringD
        mov     ecx, ebp
        shl     ecx, 5
        push    ecx
        push    eax
        call    SMemReAlloc
        test    ebp, ebp
        mov     [ebx+8], eax
        jbe     short store_count
        push    esi
        xor     eax, eax
        mov     edx, ebp
        push    edi
copy_loop:
        mov     edi, [ebx+8]
        add     edi, eax
        jz      short skip_copy
        mov     ecx, [esp+18h]
        lea     esi, [eax+ecx]
        mov     ecx, 8
        rep     movsd
skip_copy:
        add     eax, 20h
        sub     edx, 1
        jnz     short copy_loop
        pop     edi
        pop     esi
store_count:
        mov     [ebx+4], ebp
        pop     ebp
        pop     ebx
        retn    8
    }
}

namespace Net
{

class CopyProviderConfigTarget
{
public:
    void CopyConfig(void* configStruct);
};

__declspec(naked) void CopyProviderConfigTarget::CopyConfig(void*)
{
    __asm
    {
        push    ebx
        mov     ebx, ecx
        push    ebp
        mov     ebp, [esp+0Ch]
        mov     eax, [ebp+0]
        mov     [ebx], eax
        mov     ecx, [ebp+4]
        mov     [ebx+4], ecx
        mov     edx, [ebp+8]
        mov     [ebx+8], edx
        mov     eax, [ebp+0Ch]
        mov     [ebx+0Ch], eax
        mov     ecx, [ebp+10h]
        mov     [ebx+10h], ecx
        mov     edx, [ebp+14h]
        mov     [ebx+14h], edx
        mov     eax, [ebp+18h]
        mov     [ebx+18h], eax
        mov     ecx, [ebp+1Ch]
        mov     [ebx+1Ch], ecx
        mov     edx, [ebp+20h]
        push    esi
        mov     [ebx+20h], edx
        mov     eax, [ebp+24h]
        push    edi
        mov     [ebx+24h], eax
        lea     esi, [ebp+28h]
        lea     edi, [ebx+28h]
        mov     ecx, 40h
        rep     movsd
        lea     eax, [ebp+128h]
        lea     ecx, [ebx+128h]
        cmp     ecx, eax
        jz      short skip_realloc
        mov     edx, [eax+8]
        mov     eax, [eax+4]
        push    edx
        push    eax
        call    ReallocProviderConfigArray
skip_realloc:
        mov     ecx, [ebp+134h]
        mov     [ebx+134h], ecx
        lea     esi, [ebp+138h]
        lea     edi, [ebx+138h]
        mov     ecx, 10h
        rep     movsd
        mov     edx, [ebp+178h]
        mov     [ebx+178h], edx
        lea     esi, [ebp+17Ch]
        lea     edi, [ebx+17Ch]
        mov     ecx, 8
        rep     movsd
        lea     esi, [ebp+19Ch]
        lea     edi, [ebx+19Ch]
        mov     ecx, 40h
        rep     movsd
        pop     edi
        pop     esi
        pop     ebp
        mov     eax, ebx
        pop     ebx
        retn    4
    }
}

}  // namespace Net
