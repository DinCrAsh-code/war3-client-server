//============================================================================
//  0x6F62BAA0 - release and reset a cached timer-owner reference.  `this`'s
//  own +0x14 is a raw pointer this batch cannot name the class of (nothing
//  in its own 35-address closure reaches a caller of this function to say
//  what owns it), but the one thing it *does* call - Net/netproviderbnet_timer.cpp's
//  own `NetProviderBnetUnregisterTimer::Unregister()` (0x6F62B710, `this`
//  in ecx, `retn 0`) - is already reconstructed, and this function calls it
//  exactly the way `NetProviderBNET::Method_5` already does: cast the
//  pointer through that placeholder class and call `Unregister()` on it.
//  Struct offsets are exact; only the five touched fields are named.
//============================================================================
class NetProviderBnetUnregisterTimer
{
public:
    void Unregister();   // 0x6F62B710 - Net/netproviderbnet_timer.cpp
};

struct SCachedTimerOwnerRef
{
    char  m_reserved00[0x8];
    int   m_8;                              // +0x08
    int   m_C;                              // +0x0C
    float m_10;                             // +0x10
    NetProviderBnetUnregisterTimer* m_pOwner;  // +0x14
    int   m_18;                             // +0x18

    void ReleaseAndReset();
};

//  0x6F62BAA0 - `retn 0`.
void SCachedTimerOwnerRef::ReleaseAndReset()
{
    if (m_pOwner != 0)
    {
        m_pOwner->Unregister();
        m_10 = 0.0f;
        m_pOwner = 0;
        m_18 = -1;
        m_C = 0;
        m_8 = 0;
    }
}
