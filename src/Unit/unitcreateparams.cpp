//============================================================================
//  0x6F267500 - the parameter block the unit factory (0x6F29F990,
//  jasscreateunit.h) builds on its own frame before it creates anything:
//
//      6F29F9D7    lea     ecx, [esp+200h+var_1AC]
//      6F29F9DF    call    sub_6F267500
//
//  It is a constructor, not an init function - `this` in ecx, no stack
//  arguments, and `mov eax, esi` / `retn` handing `this` back, which is
//  MSVC's own constructor convention and not something a `void` initialiser
//  produces.
//
//  **Why every global is re-read.**  The body loads dword_6FAB4354 four
//  separate times and dword_6F92ED98 three, alternating, instead of keeping
//  either in a register.  That is not a missed CSE: `this` is an
//  unconstrained pointer, so the compiler cannot prove that the store it
//  just made through it did not land on the global it is about to read
//  again, and it must reload.  Written as plain member assignments this
//  build reproduces it on its own - hoisting either global into a local
//  would be the thing that broke it.
//
//  **m_int24 is written twice on purpose.**  Zero in the initialiser list,
//  then the real value in the body after the sub-object's own constructor
//  has run.  Both stores are in the dump (6F26751C and 6F26752C) with the
//  call between them, so the second is an assignment in the constructor
//  body and not a compiler artefact - fold them into one and the first
//  store, and the `xor eax, eax` that feeds it, disappear.
//============================================================================
#include "unitcreateparams.h"

SUnitCreateParams::SUnitCreateParams()
    : m_int14(0),
      //  `fild` / `fstp`, not a `mov`: this one member takes the sentinel
      //  *converted* to float, where +0x48/+0x4C/+0x54 below copy the same
      //  global's raw dword.  widgetsprite.cpp already establishes it as an
      //  integer sentinel, which is what makes the conversion visible here.
      m_float1C((float)g_animationSpeedSentinel),
      m_int20(1),
      m_int24(0),
      m_int28(0),
      m_sub2C(0)
{
    m_int24 = g_unk6F931ECC;

    m_int38 = g_unk6FAB4354;
    m_int3C = g_unk6FAB4354;
    m_int40 = g_unk6FAB4354;
    m_int44 = g_unk6FAB4354;
    m_int48 = g_animationSpeedSentinel;
    m_int4C = g_animationSpeedSentinel;
    m_int50 = g_unk6FAB4354;
    m_int54 = g_animationSpeedSentinel;
}
