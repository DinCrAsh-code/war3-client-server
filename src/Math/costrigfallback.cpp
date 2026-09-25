//============================================================================
//  Real, linkable `cos(double)`/`sin(double)`.
//
//  Same reason as sqrtfallback.cpp: itemorientation.cpp declares `cos`/`sin`
//  with `#pragma intrinsic` so tools/build.sh's /O2 listing shows the
//  shipped code's own `call _CIcos`/`call _CIsin` - and it does, because
//  that build's flags make the compiler substitute the intrinsic at the
//  call site, so these definitions are never actually reached there and
//  never appear in that .asm output. matching_decomp_injection's own build
//  of the same source compiles itemorientation.cpp under different flags
//  that do not trigger the substitution, leaving genuine unresolved
//  externals `cos`/`sin` that fail the whole DLL's link - not just this one
//  function's hook. These are those symbols' real definitions, so the link
//  succeeds regardless of which build compiled the call site. A textbook
//  Taylor-series reduction is enough: nothing in this call tree depends on
//  this fallback's own rounding, only on the intrinsic-substituted call
//  matching, which this file does not change.
//============================================================================

static const double kPi = 3.14159265358979323846;
static const double kTwoPi = 2.0 * kPi;

//  Range-reduce into (-pi, pi].
static double ReduceAngle(double x)
{
    double turns = x / kTwoPi;
    int wholeTurns = (int)turns;
    x -= wholeTurns * kTwoPi;
    if (x > kPi)
        x -= kTwoPi;
    else if (x < -kPi)
        x += kTwoPi;
    return x;
}

double sin(double x)
{
    x = ReduceAngle(x);
    double term = x;
    double result = x;
    double x2 = x * x;
    for (int n = 1; n <= 12; n++)
    {
        term *= -x2 / ((2 * n) * (2 * n + 1));
        result += term;
    }
    return result;
}

double cos(double x)
{
    x = ReduceAngle(x);
    double term = 1.0;
    double result = 1.0;
    double x2 = x * x;
    for (int n = 1; n <= 12; n++)
    {
        term *= -x2 / ((2 * n - 1) * (2 * n));
        result += term;
    }
    return result;
}
