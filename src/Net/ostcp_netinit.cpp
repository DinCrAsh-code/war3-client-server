//============================================================================
//  0x6F6E6E30 - OsNetInitialize.  Bring up the subsystems named in `mask`,
//  each behind its own reference count so a second call is a no-op, and undo
//  everything this call started if any one of them fails.
//
//  Two shapes here are the shipped code's and not tidiable:
//
//  * `mask` is a *local* whose address is taken (OsNetExpandMask closes it
//    over its dependencies), so MSVC re-reads its low byte from the stack
//    after every call rather than keeping it in a register.  Written any
//    other way the six bit tests read a register and the stream diverges.
//  * `started` accumulates the bits this call is responsible for, and the
//    or happens *outside* each refcount test: a subsystem somebody else
//    already started still has to be handed back to OsNetShutdown on the
//    failure path, because the shutdown is what decrements the count this
//    call incremented.
//============================================================================
#include "ostcp.h"

//: dbl_6F875AA0 - 1000.0, the same one Config/preferenceapply.cpp already
//: names.  Written as the global rather than a literal so the divisor
//: canonicalises to the same symbol on both sides.  *Outside* the namespace,
//: because that is where preferenceapply.cpp declares it and MSVC puts the
//: namespace in the mangled name - one symbol, not two.
extern const double g_timeScaleUnitsPerSecond;

namespace OsNet
{

static const char kW32OsTcpCpp[] = ".\\W32\\OsTcp.cpp";

int __fastcall OsNetInitialize(int arg, unsigned int mask)
{
    OsNetExpandMask(&mask);
    LazyInitAndEnterDebugLock(&g_osNetLock);

    unsigned int started = 0;
    int ok = 1;

    if (mask & 8)
    {
        int was = g_osNetRefCore;
        g_osNetRefCore = was + 1;
        started |= 8;
        if (was == 0)
        {
            LARGE_INTEGER frequency;

            g_osNetStormHandle = 0;
            g_osTcpUseQpc = QueryPerformanceFrequency(&frequency);
            g_qpcMsPerCount = g_osTcpUseQpc
                                  ? (float)(g_timeScaleUnitsPerSecond /
                                            (double)frequency.QuadPart)
                                  : 0.0f;

            OSVERSIONINFOA version;
            version.dwOSVersionInfoSize = sizeof(version);
            if (GetVersionExA(&version))
            {
                g_osTcpLegacyOs = 1;
                if (version.dwMajorVersion >= 5)
                    g_osTcpLegacyOs = 0;
            }
            else
            {
                g_osTcpLegacyOs = 0;
            }

            void* block = SMemAlloc(0x634, kW32OsTcpCpp, 0x74F, 0);
            g_pTcpManager = block ? ((TCPMGR*)block)->Construct() : 0;

            if (!g_pTcpManager->StartListenThread(arg))
                goto fail;
        }
    }

    if (mask & 1)
    {
        int was = g_osNetRefTimer;
        g_osNetRefTimer = was + 1;
        started |= 1;
        if (was == 0 && !g_pTcpManager->StartTimerThread(arg))
            goto fail;
    }

    if (mask & 0x10)
    {
        int was = g_osNetRefWinsock;
        g_osNetRefWinsock = was + 1;
        started |= 0x10;
        if (was == 0 && !g_pTcpManager->StartWinsock(arg))
            goto fail;
    }

    if (mask & 0x20)
    {
        int was = g_osNetRefIocp;
        g_osNetRefIocp = was + 1;
        started |= 0x20;
        if (was == 0 && !g_pTcpManager->StartIocp(arg))
            goto fail;
    }

    if (mask & 4)
    {
        started |= 4;
        g_osNetRefMisc = g_osNetRefMisc + 1;
    }

    if (mask & 2)
    {
        int was = g_osNetRefSelect;
        g_osNetRefSelect = was + 1;
        started |= 2;
        if (was == 0 && !g_pTcpManager->StartSelectThreads(arg))
            goto fail;
    }

    goto done;

fail:
    ok = 0;
    OsNetShutdown(started);

done:
    LeaveCriticalSection(g_pOsNetLock);
    return ok;
}

}  // namespace OsNet
