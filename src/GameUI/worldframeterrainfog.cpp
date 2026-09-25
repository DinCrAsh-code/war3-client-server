//============================================================================
//  0x6F38BE80 - CWorldFrameWar3::ApplyTerrainFogConfig: push the whole
//  [TerrainZFog] configuration section into the world frame's fog object and
//  hand that object to the frame.
//
//  The five keys are the shipped strings - "Style", "Color", "Density",
//  "Start" and "End" - all read from the section "TerrainZFog", which is
//  what names this function.  Four of them land in the fog object's numbered
//  value slots (1 = style, 2 = density, 3 = start, 4 = end) and the colour
//  goes into channel 0 through the same setter every sprite colour uses.
//
//  Own translation unit: every call in it is real.
//============================================================================
#include "gameui.h"
#include "frame.h"
#include "controllerslot.h"
#include "gameuithunks.h"

//  0x6F009F90 - Config/configint.cpp.
int   __fastcall GetConfigInt(const char* section, const char* key, int index);
//  0x6F009E30 - the hardware-float config read, declared the same way
//  GameUI/cameraresetdefaults.cpp declares it.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);
//  0x6F00A8B0 - Config/configcolor.cpp.
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);

void CWorldFrameWar3::ApplyTerrainFogConfig()
{
    SetControllerSlotInt(m_pFog, 1, GetConfigInt("TerrainZFog", "Style", 0));

    {
        unsigned int color;
        GetConfigColor(&color, "TerrainZFog", "Color", 0);
        SpriteSetChannelColor(m_pFog, 0, &color);
    }

    //  Each of the three floats goes through a named local rather than
    //  straight into the call: the shipped stream stores the config read to
    //  a stack slot and loads it straight back before pushing it
    //  (`fstp [esp+4]` / `fld [esp+4]`), which is the round-trip an
    //  assignment to a `float` variable forces and a direct argument does
    //  not.  It is the same slot the colour above used - MSVC overlays the
    //  two, whose lifetimes do not meet.
    {
    float value;

    value = GetConfigFloatHW("TerrainZFog", "Density", 0);
    SetControllerSlotFloat(m_pFog, 2, value);
    value = GetConfigFloatHW("TerrainZFog", "Start", 0);
    SetControllerSlotFloat(m_pFog, 3, value);
    value = GetConfigFloatHW("TerrainZFog", "End", 0);
    SetControllerSlotFloat(m_pFog, 4, value);
    }

    //  A CFrame method, reached by upcast: gameui.h derives this class
    //  from CObserver rather than from CFrame (see the note there), but
    //  the record really is a CFrame and +0x148 is CFrame's own fog list.
    ((CFrame*)this)->AddFog(m_pFog, 7);
}
