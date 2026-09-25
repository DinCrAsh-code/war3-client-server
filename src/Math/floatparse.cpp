//============================================================================
//  0x6F6F0520 - decimal text into the deterministic float.
//
//  A hand-written atof: an optional sign, then digits with at most one decimal
//  point, stopping at a space, a semicolon (comment), a tab or anything else
//  that is not a digit or the point.  At most nine significant digits go into
//  an integer mantissa; every digit after that only moves the decimal
//  exponent, so a long number loses precision but not magnitude.  The result
//  is assembled as mantissa * 10^-exponent using the software float's own
//  power routine, so it is bit-identical everywhere.
//
//  Its own translation unit rather than part of src/softfloat.cpp: it calls
//  CFloatFromInt, which has no loop and would be inlined if its definition
//  were visible here, and the shipped code calls it.
//============================================================================
#include <ctype.h>

#include "CFloat.h"
#include "config.h"

CFloat __fastcall CFloatFromString(const char* text)
{
    //  The base of the decimal scaling, as an encoded constant rather than a
    //  conversion: 0x41200000 is 10.0f.
    CFloat ten(0x41200000);

    int sign = 1;
    if (*text == '-')
    {
        sign = -1;
        ++text;
    }
    else if (*text == '+')
    {
        ++text;
    }

    int mantissa = 0;       // the significant digits, as an integer
    int digits = 0;         // how many significant digits have been taken
    int exponent = 0;       // decimal places the mantissa has to come back by
    int fracstep = 0;       // what each further digit adds to `exponent`
    int seenPoint = 0;
    int started = 0;        // a nonzero digit has been seen: leading zeros are
                            // not significant digits

    char c = *text++;
    while (c != 0)
    {
        if (c == ' ' || c == ';' || c == '\t')
            break;

        if (isdigit(c))
        {
            if (started || c != '0')
                started = 1;

            digits += started;
            if (digits <= 9)
            {
                //  Room left in the mantissa: take the digit, and only count a
                //  decimal place if the point has already been seen.
                exponent += fracstep;
                mantissa = mantissa * 10 + (c - '0');
            }
            else
            {
                //  Out of room.  The first dropped digit flips `fracstep` down
                //  by one - from 0 to -1 before the point, from 1 to 0 after
                //  it - so further digits scale the mantissa up instead of
                //  being added to it.
                if (digits == 10)
                    --fracstep;
                exponent += fracstep;
            }
        }
        else if (c == '.' && seenPoint == 0)
        {
            fracstep += 1;
            seenPoint = 1;
        }
        else
        {
            break;
        }

        c = *text++;
    }

    CFloat value = CFloatFromInt(mantissa * sign);
    //  Spelled with the division first because that is the shipped code's
    //  fall-through path; writing the negative-exponent case first swaps the
    //  two blocks.
    if (exponent >= 0)
        value = value / CFloatPow(ten, exponent);
    else
        value = value * CFloatPow(ten, -exponent);
    return value;
}
