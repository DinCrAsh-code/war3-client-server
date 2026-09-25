//============================================================================
//  sub_6F65C7D0 (0x6F65C7D0) - clear each node's own "posted" high bit
//  under `this`'s own +0x5F0 lock.  Reached both from vtable slot 6
//  (netprovider_slot6.cpp, ResetAndNotify) and from the base destructor's
//  own dump (0x6F65DB60's call tree).  Its own translation unit, same rule
//  as netprovider_freepool_helpers.cpp: with the definition visible in the
//  same TU as either caller this build's /Ob2 inlines the whole body on
//  the spot instead of emitting the real `call`.
//============================================================================

void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);

class NetProviderSlot6Self
{
public:
    //  0x6F65C7D0 - `this` in ecx, no stack arguments, tail-calls
    //  LeaveCritSec.
    void ClearPostedBits();
};

void NetProviderSlot6Self::ClearPostedBits()
{
    char* self = (char*)this;
    EnterCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x5F0));

    int inUse = *(int*)(self + 0x628);
    *(int*)(self + 0x628) = inUse + 1;
    if (inUse == 0)
    {
        int raw = *(int*)(self + 0x610);
        int node = (raw > 0) ? raw : 0;
        if (node > 0)
        {
            do
            {
                int flags = *(int*)(node + 0x10);
                if (flags >= 0)
                {
                    flags = (flags & 0x1FFFFFFF) | 0x80000000;
                    *(int*)(node + 0x10) = flags;
                    *(int*)(self + 0x614) -= 1;
                }
                node = *(int*)(node + 4);
            } while (node > 0);
        }
    }

    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x5F0));
}
