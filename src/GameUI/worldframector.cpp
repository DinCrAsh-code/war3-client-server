//============================================================================
//  0x6F39A910 - CWorldFrameWar3::CWorldFrameWar3.
//
//  The frame the 3D world is drawn into, and everything it owns.  Every
//  allocation is tagged ".\\CWorldFrameWar3.cpp", which is what named the
//  class in the first place (GameUI/gameui.h).
//
//  Three shapes in it are worth reading before the body:
//
//    * eight growable arrays are *reserved* rather than sized: the shipped
//      code inlines Storm's own SetCount grow - the chunk lookup with its
//      ComputeChunk fallback, the `count % chunk` round-up and SetAlloc -
//      then writes the chunk it wants and sets the count back to zero.  The
//      `m_count = n` store SetCount ends on is dead in five of the eight and
//      the shipped compiler removed it in four of them; that is where four
//      of this function's misses are, and no spelling of the source chooses
//      it.  The grow is written out here as one file-local template, because
//      TSGrowableArray::SetCount is out of line everywhere else in this repo
//      and a call is not what the shipped stream has;
//    * the four *other* arrays (+0x600, +0x620, +0x630, +0x650) really do
//      call their own out-of-line SetCount - 0x6F334410, 0x6F37CBD0 and
//      0x6F2C7150, all three reconstructed in src/Containers - which is what
//      says their element types are CAgentPtr<CUnit>, CAgentPtr<CSelectable>
//      and CAgentPtr<CGhostImage>;
//    * the eight world-frame slots at +0x378 are stamped -1 by a *downward*
//      loop off a byte offset (0xC0, 0xA8, ... 0x18), which is what the
//      shipped `sub eax, 18h` / `jnz` pair is.
//
//  +0x394 is left at +infinity - the float at 0x6FA7930C is 0x7F800000 - and
//  is spelled as the bit pattern for that reason.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base, five sub-objects and three owned allocations.
//
//  Own translation unit: forty-odd real calls out of it.
//============================================================================
#include "gameui.h"
#include "sprite.h"
#include "preferences.h"
#include "storm.h"
#include "controllerslot.h"
#include "refcnt.h"

//  0x6F009EE0 / 0x6F00A8B0 - Config/configfloat.cpp and Config/configcolor.cpp.
//  GetConfigFloat answers a CFloat by value, which is why the shipped call
//  site takes the address of a stack temporary: the bit pattern that comes
//  back is what is stored, unconverted.
CFloat __fastcall GetConfigFloat(const char* section, const char* key,
                                 int index);
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);
//  0x6F4EAF50 / 0x6F4EAFE0 - Widget/fogobjectcreate.cpp and
//  Widget/lightobjectcreate.cpp.  Both answer a TRefCnt*, and the return
//  type is part of the mangled name of a free function, so a `void*`
//  spelling here would be a second symbol nothing defines.
TRefCnt* __fastcall CreateFogObject();
TRefCnt* __fastcall CreateLightObject();
//  0x6F4EA040 - Misc/... put a freshly made handle object on the live list.
void __fastcall ActivateHandleObject(void* object);
//  0x6F4D3330 - Misc/... publish one global.  Its `this` is null here.
void __fastcall PublishGlobal_6F4D3330(void* self);

//  0x6F606EA0 / 0x6F38BF70 / 0x6F38BF80 - Misc/sweep_field_setters.cpp.
struct SDirtyFlagOwner { void __thiscall OrField128And124(int bits); };
struct SPlainFieldOwner
{
    void __thiscall SetField1A4(int value);
    void __thiscall SetField1A8(int value);
};

//----------------------------------------------------------------------------
//  The two array types the constructor resizes.  Both are declared where
//  their own bodies are (GameUI/worldframeslotarray.cpp,
//  GameUI/waypointindicatorarray.cpp); re-declared here with only the two
//  members this file calls, because gameui.h keeps their storage as plain
//  fields so that the member-initialiser list can name them.
//----------------------------------------------------------------------------
struct SWorldFrameSlotArray
{
    //  0x6F39A620 - GameUI/worldframeslotarray.cpp.
    void SetCount(unsigned int count);
};

