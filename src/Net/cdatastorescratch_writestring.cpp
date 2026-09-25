//============================================================================
//  0x6F4C2BA0 - append a NUL-terminated string to the scratch buffer, its
//  terminator included (the length is `SStrLen(src) + 1`).
//
//  A null string is not an assertion: it sets Storm's last error to
//  ERROR_INVALID_PARAMETER and returns `this` unchanged, which is the same
//  refusal shape the rest of this family has.
//
//  Own translation unit for the reason every other member of this class has
//  one: /Ob2 would fold WriteRaw into it given the chance, and the image
//  makes a real call.
//============================================================================
#include "cdatastorescratch.h"
#include "os.h"     // Storm_506, SErrSetLastError

CDataStoreScratch* CDataStoreScratch::WriteString(const char* src)
{
    if (src == 0)
    {
        SErrSetLastError(0x57);
        return this;
    }

    WriteRaw(src, Storm_506(src) + 1);
    return this;
}
