//============================================================================
//  A real, linkable `fabs(double)`.
//
//  Same reason as sqrtfallback.cpp: CCameraWar3::PanTo
//  (src/GameUI/camerapan.cpp) declares `fabs` with C linkage and
//  `#pragma intrinsic` so tools/build.sh's /O2 listing shows the shipped
//  code's own bare `fabs` x87 instruction rather than a call - and it does.
//  The injection repo's build uses different flags; a build without /Oi
//  leaves a genuine unresolved external `_fabs` that fails the whole DLL's
//  link, not just this one function's hook.
//
//  `#pragma function(fabs)` is what makes this definition legal at all:
//  without it the compiler refuses (C2169, "intrinsic function, cannot be
//  defined").  Nothing this repo scores ever reaches this body.
//============================================================================
extern "C" double __cdecl fabs(double x);

#pragma function(fabs)

extern "C" double __cdecl fabs(double x)
{
    return x < 0.0 ? -x : x;
}
