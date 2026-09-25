//============================================================================
//  CFloat - the deterministic ("software") float used by the simulation.
//
//  The storage is a plain IEEE-754 single precision bit pattern, but every
//  arithmetic operation is carried out with integer instructions so that the
//  result is bit-identical on every machine regardless of the x87 control word,
//  SSE rounding mode or 80-bit intermediate precision.  Only comparisons ever
//  touch the FPU, and those are exact for the bit patterns this type produces.
//
//  Layout of the encoded value (identical to `float`):
//
//      31    30 .. 23    22 .. 0
//      sign  exponent    mantissa
//
//  An exponent field of zero means "zero" - denormals are not represented, and
//  every operation that would produce one flushes the result to zero instead.
//
//  Target addresses:
//      operator+          0x6F6EF000
//      operator-          0x6F6EEEF0
//      operator*          0x6F6EEE20
//      CFloatFromInt      0x6F6EF350
//      CFloatToInt        0x6F6EEC20
//      CFloatReciprocal   0x6F6EF100
//      CountLeadingZeros  0x6F6EEC00
//      operator/          0x6F6EFE10
//      CFloatPow          0x6F6F00E0
//============================================================================
#ifndef CFLOAT_H
#define CFLOAT_H

//: Tag for the constructor that leaves the value alone - see CFloat below.
enum ECFloatNoInit { kCFloatNoInit };

class CFloat
{
public:
    CFloat() : m_bits(0) {}

    //  A genuinely uninitialised CFloat, for a local the shipped code passes
    //  to an out-writer by address without storing anything into it first
    //  (CWidget::InitSprite's facing scratch, MakeAxisRotation's sin/cos
    //  pair).  The zeroing default constructor costs one dead instruction at
    //  every such site.
    //
    //  Spelled as a tagged constructor rather than by making the default one
    //  trivial: that was measured, and while it does fix those sites, it
    //  takes five instructions each off six *finished* targets whose own
    //  locals rely on the zeroing - a net loss, and a regression the report
    //  flags.  This way nothing that does not ask for it changes.
    CFloat(ECFloatNoInit) {}

    CFloat(const CFloat& other) : m_bits(other.m_bits) {}

    // Wraps an already encoded bit pattern.  This is *not* a conversion from
    // an integer value - use CFloatFromInt() for that.
    explicit CFloat(int bits) : m_bits(bits) {}

    //  0x6F00C110 / 0x6F01FD40 - the two bare comparisons, out of line at
    //  their own addresses (floatcompare.cpp).  Members rather than free
    //  functions because every shipped call site loads ecx with the
    //  left-hand operand and pushes only the right-hand one; taking the
    //  right by pointer rather than by reference for the same reason the
    //  rest of this module does - the shipped code pushes the address.
    int IsLess(const CFloat* other) const;
    int IsGreater(const CFloat* other) const;
    //  0x6F01FD80 - `*this >= other`, the third bare comparator right after
    //  the two above (floatcompare.cpp).
    int IsGreaterOrEqual(const CFloat* other) const;

    int m_bits;
};

// Field layout of the encoding.
const int kCFloatSignMask = 0x80000000;
const int kCFloatExpMask  = 0x7F800000;
const int kCFloatMantMask = 0x007FFFFF;
const int kCFloatExpOne   = 0x00800000;   // one step of the exponent field
const int kCFloatExpBias  = 0x3F800000;   // encoding of 1.0

// Encoded constants owned by the simulation module.
extern const CFloat g_CFloatZero;       // 0x6FAAE470
extern const float  g_CFloatEpsilon;    // 0x6FAAE478
extern const CFloat g_CFloatHalf;       // 0x6FAAE4D0

int __fastcall CountLeadingZeros(unsigned int value);

CFloat __fastcall CFloatFromInt(int value);
int __fastcall CFloatToInt(const CFloat& value);

CFloat __fastcall CFloatReciprocal(const CFloat& value);

//  0x6F6EF800 - 1/sqrt(x), as one operation: CFloatReciprocal(CFloatSqrt(x)).
//  See Math/cfloatinvsqrt.cpp.
CFloat __fastcall CFloatInvSqrt(const CFloat& value);

CFloat __fastcall operator+(const CFloat& a, const CFloat& b);
CFloat __fastcall operator-(const CFloat& a, const CFloat& b);
CFloat __fastcall operator*(const CFloat& a, const CFloat& b);
CFloat __fastcall operator/(const CFloat& a, const CFloat& b);

CFloat __fastcall CFloatPow(const CFloat& base, int exponent);

//  0x6F6EFB00 - arc cosine, two piecewise-linear tables.  See
//  softfloat_acos.cpp.
CFloat __fastcall CFloatAcos(const CFloat& value);

#endif
