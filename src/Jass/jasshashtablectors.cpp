//============================================================================
//  See jasshashtablectors.h for why these are redirects and not
//  reconstructions.  Spelled as `Construct` members rather than as C++
//  constructors for the reason GameUI/selectmodector.cpp and
//  Containers/hashtablectors.cpp both already record: MSVC will not take
//  __declspec(naked) on a constructor.  Each hands `this` back, which is
//  what the shipped `mov eax, esi` before the `retn` does.
//============================================================================
#include "jasshashtablectors.h"

#define JASS_HASHTABLE_CTOR_THUNK(addr) \
    {                                   \
        __asm { mov eax, addr }        \
        __asm { jmp eax }               \
    }

//  0x6F44F990
__declspec(naked) SJassNativeFuncTable* SJassNativeFuncTable::Construct()
    JASS_HASHTABLE_CTOR_THUNK(06F44F990h)

//  0x6F4528D0
__declspec(naked) SJassScript2HandleTable* SJassScript2HandleTable::Construct()
    JASS_HASHTABLE_CTOR_THUNK(06F4528D0h)

//  0x6F450B80
__declspec(naked) SJassHandleTable* SJassHandleTable::Construct()
    JASS_HASHTABLE_CTOR_THUNK(06F450B80h)

//  0x6F454050 - vtable slot 2, `retn 4`.  See the header's own note: kept a
//  naked thunk because its real destructor (0x6F450DA0) tail-jumps into
//  __CxxFrameHandler3.
__declspec(naked) void* SJassHandleTable::ScalarDeletingDestructor(unsigned int)
    JASS_HASHTABLE_CTOR_THUNK(06F454050h)
