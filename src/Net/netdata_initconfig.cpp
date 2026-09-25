//============================================================================
//  InitializeNetSessionConfig (0x6F553920) - bring up the "net" checksum-
//  provider slot and its three Battle.net-timer event handlers, then read
//  the user's stored locale out of the Windows profile ("HPROFILE") into
//  the freshly-built `SNetSessionInfo`.  Zero arguments, `retn 0` - the one
//  caller this session found (docs/notes/checksum-provider-registry.md)
//  never sets up `ecx`/`edx` before the call.  TeardownNetSession
//  (netdata_teardown.cpp) is this function's own teardown counterpart -
//  same three handlers, same slot index, reverse order.
//
//  The `0x656E5553` locale default is this repo's own `kNetSessionTagNone`/
//  `kNetSessionTagBattleNet` (netdata.h) spelling convention: read the hex
//  literal's bytes MSB-to-LSB as characters and it spells `enUS` - the
//  FourCC-shaped default the shipped code falls back to when "LANGID"
//  isn't found (or the profile file/section itself doesn't exist).
//============================================================================
#include "netdata.h"
#include "gamecontext.h"
#include "config.h"        // CConfigFile
#include "refcnt.h"         // TRefCnt

int __fastcall OsNetInitializeAlias(int, unsigned int);
void __fastcall RegisterEventHandler(int slot, void* handler, int a, float when);
void* __fastcall AcquireNetData(int index, int /*unused*/);

//  0x6F5538F0 / 0x6F54CC80 / 0x6F54CCE0 - Net/pausecallchainthunks.cpp and
//  netdata_tickratehandlers.cpp.
void CNetData_PumpFromTls();
void __fastcall NetTickRateBump_6F54CC80(void* self);
void __fastcall NetTickRateDrop_6F54CCE0(void* self);

//  0x6F4D18D0 / 0x6F4D26C0 - Config/hprofile.cpp.  Both reach deep into a
//  Windows-profile/registry subsystem (`ProfileInternal::PROFILE`) this
//  batch does not otherwise touch; see that file's own header for why their
//  own callees are naked redirects rather than full reconstructions.
void* __fastcall AcquireHProfile();
int __fastcall ConfigProfileHasSection_6F4D26C0(void* self, const char* name,
                                                int /*unused*/);

void __fastcall InitializeNetSessionConfig()
{
    OsNetInitializeAlias(0, 2);

    AcquireNetData(2, 0);

    RegisterEventHandler(5, (void*)CNetData_PumpFromTls, 0, 1.0f);
    RegisterEventHandler(0x18, (void*)NetTickRateBump_6F54CC80, 0, 1.0f);
    RegisterEventHandler(0x19, (void*)NetTickRateDrop_6F54CCE0, 0, 0.0f);

    unsigned int locale = 0x656E5553u;   // 'enUS' default

    void* hprofile = AcquireHProfile();
    if (hprofile != 0)
    {
        if (ConfigProfileHasSection_6F4D26C0(hprofile, "config.txt", 0) != 0)
            ((CConfigFile*)hprofile)->GetIntValue("Battle.net", "LANGID",
                                                  (int*)&locale, 0);

        ((TRefCnt*)hprofile)->Release();
    }

    CNetData* records = NetDataRecords();
    if (records != 0)
        *(unsigned int*)((char*)records + 0x22C4) = locale;
}