struct SWaypointIndicatorArray
{
    //  0x6F399060 / 0x6F393270 - GameUI/waypointindicatorarray.cpp and
    //  GameUI/waypointindicatorreset.cpp.
    void SetCount(unsigned int count);
    void ResetAll();
};

//  The two explicit specialisations Containers/agentptrarray_thunks.cpp and
//  Containers/unitagentptrarray.cpp own.  Declared so this translation unit
//  does not implicitly instantiate the generic bodies instead.
template <> void TSGrowableArray<SCheckedUnitSlot>::SetAlloc(unsigned int);
template <> unsigned int
TSGrowableArray<SCheckedSelectableSlot>::ComputeChunk(unsigned int);
template <> void TSGrowableArray<SCheckedSelectableSlot>::SetAlloc(unsigned int);
template <> unsigned int
TSGrowableArray<SCheckedGhostImageSlot>::ComputeChunk(unsigned int);
template <> void TSGrowableArray<SCheckedGhostImageSlot>::SetAlloc(unsigned int);

//  Storm's SetCount grow with nothing after it.  __forceinline because the
//  shipped code has all five copies inline and /Ob2's cost model declines a
//  body this size on its own.
template <class T>
static __forceinline void ReserveArray(TSGrowableArray<T>& arr,
                                       unsigned int count)
{
    if (count > arr.m_count && count > arr.m_alloc)
    {
        unsigned int chunk = arr.m_chunk;
        if (chunk == 0)
            chunk = arr.ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = count + (chunk - over);

        arr.SetAlloc(alloc);
    }

    arr.m_count = count;
}

static const char kWorldFrameFile[] = ".\\CWorldFrameWar3.cpp";

//  0x6FA7930C - +infinity, written as its bit pattern because that is what
//  the shipped constant is.
static const unsigned int kPositiveInfinity = 0x7F800000u;

//  0x6FA7932C - the light's own (0.5, 0.5, 0.5).
static const float kHalfWhite[3] = { 0.5f, 0.5f, 0.5f };

//  One of the three 0x1C-byte sprite slots at +0x16C .. +0x174.  A helper
//  because the three allocations are member initialisers - they sit between
//  the store of +0x168 and the store of +0x17C, which only the initialiser
//  list can spell - and a member initialiser is one expression.
static __forceinline SSpriteSlot* MakeSpriteSlot(unsigned int line)
{
    void* block = SMemAlloc(0x1C, kWorldFrameFile, line, 0);
    return block ? ((SSpriteSlot*)block)->Init(1) : 0;
}

