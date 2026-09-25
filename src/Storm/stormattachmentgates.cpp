//============================================================================
//  0x6F00D970 and 0x6F00F790 - two of the one-line gates that sit between
//  this codebase's widget/item logic and the large Storm singleton.
//
//  Both were `mov eax,<address> / jmp eax` redirects.  Both are five
//  instructions: push the one argument, fetch the singleton, call one of
//  its members with `this` in ecx.  The singleton fetch
//  (GetStormSingletonA, 0x6F01F5A0) is already reconstructed here, so
//  nothing about the gates themselves was ever out of reach - what is out
//  of reach is one member each, and those two are declared below and left
//  as thunks with their real `retn <n>` recorded.
//
//  The argument is pushed *before* the singleton fetch runs, which is what
//  MSVC does for a call whose receiver is itself a call: arguments first,
//  object expression second.  Binding the singleton to a local ahead of the
//  call reverses that and loses the two pushes their position.
//
//  Own translation unit: four real calls out of it, and the callers of both
//  gates must reach them with real calls of their own.  They share a file
//  because the two are in the same 0x6F00xxxx gate module and neither calls
//  the other.
//============================================================================

struct SStormSingletonA;
SStormSingletonA* __fastcall GetStormSingletonA();

//----------------------------------------------------------------------------
//  The two singleton members the gates reach.  Both index the same 0xA0-byte
//  record array at +0x2CC that SIndexedArrayFlags (misc_leaves_batch3.cpp)
//  and SStormSingletonA::m_attachments (stormsingletona.h) already describe,
//  so they are declared on that same shape rather than on a new invented
//  class.
//
//  Neither is reconstructed, and both say why:
//
//   * 0x6F741760 is now reconstructed in stormattachmentslotrelease.cpp -
//     what this comment used to call "a Storm-side destructor at
//     0x6F4C40A0 that no dump here carries" is TRefCnt::Release, which has
//     been EXACT in refcnt.cpp since the first target.  Declared, not
//     defined, here.
//   * 0x6F7422B0 is a 99-instruction sweep over a terrain-tile rectangle,
//     rewriting five 16-bit slots per tile and finishing through another
//     unreconstructed Storm helper (0x6F7420F0).  `retn 4` - one stack
//     argument.
//
//  Both `retn <n>` values are read off the shipped bodies in
//  asm/sub_6F2B5C30_... and asm/sub_6F2ABEA0_..., which is what
//  tools/thunk_abi_audit.py checks these declarations against.
//----------------------------------------------------------------------------
struct SIndexedArrayRecords
{
    void* __thiscall ReleaseSlotObject(int index);
    void* __thiscall ReleaseIssuedSlot(int handle);
};

__declspec(naked) void* __thiscall SIndexedArrayRecords::ReleaseIssuedSlot(int)
{
    __asm
    {
        mov     eax, 06F7422B0h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F00D970 - drop the visual attachment in this slot.
//----------------------------------------------------------------------------
void __fastcall AttachmentRelease(unsigned int slot)
{
    ((SIndexedArrayRecords*)GetStormSingletonA())->ReleaseSlotObject((int)slot);
}

//----------------------------------------------------------------------------
//  0x6F00F790 - hand a handle back to whatever pool issued it.
//----------------------------------------------------------------------------
void __fastcall ReleaseIssuedHandle(unsigned int handle)
{
    ((SIndexedArrayRecords*)GetStormSingletonA())->ReleaseIssuedSlot((int)handle);
}
