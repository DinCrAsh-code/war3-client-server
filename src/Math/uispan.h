//============================================================================
//  0x6F4C6480 / 0x6F4C64E0 - Math/uispan.cpp.  Move a pair of coordinates
//  between the game's normalised [0,1] screen space and its UI coordinate
//  space, whose extents are 0.8 across and 0.6 down.  Either output pointer
//  may be null.
//============================================================================
#ifndef UISPAN_H
#define UISPAN_H

//  0x6F4C6480 - normalised -> UI units (multiply).
void __fastcall ScaleToUiSpan(float* outX, float* outY, float x, float y);
//  0x6F4C64E0 - UI units -> normalised (divide).
void __fastcall ScaleFromUiSpan(float* outX, float* outY, float x, float y);

#endif