CWorldFrameWar3::CWorldFrameWar3(void* parent)
    : CFrame((int)parent, 0, 0),
      m_field168(0),
      m_pSlot16C(MakeSpriteSlot(0xA8F)),
      m_pSlot170(MakeSpriteSlot(0xA90)),
      m_pSlot174(MakeSpriteSlot(0xA91)),
      m_field17C(0),
      m_field180(0),
      m_float18C(0.0f),
      m_float190(0.0f),
      m_float194(0.0f),
      m_camera(0),
      m_pMinimap(0),
      m_field1A4(1),
      m_field1A8(1),
      m_cursorMode(0),
      m_modeCount(0),
      m_modeCapacity(0),
      m_pModes(0),
      m_field1BC(0),
      m_suppressed(0),
      m_pausedByGame(0),
      m_texturePath(0),
      m_cursorSpan0(0.0f),
      m_cursorSpan1(0.0f),
      m_cursorSpan2(0.0f),
      m_cursorSpan3(0.0f),
      m_field1E4(0),
      m_field1E8(0),
      m_field1EC(0),
      m_field1F0(0),
      m_playerMaskBySlot(),
      m_playerMaskByRecord(),
      m_float238(0.0f),
      m_float23C(0.0f),
      m_pTrackTarget(0),
      m_pTrackOwner(0),
      m_field248(0),
      m_field250(0),
      m_filter(),
      m_field2F8(0),
      m_field2FC(0),
      m_field300(-1),
      m_field30C(0),
      m_float310(0.0f),
      m_float314(0.0f),
      m_float318(0.0f),
      m_pauseLatched(0),
      m_pauseKind(0),
      m_field324(1),
      m_field328(1),
      m_field330(1),
      m_pDayModel(0),
      m_pNightModel(0),
      m_field348(0),
      m_field354(0),
      m_field358(1),
      m_field35C(0),
      m_field360(0),
      m_field364(0),
      m_field368(0),
      m_field36C(0),
      m_slotAlloc(0),
      m_slotCount(0),
      m_pSlots(0),
      m_field37C(0),
      m_float380(0.0f),
      m_float384(0.0f),
      m_field388(1),
      m_field38C(0),
      m_float390(0.0f),
      m_float394(*(const float*)&kPositiveInfinity),
      m_colorFriend(0),
      m_colorNeutral(0),
      m_colorEnemy(0),
      m_field3AC(0)
{
    //  Everything above is the member-initialiser list, in declaration
    //  order, because that is what the shipped stream is: the unwind-state
    //  byte the constructor writes before each sub-object climbs from 1 to
    //  0x19 without a break, and only the config reads below carry state
    //  0x1A.  Nothing here can be moved into the body without pushing every
    //  store after it past the whole list.
    //
    //  The eleven arrays below are the tail of that same run, and they are
    //  the one part the list cannot spell: TSGrowableArray has no
    //  constructor of its own (its zeroing is the containing class's, in
    //  every user of it in this repo), and a member initialiser cannot name
    //  one of its fields.  Being last, they cost nothing - the first body
    //  statement lands exactly where the list would have ended.
    m_rallyIndicators.m_alloc = 0;
    m_rallyIndicators.m_count = 0;
    m_rallyIndicators.m_data  = 0;

    m_rallyOrder.m_alloc = 0;
    m_rallyOrder.m_count = 0;
    m_rallyOrder.m_data  = 0;

    m_pRallySourceSprite   = 0;
    m_pTargetConfirmSprite = 0;

    m_waypointAlloc = 0;
    m_waypointCount = 0;
    m_pWaypoints    = 0;

    m_units.m_alloc = 0;
    m_units.m_count = 0;
    m_units.m_data  = 0;
    m_units.m_chunk = 0;
    m_destructables.m_alloc = 0;
    m_destructables.m_count = 0;
    m_destructables.m_data  = 0;
    m_destructables.m_chunk = 0;
    m_items.m_alloc = 0;
    m_items.m_count = 0;
    m_items.m_data  = 0;
    m_items.m_chunk = 0;
    m_field5D0 = 0;
    m_field5D4 = 0;
    m_field5D8 = 0;
    m_field5DC = 0;
    m_effects.m_alloc = 0;
    m_effects.m_count = 0;
    m_effects.m_data  = 0;
    m_effects.m_chunk = 0;
    m_ghostImages.m_alloc = 0;
    m_ghostImages.m_count = 0;
    m_ghostImages.m_data  = 0;
    m_ghostImages.m_chunk = 0;
    m_selectedUnits.m_alloc = 0;
    m_selectedUnits.m_count = 0;
    m_selectedUnits.m_data  = 0;
    m_selectedUnits.m_chunk = 0;
    m_field610 = 0;
    m_field614 = 0;
    m_field618 = 0;
    m_field61C = 0;
    m_trackedWidgets.m_alloc = 0;
    m_trackedWidgets.m_count = 0;
    m_trackedWidgets.m_data  = 0;
    m_trackedWidgets.m_chunk = 0;
    m_selectables.m_alloc = 0;
    m_selectables.m_count = 0;
    m_selectables.m_data  = 0;
    m_selectables.m_chunk = 0;
    m_field640 = 0;
    m_field644 = 0;
    m_field648 = 0;
    m_field64C = 0;
    m_ghostSlots.m_alloc = 0;
    m_ghostSlots.m_count = 0;
    m_ghostSlots.m_data  = 0;
    m_ghostSlots.m_chunk = 0;

    //  [Misc] and [SelectionCircle], read once at construction.  Both floats
    //  come back as a CFloat by value and are kept as the bit pattern the
    //  shipped code stores.
    CFloat animTime = GetConfigFloat("Misc", "ScaledAnimTime", 0);
    m_scaledAnimTime = animTime.m_bits;
    CFloat dayHours = GetConfigFloat("Misc", "DayHours", 0);
    m_dayHours = dayHours.m_bits;

    unsigned int colour;
    GetConfigColor(&colour, "SelectionCircle", "ColorFriend", 0);
    m_colorFriend = colour;
    GetConfigColor(&colour, "SelectionCircle", "ColorNeutral", 0);
    m_colorNeutral = colour;
    GetConfigColor(&colour, "SelectionCircle", "ColorEnemy", 0);
    m_colorEnemy = colour;

    m_playerMaskBySlot.m_count   = 12;
    m_playerMaskByRecord.m_count = 12;

    ((SWorldFrameSlotArray*)&m_slotAlloc)->SetCount(8);

    ((SWaypointIndicatorArray*)&m_waypointAlloc)->SetCount(0x100);
    ((SWaypointIndicatorArray*)&m_waypointAlloc)->ResetAll();

    LatchLocalPlayer();

    PublishGlobal_6F4D3330(0);

    m_selectedUnits.SetCount(0x7D0);
    m_trackedWidgets.SetCount(0xC8);
    m_selectables.SetCount(0x14);
    m_ghostSlots.SetCount(0x7D0);

    ReserveArray(m_units, 0x200);
    m_units.m_chunk = 0x80;
    m_units.m_count = 0;

    ReserveArray(m_destructables, 0x400);
    m_destructables.m_chunk = 0x100;
    m_destructables.m_count = 0;

    ReserveArray(m_items, 0x10);
    m_items.m_chunk = 0x10;
    m_items.m_count = 0;

    ReserveArray(m_effects, 0x10);
    m_effects.m_chunk = 0x10;
    m_effects.m_count = 0;

    ReserveArray(m_ghostImages, 0x10);
    m_ghostImages.m_chunk = 0x10;
    m_ghostImages.m_count = 0;

    //  Downward, off a byte offset: eight slots of 0x18 bytes each, the id
    //  of every one of them set to -1.
    for (unsigned int offset = 0xC0; offset != 0; offset -= 0x18)
        *(int*)((char*)m_pSlots + offset - 0x18) = -1;

    ((SDirtyFlagOwner*)this)->OrField128And124(4);

    m_flagMask |= 5;
    m_flags    |= 5;

    ((SPlainFieldOwner*)this)->SetField1A4(5);
    ((SPlainFieldOwner*)this)->SetField1A8(7);

    Dispatch(0x1A0064, 0x1A0064, parent);
    Dispatch(0x1A0065, 0x1A0065, parent);
    Dispatch(0x1A0068, 0x1A0068, parent);
    Dispatch(0x400500C9, 0x400500C9, parent);

    m_pFog = CreateFogObject();
    ApplyTerrainFogConfig();

    SetDayNightModels(
        (unsigned int)"Environment\\DNC\\DNCLordaeron\\DNCLordaeronTerrain"
                      "\\DNCLordaeronTerrain.mdl",
        (unsigned int)"Environment\\DNC\\DNCLordaeron\\DNCLordaeronUnit"
                      "\\DNCLordaeronUnit.mdl");

    CreateRallyIndicators();
    CreateTargetConfirmSprite();
    CreateWaypointIndicators();

    //  The shipped stream re-reads m_pWorldLight for every use after the
    //  first: only the first call gets the value still sitting in eax from
    //  CreateLightObject, and each intervening call clobbers it.  Holding the
    //  pointer in a local instead costs six instructions and keeps it in a
    //  callee-saved register the shipped code never spends here.
    m_pWorldLight = CreateLightObject();

    SetControllerSlotFloat(m_pWorldLight, 2, 0.75f);
    SetControllerSlotFloat(m_pWorldLight, 3, 0.0f);
    ResetControllerSlot(m_pWorldLight, 0, (void*)kHalfWhite);

    //  0x10, not 0: the shipped `push ebp` at 0x6F39B0F6 is the constant
    //  register MSVC still has 0x10 in from the four array reserves above,
    //  and 0x10 is what CFrame::AddLight parks in the LIGHTNODE's +0x0C.
    AddLight(m_pWorldLight, 0x10);
    ActivateHandleObject(m_pWorldLight);

    CPreferencesWar3* prefs = GetPreferences();
    prefs->ApplyStoredPreference(6);
    prefs->ApplyStoredPreference(8);
    prefs->ApplyStoredPreference(0x0B);
    prefs->ApplyStoredPreference(0x0E);
    prefs->ApplyStoredPreference(7);
    prefs->ApplyStoredPreference(0x0F);
}
