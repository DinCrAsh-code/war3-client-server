//============================================================================
//  A real, linkable `sqrt(double)`.
//
//  FVec3::Normalize (src/fvec3normalize.cpp) declares `sqrt` with `#pragma
//  intrinsic` so tools/build.sh's /O2 listing shows the shipped code's own
//  `call _CIsqrt` - and it does, because that build's flags make the
//  compiler substitute the intrinsic at the call site, so this definition
//  is never actually reached there and never appears in that .asm output.
//  matching_decomp_injection's own build of the same source compiles
//  fvec3normalize.cpp under different flags that do not trigger the
//  substitution, leaving a genuine unresolved external `sqrt` that fails
//  the whole DLL's link - not just this one function's hook. This is that
//  symbol's real definition, so the link succeeds regardless of which
//  build compiled the call site. A plain Newton's-method iteration is
//  enough: nothing in this call tree depends on this fallback's own
//  rounding, only on the intrinsic-substituted call matching, which this
//  file does not change.
//============================================================================

double sqrt(double x)
{
    if (x <= 0.0)
        return 0.0;

    double guess = x;
    for (int i = 0; i < 40; i++)
        guess = 0.5 * (guess + x / guess);
    return guess;
}
