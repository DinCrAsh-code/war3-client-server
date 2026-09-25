//============================================================================
//  0x6F4A9700 - a `this`-implicit, `ecx`-only, `retn 0` leaf returning
//  `this`'s own +0x40 (CAgentRelation::m_endpointB) - the same
//  one-instruction shape already reconstructed at three other addresses
//  in this codebase (Agent/cpofglis.h's GetEndpointBRaw,
//  Agent/cpofgpulsemod_getendpointbraw.cpp,
//  Misc/trivialaccessors_04.cpp's own GetField0x40_6F4A95A0 -
//  docs/msvc-vc8-idioms.md's own "Four identical functions at four
//  addresses" idiom, a fourth instance). Own translation unit, kept out
//  of line to match the real dump.
//============================================================================
#include "cporepulsemod.h"

int __fastcall GetField0x40_6F4A9700(const void* self)
{
    return *(const int*)((const char*)self + 0x40);
}
