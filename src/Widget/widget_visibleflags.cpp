//============================================================================
//  0x6F2AD5D0 - CWidget::IsVisibleToLocalPlayer, the predicate CWidget's
//  vtable slot 61 (widget_slot61.cpp) decides sprite visibility with.
//
//  Own translation unit: slot 61 (widget_slot61.cpp) calls this with a real
//  `call`, and this one calls IsGameModeOne (gamemode.cpp) the same way -
//  three files for three functions, because MSVC inlines any two of them
//  that share one.
//
//  The `IsGameModeOne() && world->...` tail is duplicated between this
//  function and slot 61 rather than factored out, which is what the shipped
//  code has: two separate functions with the same three-instruction ending.
//============================================================================
#include "widget.h"
#include "playercolor.h"    // g_unk6FAB65F4 - the world-object singleton

//  0x6F53F160 - see gamemode.cpp.  Its own translation unit precisely so
//  that this function's call to it survives: same-TU, MSVC inlines its
//  five instructions and the shipped `call` disappears.
int __fastcall IsGameModeOne();

//----------------------------------------------------------------------------
//  0x6F2AD5D0 - "does the local player get to see this widget": bit 15 of
//  m_playerMaskB has to be set, and the game either is not in mode 1 or the
//  world object's own +0x34 record does not carry its bit 0.
//
//  The world-object field is reached by raw offset: nothing in these three
//  vtables gives +0x34 or the flag byte at +0x24 inside it a type.
//----------------------------------------------------------------------------
int CWidget::IsVisibleToLocalPlayer()
{
    //  One boolean expression with the `return 1` first: written as a
    //  sequence of early `return 0`s instead, MSVC puts the zero exit at
    //  the top and the shipped order (`mov eax,1` / `retn`, then
    //  `xor eax,eax` / `retn`) inverts.
    if ((m_playerMaskB & 0x8000) != 0
        && (IsGameModeOne() == 0
            || (*((const unsigned char*)
                    *(void**)((char*)g_unk6FAB65F4 + 0x34) + 0x24) & 1) == 0))
        return 1;
    return 0;
}
