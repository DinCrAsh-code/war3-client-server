//============================================================================
//  0x6F4663B0 / 0x6F466690 - the lexer's own hex- and decimal-literal
//  readers.  `this` is the lexer/parser state object this batch's closure
//  never gets a full struct for - only the three fields either function
//  touches are named, by raw offset, the same way jassdeclhelpers.cpp does
//  for the declaration object neither dump names.  Both are real members
//  (docs/msvc-vc8-idioms.md's "`__thiscall` on a free function: rejected by
//  this compiler") of a placeholder class, not a recovered one.
//
//    +0x98  the source buffer
//    +0xC4  its length
//    +0x24  where the literal's own decoded value lands (an int for the hex
//           reader, a CFloat's encoded bits for the decimal one)
//
//  ParseHexLiteral cleans 4 bytes of stack (the starting index) and returns
//  a fixed token-kind constant (0x108); ParseRealLiteral's own shipped
//  body cleans none, so it is reconstructed here with no stack argument at
//  all - see its own note on what that leaves unrecovered.
//============================================================================
#include "CFloat.h"

//  0x6F6EF350/0x6F6EF000/0x6F6EFE10 - Math/CFloat.h's own software float.
CFloat __fastcall CFloatFromInt(int value);

class JassLexerState
{
public:
    int   ParseHexLiteral(unsigned int index);   // 0x6F4663B0
    float ParseRealLiteral();                    // 0x6F466690

    char m_reserved00[0x24];
    int  m_literalValue;    // +0x24
    char m_reserved28[0x98 - 0x28];
    const char*  m_source;  // +0x98
    char m_reservedC0[0xC4 - 0x9C];
    unsigned int m_length;  // +0xC4
};

//  A hex digit's value, in the shipped body's own three-way order: lower
//  case first, checked against the cheap `- 'a' <= 5` range before upper
//  case, before falling through to decimal.
static int HexDigitValue(char c)
{
    if ((unsigned char)(c - 'a') <= 5)
        return c - 'W';
    if ((unsigned char)(c - 'A') <= 5)
        return c - '7';
    return c - '0';
}

//  0x6F4663B0 - accumulate a hex literal's digits (the leading `0x`/`$`
//  already consumed by the caller) starting at `index`, most significant
//  digit first.
int JassLexerState::ParseHexLiteral(unsigned int index)
{
    int value = HexDigitValue(m_source[index]);
    index += 1;

    while (index < m_length)
    {
        value = (value << 4) + HexDigitValue(m_source[index]);
        index += 1;
    }

    m_literalValue = value;
    return 0x108;
}

//  0x6F466690 - accumulate a decimal literal's digits into an integer
//  (`base`), the same one digit-at-a-time walk as the hex reader above,
//  stopping at `.` if there is one; a fractional part walks its own digits
//  into a second integer (`frac`) and the two are combined into a CFloat
//  through the software-float library rather than through the FPU, the
//  same determinism CFloat.h's own header note explains.  The shipped body
//  reads its own starting index off `this` rather than a stack argument
//  (`retn` with no operand - unlike ParseHexLiteral above), so wherever
//  that index actually lives on the real object is not established here.
//  Out of this batch's scope: precisely which CFloat operations the
//  shipped body chains together to combine `base` and `frac`
//  (sub_6F6EF350 / sub_6F6EFE10 / sub_6F6EF000) is not reproduced - see
//  the BEHAVIOUR entry.
float JassLexerState::ParseRealLiteral()
{
    unsigned int index = 0;

    int base = 0;
    while (m_source[index] != '.' && m_source[index] != 0)
    {
        base = base * 10 + (m_source[index] - '0');
        index += 1;
    }

    int frac = 0;
    if (m_source[index] == '.')
    {
        index += 1;
        while (index < m_length)
        {
            frac = frac * 10 + (m_source[index] - '0');
            index += 1;
        }
    }

    CFloat result = CFloatFromInt(base);
    *(CFloat*)&m_literalValue = result;

    return 0.0f;   //  the shipped body's own return value is not reproduced
}
