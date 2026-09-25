//============================================================================
//  0x6F37BA90 - release whatever CSelectable parks at +0x50 (slot 97).
//
//  Was a `mov eax,<address> / jmp eax` redirect in configgates.cpp on the
//  grounds that it is "a rendering-side object; nothing in these three
//  trees builds one".  Seven instructions: detach the visual, then destroy
//  it, both through the object itself, the second as a tail jump.
//
//  Both callees are genuinely rendering-side and stay thunks, with their
//  shipped `retn <n>` recorded below.
//
//  Own translation unit: CSelectable's slot 97 reaches this with a real
//  call and it makes two of its own.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F379D10 (`retn 4`, one stack argument) and 0x6F37B880 (`retn`, none),
//  both thiscall members of the visual.  ~90 and ~470 instructions of
//  sprite and attachment teardown in the 0x6F379xxx/0x6F37Bxxx rendering
//  modules; out of scope.  The two counts are what
//  tools/thunk_abi_audit.py checks these declarations against.
//----------------------------------------------------------------------------
struct SSelectionVisual
{
    void __thiscall Detach(int mode);
    void __thiscall Destroy();
};

__declspec(naked) void __thiscall SSelectionVisual::Detach(int)
{
    __asm
    {
        mov     eax, 06F379D10h
        jmp     eax
    }
}

__declspec(naked) void __thiscall SSelectionVisual::Destroy()
{
    __asm
    {
        mov     eax, 06F37B880h
        jmp     eax
    }
}

void __fastcall ReleaseSelectionVisualObject(void* visual)
{
    SSelectionVisual* self = (SSelectionVisual*)visual;
    self->Detach(1);
    return self->Destroy();
}
