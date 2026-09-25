//============================================================================
//  CUnit batch-15.  Two GameUI forwarders (slot 16/107 closure): each takes
//  a fixed number of stack dwords, looks up the GameUI singleton
//  (GetGameUI(0,0), gameui.h) and, if it exists, forwards `this`/edx plus
//  every stack argument straight through to a deeper GameUI member -
//  0x6F2F75F0 (5 own stack args -> 7 total) / 0x6F2F44D0 (6 own stack args
//  -> 8 total).  Both deeper callees have real bodies in this dump but pull
//  in their own further-unnamed closures (sub_6F2F4330/sub_6F342070/
//  sub_6F34F630/sub_6F372C60/sub_6F337A70) well past this batch's scope;
//  declared here only for the `retn` byte count their own dump lines give
//  directly, so the two forwarders above compile against a real, dump-
//  confirmed signature rather than a guessed one.
//
//  Two more of the same shape added for CAbilityButton's own icon
//  accessors (Method_0x1B0/Method_0x1C0, abilitybutton_bigslots.cpp):
//  0x6F332A50 (-> 0x6F2F4330, 9 total forwarded) and 0x6F332C20
//  (-> 0x6F2F46B0, 3 total forwarded).  Same reasoning - the deeper
//  member has a real body in this dump but pulls in its own
//  further-unnamed closure (sub_6F3777D0 / sub_6F372C60+sub_6F337BA0)
//  past this batch's scope, so it is declared here only for its own
//  `retn` byte count.
//============================================================================
#include "gameui.h"

//  0x6F2F75F0 - `retn 1Ch` (7 stack dwords), this in ecx and nothing else in
//  a register - every one of the 7 forwarded values is a stack push at the
//  call site.
struct GameUIMember6F2F75F0Host { void __thiscall Call(int, int, int, int, int, int, int); };
__declspec(naked) void __thiscall GameUIMember6F2F75F0Host::Call(int, int, int, int, int, int, int)
{
    __asm { mov eax, 06F2F75F0h }
    __asm { jmp eax }
}

//  0x6F2F44D0 - `retn 20h` (8 stack dwords), this in ecx, same all-stack shape.
struct GameUIMember6F2F44D0Host { void __thiscall Call(int, int, int, int, int, int, int, int); };
__declspec(naked) void __thiscall GameUIMember6F2F44D0Host::Call(int, int, int, int, int, int, int, int)
{
    __asm { mov eax, 06F2F44D0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F332A10 - `retn 14h` (5 stack dwords).
//----------------------------------------------------------------------------
void __fastcall ForwardToGameUI_6F2F75F0(void* a, int b, int c0, int c1, int c2, int c3, int c4)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((GameUIMember6F2F75F0Host*)ui)->Call((int)a, b, c0, c1, c2, c3, c4);
}

//----------------------------------------------------------------------------
//  0x6F332B50 - `retn 18h` (6 stack dwords).
//----------------------------------------------------------------------------
void __fastcall ForwardToGameUI_6F2F44D0(void* a, int b, int c0, int c1, int c2, int c3, int c4, int c5)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((GameUIMember6F2F44D0Host*)ui)->Call((int)a, b, c0, c1, c2, c3, c4, c5);
}

//  0x6F2F4330 - `retn 24h` (9 stack dwords), this in ecx, same all-stack
//  shape as the two hosts above.
struct GameUIMember6F2F4330Host { void __thiscall Call(int, int, int, int, int, int, int, int, int); };
__declspec(naked) void __thiscall GameUIMember6F2F4330Host::Call(int, int, int, int, int, int, int, int, int)
{
    __asm { mov eax, 06F2F4330h }
    __asm { jmp eax }
}

//  0x6F2F46B0 - `retn 0Ch` (3 stack dwords).
struct GameUIMember6F2F46B0Host { void __thiscall Call(int, int, int); };
__declspec(naked) void __thiscall GameUIMember6F2F46B0Host::Call(int, int, int)
{
    __asm { mov eax, 06F2F46B0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F332A50 - `retn 1Ch` (7 stack dwords).
//----------------------------------------------------------------------------
void __fastcall ForwardToGameUI_6F2F4330(void* a, int b, int c0, int c1, int c2, int c3, int c4, int c5, int c6)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((GameUIMember6F2F4330Host*)ui)->Call((int)a, b, c0, c1, c2, c3, c4, c5, c6);
}

//----------------------------------------------------------------------------
//  0x6F332C20 - `retn 4` (1 stack dword).
//----------------------------------------------------------------------------
void __fastcall ForwardToGameUI_6F2F46B0(void* a, int b, int c0)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((GameUIMember6F2F46B0Host*)ui)->Call((int)a, b, c0);
}

//  0x6F2F4380 / 0x6F2F4710 - both `retn 0Ch` (3 stack dwords), same
//  all-stack, this-in-ecx shape as the two hosts above - two more
//  members Method_0x1C0 (abilitybutton_bigslots.cpp) reaches through
//  their own one-stack-arg forwarders (0x6F332AA0/0x6F332C50).
struct GameUIMember6F2F4380Host { void __thiscall Call(int, int, int); };
__declspec(naked) void __thiscall GameUIMember6F2F4380Host::Call(int, int, int)
{
    __asm { mov eax, 06F2F4380h }
    __asm { jmp eax }
}

struct GameUIMember6F2F4710Host { void __thiscall Call(int, int, int); };
__declspec(naked) void __thiscall GameUIMember6F2F4710Host::Call(int, int, int)
{
    __asm { mov eax, 06F2F4710h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F332AA0 - `retn 4` (1 stack dword).
//----------------------------------------------------------------------------
void __fastcall ForwardToGameUI_6F2F4380(void* a, int b, int c0)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((GameUIMember6F2F4380Host*)ui)->Call((int)a, b, c0);
}

//----------------------------------------------------------------------------
//  0x6F332C50 - `retn 4` (1 stack dword).
//----------------------------------------------------------------------------
void __fastcall ForwardToGameUI_6F2F4710(void* a, int b, int c0)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((GameUIMember6F2F4710Host*)ui)->Call((int)a, b, c0);
}
