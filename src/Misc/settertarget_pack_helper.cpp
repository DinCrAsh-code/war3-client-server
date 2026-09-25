//============================================================================
//  0x6F6FDE70 - see settertarget_pack.cpp for the five 0x6F6FExxx callers.
//  Kept in its own translation unit deliberately: compiled alongside its
//  callers, /Ob2 inlines this small a function into every one of the five
//  call sites (50 generated instructions per wrapper instead of the
//  shipped 13) where the dump shows a real `call sub_6F6FDE70` at each -
//  the shipped build kept it out-of-line, so this file has to stay
//  separate to reproduce that rather than reaching for
//  `__declspec(noinline)`, which the shipping compiler had no equivalent
//  of.
//============================================================================
#include "settertarget.h"

//----------------------------------------------------------------------------
//  0x6F6FDE70 - id1/id2/id3 are always queried; id0OrSentinel is queried
//  too unless it is -1, in which case its byte defaults to 0xFF instead.
//  Packed as (id0<<24)|(id1<<16)|(id2<<8)|id3 - id0 in the high byte
//  despite being the *last* of the four queried, id1 first.
//----------------------------------------------------------------------------
void ISetterTarget::QueryFourFieldsPacked(int* out, int value, int id1,
                                          int id2, int id3,
                                          int id0OrSentinel)
{
    int result1 = SetFieldA(value, id1);
    int result2 = SetFieldA(value, id2);
    int result3 = SetFieldA(value, id3);
    int result0 = (id0OrSentinel == -1) ? 0xFF : SetFieldA(value, id0OrSentinel);

    *out = ((result0 & 0xFF) << 24) | ((result1 & 0xFF) << 16) |
           ((result2 & 0xFF) << 8) | (result3 & 0xFF);
}

