//============================================================================
//  Three raw Win32 IME/codepage imports, and the one local caller among
//  them.  IDA labels 0x6F6C66C0/0x6F7E251A/0x6F7E2520 with the real API's
//  own name because each is a single `jmp ds:__imp_<Name>` compiler-
//  generated import trampoline inside Game.dll itself (the local stub every
//  other call to that import goes through, not the import in
//  Kernel32.dll/Imm32.dll proper) - there is no separate function body to
//  reconstruct beyond that one instruction.  Spelled `jmp DWORD PTR
//  [<Name>]` per CLAUDE.md's house rule for calling a Win32 API from inside
//  an `__asm` block (`call SomeWin32Api` bare would execute the import
//  table slot's four bytes as code; MASM resolves the bracketed form to the
//  dllimport symbol automatically), and given a distinct C++ name from the
//  real import so the declaration from <windows.h>/imm32-via-user32 is not
//  redefined.
//============================================================================
#include <windows.h>
#include <imm.h>
#include "wininputbridge.h"

//  0x6F6C66C0 - `retn 0`, no arguments (GetACP takes none).
__declspec(naked) unsigned int __stdcall GetACP_ImportStub()
{
    __asm { jmp DWORD PTR [GetACP] }
}

//  0x6F7E251A - `retn 8`, two stack arguments (stdcall, no fastcall-able
//  registers reach a raw Win32 import).
__declspec(naked) HIMC __stdcall
ImmAssociateContext_ImportStub(HWND, HIMC)
{
    __asm { jmp DWORD PTR [ImmAssociateContext] }
}

//  0x6F7E2520 - `retn 0Ch`, three stack arguments.
__declspec(naked) BOOL __stdcall
ImmAssociateContextEx_ImportStub(HWND, HIMC, DWORD)
{
    __asm { jmp DWORD PTR [ImmAssociateContextEx] }
}

//  0x6F6C6A80 - the one caller in this dump: disassociate the IME context
//  from the currently selected window (GetSelectedWindow(0), the same
//  "which window" helper wininputbridge.h's own callers use) and clear a
//  module-local flag afterwards.  IACE_DEFAULT (0x10) restores the
//  window's default IME context instead of clearing it to NULL outright -
//  the shipped code passes a literal `0` HIMC with that flag, not
//  ImmAssociateContext's simpler two-argument form.
extern unsigned int g_imeAssociationFlag;   // dword_6FAD1470

void __fastcall DisassociateWindowIme()
{
    ImmAssociateContextEx_ImportStub(GetSelectedWindow(0), 0, 0x10);
    g_imeAssociationFlag = 0;
}
