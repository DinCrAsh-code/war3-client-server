//============================================================================
//  0x6F443C60 - the agile-type id of the floating-text agents.
//
//  Two instructions: the constant `0x2B746578` in eax and a `retn`.  Read as
//  bytes it is `"xet+"`, i.e. the little-endian form of the `'+tex'` tag the
//  rest of the binary spells the same way (`0x2B61676C` is `'+agl'`), so
//  this is a type id and not an arbitrary magic number.  Its one caller in
//  this tree hands it to the agile-type enumerator.
//
//  Own translation unit: its caller (CGameWorld::SetTextAgentsPaused,
//  gameuiplayerenum.cpp) calls it for real.
//============================================================================

unsigned int GetTextAgentTypeId();

unsigned int GetTextAgentTypeId()
{
    return 0x2B746578;
}
