#include "playerwar3_releasehelper.h"

typedef void (__thiscall *Slot2Fn)(void*);
typedef void (__thiscall *Slot0Fn)(void*);

void __fastcall CPlayerWar3_ReleaseAndClearSlot(void** field)
{
    void* obj = *field;
    if (obj)
    {
        ((Slot2Fn)(*(void***)obj)[2])(obj);
        obj = *field;
        if (obj && --*(int*)((char*)obj + 4) == 0)
            ((Slot0Fn)(*(void***)obj)[0])(obj);
        *field = 0;
    }
}
