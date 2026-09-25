//============================================================================
//  0x6F302A70 / 0x6F302A30 - `SetCameraPreferenceFractionSecondary`/
//  `SetCameraPreferenceFractionPrimary`, already declared (as thunked
//  externs) in Config/preferenceapply.cpp and, until this session, thunked
//  in Misc/spritehandlethunks.cpp - real bodies now, here.  Each is called
//  once by CCameraWar3::CCameraWar3() (cameraconstructor.cpp) too, for the
//  same preference ids ApplyPreference's own cases 41/39 push through them
//  live.
//
//  0x6F3029F0 / 0x6F3029B0 - their own two callees, genuinely new (no prior
//  name anywhere).  See cameraweightclamp.cpp for the full derivation.
//============================================================================
#ifndef CAMERAWEIGHTCLAMP_H
#define CAMERAWEIGHTCLAMP_H

class CCameraWar3;

//  0x6F302A70 - retn 4.  Config/preferenceapply.cpp's own forward
//  declaration (case 41) is the authority on this signature; declared again
//  here only so cameraconstructor.cpp's own two call sites do not need to
//  include preferenceapply.cpp's private declaration block.
void __fastcall SetCameraPreferenceFractionSecondary(CCameraWar3* camera,
                                                      float value);
//  0x6F302A30 - retn 4 (case 39).
void __fastcall SetCameraPreferenceFractionPrimary(CCameraWar3* camera,
                                                    float value);

#endif
