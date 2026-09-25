//============================================================================
//  0x6F4729C0 - QueryAgileTypeObject: the per-type object a registered agile
//  type keeps at +0x6C, built on first use.
//
//  Its second (edx) argument is never read - the one caller in this tree
//  loads 1 into edx immediately before the call, so the parameter is
//  declared to keep the register assignment honest without pretending the
//  body uses it.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "agiletype.h"
#include "game.h"

//  0x6F471F40 - builds whatever goes in the slot.  No arguments;
//  reconstructed in agiletypeobjectbuild.cpp, declared here so this call
//  site reaches it with a real call.
void* BuildAgileTypeObject();

void* __fastcall QueryAgileTypeObject(unsigned int typeId, int /*unused*/)
{
    //  The singleton read into a local before Hash() runs and held across
    //  it, exactly as in CAgent::ReleaseSelf (agent_releaseself.cpp) - the
    //  same `mov esi, dword_6FAB7368` / `lea ecx,[esi+0Ch]` shape.
    CGameData* game = g_pGameData;

    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    //  Written as "start at null and overwrite" rather than as two returns:
    //  the shipped code zeroes eax before the null test and falls through
    //  to one shared exit.
    void* built = 0;
    if (type != 0)
    {
        built = type->m_field6C;
        if (built == 0)
        {
            built = BuildAgileTypeObject();
            type->m_field6C = built;
        }
    }
    return built;
}
