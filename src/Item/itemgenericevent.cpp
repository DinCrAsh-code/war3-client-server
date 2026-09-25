//============================================================================
//  0x6F40B5A0 - build a CEvent record on the stack (vftable
//  ??_7CEvent@@6B@, id 0x80259, one caller-supplied subject dword) and
//  dispatch it through vtable slot 4 - the same shape SItemDeathEvent::Fire
//  (0x6F2AD4D0, item_valuefloor.cpp) already carries with a different event
//  id and no argument.  Same reason it stays a redirect: the
//  __except_handler4-shaped SEH frame this toolchain cannot reproduce
//  (docs/msvc-vc8-idioms.md).  A thiscall member with one stack argument -
//  `retn 4`.
//
//  Reached from CItem's slots 73/74 through SItemDeathEvent::Fire's own
//  body, which is itself entirely unhooked shipped code - reconstructed
//  here only so this address has a scored entry of its own.
//============================================================================

struct SGenericWorldEvent
{
    void Fire(int subject);
};

__declspec(naked) void SGenericWorldEvent::Fire(int)
{
    __asm
    {
        mov     eax, 06F40B5A0h
        jmp     eax
    }
}
