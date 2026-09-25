//============================================================================
//  0x6F0D2650 (createunit-G4 cluster B) - a two-instruction constant getter:
//  no `this`, no arguments, `retn 0`.  Returns the literal 0x42707276, the
//  same multi-char-literal shape unit.h's own slot-7 GetAgileTypeId family
//  and Unit/unit_fixedtypeidgetter.cpp's GetFixedAgileTypeId_58646973 both
//  use for a fixed agile-type-id constant.  Its own address (0x6F0D2xxx)
//  sits far outside its two callers' module (0x6F275xxx/0x6F281xxx,
//  typecheckedrefholder_42707276.cpp), so - unlike
//  GetFixedAgileTypeId_58646973, which its own file keeps merged with its
//  one caller because MSVC folds it into an immediate there - this one is
//  a real out-of-line leaf, most likely shared by callers outside this
//  session's own two-function slice.  Left in its own translation unit so
//  every caller sees a declaration rather than a definition and keeps the
//  shipped code's real `call`, exactly the split unit_fixedtypeidgetter.cpp
//  measured as *worse* for its own single-caller case but is what the
//  address distance says actually happened here.
//============================================================================
unsigned int GetFixedAgileTypeId_42707276()
{
    return 0x42707276;
}
