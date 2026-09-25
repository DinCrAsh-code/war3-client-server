//============================================================================
//  FACEDATA's own render-device notification and a small allocate/release
//  pair, off the same closure Misc/citemclosure_facedata_hashtable.cpp and
//  Misc/citemclosure_facedata_metrics.cpp document (CItem vtable slots
//  32/104, batch K, depth 12-25+).
//============================================================================
#include "storm.h"
#include "renderdevice.h"
#include <string.h>

//----------------------------------------------------------------------------
//  0x6F5265A0 - FACEDATA::NotifyRenderDevice: forwards `this` to
//  g_renderDevice's own vtable slot 0xAC, the same
//  `(*(void***)g_renderDevice)[N/4]` shape every other g_renderDevice caller
//  in Render/ uses (renderdeviceslot8c.cpp, renderdeviceslot3c44.cpp).
//----------------------------------------------------------------------------
typedef void (__thiscall *RenderDeviceSlotACFn)(void*, void*);

void __fastcall FACEDATA_NotifyRenderDevice(void* self)
{
    ((RenderDeviceSlotACFn)(*(void***)g_renderDevice)[0xAC / 4])(g_renderDevice, self);
}

//----------------------------------------------------------------------------
//  0x6F7C9660 - FACEDATA::AllocateAndZero: allocate `size` bytes through an
//  allocator function pointer carried at +0x04 of `self` (a direct
//  `__thiscall` function-pointer field, not a vtable slot - the call site
//  never reloads a vtable pointer first), zero it, and hand the pointer
//  back through `*outPtr`.  Returns 0 on success (including the `size<=0`
//  no-op case) and 0x40 when the allocator itself answers null.
//----------------------------------------------------------------------------
struct SFaceAllocator
{
    void*                              m_reserved00;
    void* (__thiscall *m_alloc)(void*);   // 0x04
};

//  Takes `void*` rather than `SFaceAllocator*` because several other
//  translation units in this closure call it against their own local view
//  of the same object - MSVC bakes the parameter type into the mangled
//  name, so a `SFaceAllocator*` here and a `void*` (or a different struct)
//  at a re-declaration are two symbols that never meet, which
//  `link_check.py` catches as a live link failure invisible to `verify.py`.
int __fastcall FACEDATA_AllocateAndZero(void* selfVoid, int size,
                                         void** outPtr)
{
    SFaceAllocator* self = (SFaceAllocator*)selfVoid;

    if (size <= 0)
        goto zero_out;

    {
        void* p = self->m_alloc(self);
        *outPtr = p;
        if (p == 0)
            return 0x40;

        memset(p, 0, size);
        return 0;
    }

zero_out:
    *outPtr = 0;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F7C96B0 - FACEDATA::ReleaseIfSet: if `*slot` is non-null, call a
//  release function pointer carried at +0x08 of `self` (again a direct
//  function-pointer field, called with `self` itself as the implicit
//  `this` - not `*slot`) and clear `*slot`.
//----------------------------------------------------------------------------
struct SFaceReleaser
{
    char                      m_reserved00[0x08];
    void (__thiscall *m_release)(void*);   // 0x08
};

//  `void*` rather than `SFaceReleaser*` - see FACEDATA_AllocateAndZero's
//  own note above.
void __fastcall FACEDATA_ReleaseIfSet(void* selfVoid, void** slot)
{
    SFaceReleaser* self = (SFaceReleaser*)selfVoid;

    if (slot == 0)
        return;

    void* value = *slot;
    if (value == 0)
        return;

    self->m_release(self);
    *slot = 0;
}

//----------------------------------------------------------------------------
//  0x6F7C9CC0 - FACEDATA::ComputeGlyphMetrics: four FixedMul-then-round-to-
//  64 calls filling in a metrics block's four output fields from four
//  16-bit source fields.  `__declspec(naked)`, for the same reason as
//  FACEDATA_ReleaseFieldViaEdi below: `edi` (the field source record) and
//  `esi` (the metrics block being filled in) both arrive already set, with
//  no `mov edi/esi, ecx/edx` anywhere in the shipped body - this is another
//  whole-program-optimised internal helper (its one caller in this closure,
//  sub_6F7CBBF0/FACEDATA::ScaleGlyphSize, keeps both live across the call).
//  The third and fourth calls use a *different* divisor field (`[esi+4]`
//  instead of `[esi+8]`) for the fourth call only - not a typo, the shipped
//  stream reloads `edx` between the third call's return and the fourth
//  call's own setup, and this transcribes that reload exactly where it
//  happens.
//----------------------------------------------------------------------------
int __fastcall FACEDATA_FixedMul(int a, int b);

__declspec(naked) void FACEDATA_ComputeGlyphMetrics()
{
    __asm {
        movsx   ecx, word ptr [edi + 0x46]
        push    ebx
        mov     ebx, [esi + 8]
        mov     edx, ebx
        call    FACEDATA_FixedMul
        add     eax, 0x20
        and     eax, 0xFFFFFFC0
        mov     [esi + 0x0C], eax
        movsx   ecx, word ptr [edi + 0x48]
        mov     edx, ebx
        call    FACEDATA_FixedMul
        add     eax, 0x20
        and     eax, 0xFFFFFFC0
        mov     [esi + 0x10], eax
        movsx   ecx, word ptr [edi + 0x4A]
        mov     edx, ebx
        call    FACEDATA_FixedMul
        mov     edx, [esi + 4]
        add     eax, 0x20
        and     eax, 0xFFFFFFC0
        mov     [esi + 0x14], eax
        movsx   ecx, word ptr [edi + 0x4C]
        call    FACEDATA_FixedMul
        add     eax, 0x20
        and     eax, 0xFFFFFFC0
        mov     [esi + 0x18], eax
        pop     ebx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F7C96F0 - FACEDATA's own "release one field, then clear the owning
//  pointer" step: dereference `edi` (the field being torn down) to reach a
//  record, call its own vtable slot 0x18 if set, then - unless the caller's
//  one stack argument says not to - call ReleaseIfSet above on the record's
//  own +0x1C sub-object, and finally null the field at `edi` itself.
//
//  `__declspec(naked)`, and has to be: `edi` arrives holding the field's
//  address already, with no `mov edi, ecx` anywhere in the shipped body -
//  this is a whole-program-optimised internal helper whose one caller in
//  this closure (FACEDATA::TeardownField, citemclosure_facedata_dtor.cpp)
//  keeps it live in `edi` across the call, and there is no MSVC calling
//  convention that reaches an argument in `edi`.  A naked body is not
//  subject to the register/scheduling excuses CLAUDE.md carves out for
//  everything else - MSVC does no register allocation inside a pure `__asm`
//  block - so this is transcribed instruction for instruction rather than
//  reshaped, and the `edi` parameter reads exactly like the shipped stream.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FACEDATA_ReleaseFieldViaEdi(void* /*stopFlag*/)
{
    __asm {
        push    ecx
        push    esi
        mov     esi, [edi]
        cmp     dword ptr [esi + 0x18], 0
        mov     [esp + 4], esi
        jz      skip_slot18_call
        mov     eax, [esi + 0x18]
        mov     ecx, esi
        call    eax
skip_slot18_call:
        cmp     dword ptr [esp + 0x0C], 0
        jnz     skip_release
        mov     ecx, [esi + 0x1C]
        lea     edx, [esp + 4]
        call    FACEDATA_ReleaseIfSet
skip_release:
        mov     dword ptr [edi], 0
        pop     esi
        pop     ecx
        retn    4
    }
}
