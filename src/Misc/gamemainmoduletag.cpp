//============================================================================
//  0x6F6C4CA0/0x6F6C58C0/0x6F6C5910 - one small module, reached from
//  GameMain (Game/gamemain.cpp) right after its own security-descriptor
//  setup: a startup timestamp store and an 8-slot [tag, value] registry
//  keyed by a 4-byte packed string ('game' == 0x67616D65 is the one tag
//  this call tree ever uses).  Nothing in this session's tree names the
//  registry or reads it back, so only its write/erase shape is asserted;
//  the names below describe the observed behaviour rather than a recovered
//  Storm API.
//============================================================================
#include <windows.h>

//  Storm ordinal 465 - already declared this shape by other targets
//  (SErrSetLastError).
void __stdcall SErrSetLastError(unsigned int code);

//----------------------------------------------------------------------------
//  0x6F6C4CA0 - GameMain's own first act after SetupWorldSecurityDescriptor:
//  stash GetTickCount()'s result (passed in ecx) into a single global -
//  already declared `void*` and named g_unk6FAD1420 by its own reader,
//  GetGlobal_6F6C4CB0 (0x6F6C4CB0, Misc/trivialaccessors_06.cpp, the very
//  next address after this function).
//----------------------------------------------------------------------------
extern void* g_unk6FAD1420;   // 0x6FAD1420

void __fastcall SetGameMainStartTick(DWORD tick)
{
    g_unk6FAD1420 = (void*)tick;
}

//----------------------------------------------------------------------------
//  0x6F6C58C0 - an 8-slot [tag, value] table, one 8-byte struct per slot
//  (dword_6FAD1430/1434 index with an `eax*8` stride, i.e. two fields of
//  one array, not two parallel arrays).  A null tag or null value fails
//  outright (`if (!tag || !value)`: both `test`s branch to the same
//  SErrSetLastError(0x57) / return-0 tail); otherwise it scans for an
//  existing slot with this tag or the first free one (value == 0) and
//  stores (tag, value) there, or returns 0 once all eight are taken by
//  other tags.
//----------------------------------------------------------------------------
struct ModuleTagSlot
{
    DWORD tag;     // +0
    void* value;   // +4
};
extern ModuleTagSlot g_moduleTagTable[8];   // 0x6FAD1430

int __fastcall RegisterModuleTag(DWORD tag, void* value)
{
    if (tag == 0)
        goto fail;
    if (value == 0)
        goto fail;

    {
        unsigned int slot = 0;
        do
        {
            if (!g_moduleTagTable[slot].value)
                goto store;
            if (g_moduleTagTable[slot].tag == tag)
                goto store;
            ++slot;
        } while (slot < 8);
        return 0;

    store:
        g_moduleTagTable[slot].tag = tag;
        g_moduleTagTable[slot].value = value;
        return 1;
    }

fail:
    SErrSetLastError(0x57);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F6C5910 - the same table's erase: find the slot with this tag and
//  zero both halves.  A tag not present is a silent no-op (falls out of
//  the loop with nothing stored).
//----------------------------------------------------------------------------
void __fastcall UnregisterModuleTag(DWORD tag)
{
    unsigned int slot = 0;
    do
    {
        if (g_moduleTagTable[slot].tag == tag)
        {
            g_moduleTagTable[slot].tag = 0;
            g_moduleTagTable[slot].value = 0;
            return;
        }
        ++slot;
    } while (slot < 8);
}
