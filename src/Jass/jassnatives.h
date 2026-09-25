//============================================================================
//  Shared declarations for the JASS native entry points in
//  jassnatives_*.cpp.
//
//  Every native in this family is registered by the same table walk at
//  0x6F3D4020, which is also the ground truth for each one's signature:
//
//      push offset "(Hlocation;RR)V"
//      mov  edx, offset "MoveLocation"
//      mov  ecx, offset sub_6F3C0DF0
//      call sub_6F455110
//
//  Two conventions follow from that table and from the bodies themselves:
//  every native is __cdecl (they all end in a bare `retn`), and a JASS
//  `real` argument arrives as a *pointer* to the value while a `real` result
//  comes back in eax as its raw 32-bit encoding, never in st(0).  Handle
//  arguments are plain ints that the per-type resolvers below turn into an
//  object or into null.
//============================================================================
#ifndef JASSNATIVES_H
#define JASSNATIVES_H

#include "storm.h"

//  dword_6FAB65F4.  itemhandlemain.h types the same pointer as
//  CItemSlotSingletonHost and agentregistry.h reaches the same object as a
//  CAgentRegistry; this is a third view of it, for the members and the
//  tables the player/location natives use.  One `extern` and a cast per view
//  rather than one `extern` per view, so funcmap.py's DATA table keeps
//  exactly one C++ name for the address.
extern void* g_unk6FAB65F4;

//----------------------------------------------------------------------------
//  One entry of a start location's ally-priority list.  The name is not a
//  guess: TSGrowableArray hands Storm the element type's RTTI descriptor
//  name as its allocation tag, and the one this array passes is
//  ".?AUAllySlot@@" - IDA's `aAuallyslot`.  A fresh entry is
//  {-1, 0}, which is what SetCount's construction loop stores.
//
//  Which of the two words is which comes from the natives themselves:
//  SetStartLocPrio "(IIIHstartlocprio;)V" writes its third argument (the
//  other start location) into the first word and its fourth (the priority)
//  into the second, and GetStartLocPrio "(II)Hstartlocprio;" reads the
//  second word back.
//----------------------------------------------------------------------------
struct AllySlot
{
    AllySlot() : m_startLocation(-1), m_priority(0) {}

    int m_startLocation;   // +0x00
    int m_priority;        // +0x04
};

//  One start location as the game state stores them: a 0x2C-byte record.
//  The coordinate pair is what the natives read; the three flag words below
//  are what DefineStartLocation and ForcePlayerStartLocation write, and
//  everything else in the record is untouched here and deliberately
//  unnamed.
struct SStartLocation
{
    int  m_x;              // +0x00
    int  m_y;              // +0x04
    //  +0x08 - the ally-priority list.  Its m_data lands at +0x10 of the
    //  record, which is the 0xBC the two accessors below index off `this`
    //  (0xAC + 0x08 + 0x08).
    TSGrowableArray<AllySlot> m_priorities;   // +0x08
    //  +0x18 and +0x28 are both set to 1 by ForcePlayerStartLocation and
    //  nothing in this family reads either; +0x24 is set to 1 by
    //  DefineStartLocation, which is the only thing that ever writes the
    //  coordinate pair, so it reads as "this slot has been placed".  Two of
    //  the three keep an address-derived name because the call tree says
    //  what is written and not what it means.
    int  m_field18;        // +0x18
    char m_reserved1C[8];
    int  m_defined;        // +0x24
    int  m_field28;        // +0x28
};

class CJassGameState
{
public:
    //  0x6F3A1660 - the team index a player belongs to.
    int GetPlayerTeam(int playerIndex);
    //  0x6F3A82B0 - move a player to a team.
    void SetPlayerTeam(int team, int playerIndex);
    //  0x6F3A1320 - place start location `index` at (*x, *y).
    void DefineStartLocation(int index, const int* x, const int* y);
    //  0x6F3A3240 - pin a player to a start location.
    void ForcePlayerStartLocation(int playerIndex, int startLocation);
    //  0x6F3A4F70 - resize start location `index`'s ally-priority list.
    //  `retn 8`.
    void SetStartLocPrioCount(int index, unsigned int count);
    //  0x6F3A12B0 - write one entry of that list.  `retn 10h`.
    void SetStartLocPrio(int index, int slot, int otherStartLocation,
                         int priority);
    //  0x6F3A1300 - read one entry's priority back.  `retn 8`.
    int GetStartLocPrio(int index, int slot);
    //  0x6F3A12E0 - read the *other half* of the same entry back: which
    //  start location that priority slot names.  `retn 8`.
    int GetStartLocPrioSlot(int index, int slot);


    char           m_reserved000[0xA0];
    //  +0xA0 - base of a 0x10-byte-stride array, one entry per team, that
    //  SetPlayerTeam (jassgamestate_playerteam.cpp) links a player's own
    //  per-player record into/out of.  Nothing here establishes the entry's
    //  own layout past "something CJassTeamRoster::Add/Remove take a
    //  pointer to", so it stays void*.
    void*          m_teamRosters;         // +0xA0
    char           m_reservedA4[0xAC - 0xA4];
    SStartLocation m_startLocations[1];   // +0xAC
};

#define g_pJassGameState ((CJassGameState*)g_unk6FAB65F4)

//----------------------------------------------------------------------------
//  The per-type handle resolvers.  All three are the same function shape as
//  ResolveItemHandle (itemhandlemain.h) - the same lazy singleton, the same
//  'agl+' tag and pending-flag guards, under the same SEH frame - against a
//  different type table.  None is reconstructed here.
//----------------------------------------------------------------------------
struct SJassLocation
{
    char m_reserved00[0x24];
    int  m_x;              // +0x24
    int  m_y;              // +0x28
};

struct SJassPlayer
{
    char          m_reserved00[0x20];
    //  +0x20 - a flags word.  Bit 2 is the one
    //  NotifyPlayerControllerChanged (jassplayercontrollernotify.cpp) sets
    //  or clears; nothing here reads it back, so what it gates elsewhere is
    //  not established.
    unsigned int  m_field20;       // +0x20
    char          m_reserved24[0x30 - 0x24];
    unsigned char m_playerIndex;   // +0x30
    char          m_reserved31[0x22B];
    unsigned int  m_raceFlags;     // +0x25C
    char          m_reserved260[4];
    unsigned int  m_color;         // +0x264
    int           m_controller;    // +0x268
    char          m_reserved26C[8];
    int           m_startLocation; // +0x274
    char          m_reserved278[0x328 - 0x278];
    //  +0x328 - CripplePlayer's own "handicap" flag word
    //  (SJassPlayerRates::SetCrippled, jassplayercripple.cpp), stored as a
    //  full dword even though only the low 16 bits are ever a real value.
    int           m_field328;      // +0x328
};

//  0x6F3BD3B0
SJassLocation* __fastcall ResolveLocationHandle(int hLocation);
//  0x6F3BD4D0
SJassPlayer* __fastcall ResolvePlayerHandle(int hPlayer);

//  The flag bit SetPlayerRaceSelectable owns inside SJassPlayer::m_raceFlags,
//  and the one bit SetPlayerRacePreference is careful not to disturb.
const unsigned int kPlayerFlagRaceSelectable = 0x40;

//  0x6F416700 - re-evaluate a player's own +0x20 flags bit 2 after a
//  controller change.  `retn 0`, so no stack arguments beyond `this` in
//  ecx.  See jassplayercontrollernotify.cpp.
void __fastcall NotifyPlayerControllerChanged(SJassPlayer* player);

//  0x6F3BAA40 - a JASS string handle to the C string behind it.  `retn 4`.
const char* __stdcall JassStringToCString(int hString);

//  0x6F3BAA20 - the other direction: a C string to a JASS string handle
//  (jassmakestringhandle.cpp).
int __fastcall MakeJassStringHandleFromCString(const char* str);

//  0x6F4C5CF0 - assign a C string into the counted string at `this`.
//  `retn 4`.
struct STStringField { void Assign(const char* value); };


//----------------------------------------------------------------------------
//  The rest of the per-type handle resolvers.  Every one of them is the same
//  115-instruction body as ResolveLocationHandle against a different type
//  table; none is reconstructed here.  Their objects are opaque except for
//  the fields these natives actually read.
//
//  Everything the natives call on one of these objects is declared as a
//  *member*, never as a free function taking the receiver: the shipped call
//  sites all put the object in ecx and push every other argument, which is
//  __thiscall, and a free __fastcall would take the second argument in edx
//  and clean four bytes too few.  Each `retn <n>` below was read off this
//  batch's own dumps and is what fixes the argument count.
//----------------------------------------------------------------------------
struct SJassRegion
{
    //  0x6F47CFB0 - set or clear one cell.  `retn 0Ch`.
    void SetCell(const int* x, const int* y, int set);
    //  0x6F47CBA0 - the same for a whole rectangle, whose four bounds arrive
    //  as one block.  `retn 8`.
    void SetRect(const int* bounds, int set);
};

struct SJassTimer
{
    //  0x6F3F6890 - arm the timer.  `retn 0Ch`.
    void Start(const int* seconds, int periodic, int callback);
    //  0x6F3ECD30 - elapsed time, written into the caller's buffer and also
    //  returned.  `retn 4`.
    int* GetElapsed(int* out);
    //  0x6F3E8F30 - resume after a pause.  A tail jump from ResumeTimer, and
    //  the one callee in this batch with no dump of its own; `retn 0`
    //  follows from the call site pushing nothing.
    void Resume();
};

struct SJassBoolExpr {};

struct SJassForce
{
    //  The force's own handle ref at +0x24, resolved through
    //  HandleRefFieldOwner_6F473170::QueryField78 (queryfield.cpp).
    char m_reserved00[0x24];
};

//  The four bounds as one object, so that copying them is one struct
//  assignment - see jassnatives_region.cpp for why that matters.
struct SJassBounds
{
    //  A constructor and not aggregate initialisation, for the reason
    //  SJassPoint below has one: it is what lets MSVC build the temporary
    //  straight in the outgoing argument slot, which is JASS_RectFromLoc's
    //  own `sub esp,10h` / four stores through `esp`.
    SJassBounds(int minX, int minY, int maxX, int maxY)
        : m_minX(minX), m_minY(minY), m_maxX(maxX), m_maxY(maxY) {}

    int m_minX;
    int m_minY;
    int m_maxX;
    int m_maxY;
};

//  A rect keeps its four bounds as one contiguous block at +0x24, which is
//  the block RegionAddRect/RegionClearRect copy onto their own stack before
//  passing its address on.
struct SJassRect
{
    //  0x6F3DB8F0 - overwrite all four bounds at once, taking them by value
    //  as one block.  `retn 10h`, `this` being the rect.  See
    //  jassrectbounds.cpp.
    void SetBounds(SJassBounds bounds);
    //  0x6F3DB9D0 - the same four stores at a second address, the one the
    //  mutating natives call.  See jassrectbounds.cpp for why both are kept.
    void Resize(SJassBounds bounds);

    char m_reserved00[0x24];
    //  The four words are *not* in (minX, minY, maxX, maxY) order in the
    //  shipped layout - GetRectMinX reads +0x28 and GetRectMaxX reads +0x30
    //  (jassnatives_rect.cpp), and JASS_RectFromLoc writes the pair it
    //  computed for Y into +0x24/+0x2C.  The names below are kept as the
    //  first target spelled them, so that every existing call site still
    //  reads; what they mean is one word crossed.
    int  m_minX;   // +0x24
    int  m_minY;   // +0x28
    int  m_maxX;   // +0x2C
    int  m_maxY;   // +0x30
};

SJassRegion*   __fastcall ResolveRegionHandle(int hRegion);
SJassTimer*    __fastcall ResolveTimerHandle(int hTimer);
SJassBoolExpr* __fastcall ResolveFilterHandle(int hFilter);
SJassForce*    __fastcall ResolveForceHandle(int hForce);
SJassRect*     __fastcall ResolveRectHandle(int hRect);

//  0x6F42DCC0 - drop an object out of the handle registry.  `retn 4`.  Its
//  receiver is what CItemSlotSingletonHost::GetSlotTable (itemhandlemain.h)
//  hands back, not dword_6FAB65F4 itself - the same holder agentregistry.h
//  reaches as a CAgentRegistry, typed here for the one member these natives
//  use.
class CJassHandleRegistry
{
public:
    void ReleaseObject(void* object);
};

//  Re-declared (not redefined) to match misc_handle_lookups.cpp's own struct
//  exactly, so the compiler mangles this declaration the same way and
//  CripplePlayer's call resolves to the real symbol instead of a second one.
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int  QueryField78();
    //  0x6F473140 - the same member playerunitcounters.cpp and
    //  agentregistrysingleton.cpp each re-declare locally; declared here too
    //  so the start-location members can publish through it.
    void SetField78(int value);
};

//  Two per-player-pair rate tables GetPlayerTaxRate picks between and
//  SetPlayerTaxRate writes into (jassplayerrates.cpp), plus the cripple
//  state CripplePlayer sets.  All four `retn 4`/`retn 8` as noted below.
struct SJassPlayerRates
{
    int  GetRateA(int otherPlayerIndex);           // 0x6F40F7F0, retn 4
    int  GetRateB(int otherPlayerIndex);           // 0x6F40F820, retn 4
    //  0x6F40F800/0x6F40F830 - both `retn 8`.  Argument order matches the
    //  caller's own push order (rightmost pushed first): the rate value
    //  clamped to [0,100] first, the other player's index second.
    void SetRateA(int value, int otherPlayerIndex); // 0x6F40F800
    void SetRateB(int value, int otherPlayerIndex); // 0x6F40F830
    void SetCrippled(int crippled, int flag); // 0x6F416670, retn 8
};


//----------------------------------------------------------------------------
//  The rest of this batch: the destroy-family types, the rect helpers and the
//  region membership tests.  Same rules as above - members, not free
//  functions, and every `retn <n>` read off the dumps.
//----------------------------------------------------------------------------

//  Every "destroy this handle" native in the family - DestroyTimer,
//  DestroyTrigger, DestroyBoolExpr, DestroyFogModifier, RemoveRect,
//  RemoveRegion - is the same five instructions: resolve, null-check, tail
//  jump through vtable slot 23 (+0x5C).  That is the same slot agent.h names
//  Method_0x5C on CAgent, dispatched by hand here because nothing in this
//  repo establishes a vtable for any of these object types and a guessed one
//  would misplace every slot after it.
inline void JassDestroySelf(void* object)
{
    typedef void (__thiscall *DestroySelfFn)(void*);
    ((DestroySelfFn)(*(void***)object)[0x5C / 4])(object);
}

struct SJassFogModifier
{
    char         m_reserved00[0x20];
    unsigned int m_flags;   // +0x20 - bit 6 is "running"
};

struct SJassTrigger
{
    char m_reserved00[0x58];
    int  m_evalCount;   // +0x58 - both cleared by ResetTrigger
    int  m_execCount;   // +0x5C
    char m_reserved60[4];
    //  +0x64 - the "enabled" flag, reached through the same
    //  HandleRefFieldOwner_6F473170::SetField78/QueryField78 pair
    //  CripplePlayer and the start-location members use on other objects.
    //  EnableTrigger/DisableTrigger call SetField78(1)/SetField78(0);
    //  IsTriggerEnabled tail-jumps into QueryField78.
    HandleRefFieldOwner_6F473170 m_enabledOwner;   // +0x64, 0x10 bytes (0x64-0x74)
    char m_reserved74[0x78 - 0x74];
    //  +0x78 - "wait on sleeps", a plain int flag: TriggerWaitOnSleeps
    //  stores its argument here directly (no vtable dispatch) and
    //  IsTriggerWaitOnSleeps reads it straight back.
    int  m_waitOnSleeps;
};

//  0x6F3BDDD0 / 0x6F3BDEF0 / 0x6F3BD950 - three more resolvers of the same
//  115-instruction shape.
SJassFogModifier* __fastcall ResolveFogModifierHandle(int hFogModifier);
SJassTrigger*     __fastcall ResolveTriggerHandle(int hTrigger);
SJassBoolExpr*    __fastcall ResolveBoolExprHandle(int hBoolExpr);

//  0x6F3BB260 - jasstriggerdestructableeventslot.cpp.
int GetDestructableEventSlotHandle();
//  0x6F3BE010 - jassresolvedestructablehandle.cpp.  Not a JASS handle
//  resolver (its slot is SCheckedDestructableSlot, agenttypedslots.h's
//  '+w3d' entry) - used here only for its truthiness.
class CAgent;
CAgent* __fastcall ResolveDestructableHandle(int hDestructableEventSlot);

//  The bit FogModifierStart sets and FogModifierStop clears.
const unsigned int kFogModifierRunning = 0x40;

//  The default rect bound the rect getters fall back to when the handle does
//  not resolve.  flt_6FAAE470 is CFloat.h's own g_CFloatZero, so it is spelled
//  that way and read through its bits: every real crosses the JASS boundary
//  as its raw encoding, never as a float.
#include "CFloat.h"

//  The point MoveTo takes *by value*: the shipped call site carves eight
//  bytes off the stack and writes the pair into them, which is how MSVC
//  passes an eight-byte struct, not how it pushes two ints (that would be
//  two `push`es in the other order).  The constructor is what lets the
//  compiler build the temporary directly in the outgoing argument slot.
struct SJassPoint
{
    SJassPoint(const CFloat& x, const CFloat& y) : m_x(x), m_y(y) {}
    CFloat m_x;
    CFloat m_y;
};

//  Members on SJassRect, past the four bounds declared above.
struct SJassRectCentre
{
    //  0x6F3B1FB0 - the rect's centre point, written into the caller's own
    //  two-dword buffer.  `retn 4`, `this` being the bounds block at +0x24.
    //  It also answers that same buffer in eax, which is what MoveTo reads
    //  the pair back through (jassrectmoveto.cpp); the two natives that only
    //  want one half ignore the result.
    int* GetCentre(int* out);
    //  0x6F3B1F10 - shift the bounds block by a delta point handed over by
    //  address.  `retn 4`, `this` being the bounds block at +0x24 - the same
    //  receiver GetCentre takes.  The byte count is not a guess: MoveTo
    //  pushes the address and then pops its three saved registers off an
    //  `esp` that is only correct if the callee cleaned those four bytes.
    void OffsetBy(const SJassPoint* delta);
    //  0x6F3B2360 - move the rect so that its centre lands on the point the
    //  caller has just built on the stack.  `retn 8`, `this` being the rect.
    void MoveTo(SJassPoint point);

    //  The same four-word block SJassRect's own m_minX/m_minY/m_maxX/m_maxY
    //  declares (this type is that block re-viewed through +0x24), named
    //  here too because GetCentre (jassrectgetcentre.cpp) reads all four -
    //  see that file for the same "one word crossed" labelling note.
    CFloat m_minX;   // +0x00
    CFloat m_minY;   // +0x04
    CFloat m_maxX;   // +0x08
    CFloat m_maxY;   // +0x0C
};

//  More members on SJassRegion.
struct SJassRegionTests
{
    //  0x6F47D080 - is this point inside?  `retn 8`.
    int ContainsPoint(const int* x, const int* y);
    //  0x6F47D3E0 - is this unit's position sub-object inside?  `retn 4`.
    int ContainsUnitPosition(void* unitPositionAt164);
};

//  nullsub_14 - `retn 4`.  SetPlayerUnitsOwner's whole body past the resolve
//  is a call to an empty function: the native is a no-op in the shipped
//  build, not something this reconstruction failed to find.
struct SJassPlayerUnits { void SetOwner(int newOwner); };

//  0x6F3BDCB0 - the unit resolver, shared with
//  jassunitadditembyid_native.cpp.  CUnit is forward-declared as a *class*
//  and not re-spelled as a struct: MSVC encodes which of the two keywords a
//  type was declared with into the mangled name, so a `struct CUnit` here
//  and a `class CUnit` there would be two symbols the linker never joins.
class CUnit;
CUnit* __fastcall ResolveUnitHandle(int hUnit);

//  0x6F3BE5B0 - the widget resolver, the one the *widget*-typed natives use
//  (GetWidgetLife, SetWidgetLife, UnitDamageTarget's second argument).  Same
//  115-instruction lazy-singleton / 'agl+'-tag / pending-flag shape as
//  ResolveUnitHandle, differing only in which checked-slot type it stages
//  the candidate through (0x6F0382B0 rather than 0x6F038450).
//
//  Still a thunk, deliberately, and the same call as ResolvePlayerHandle
//  above: the shipped body is wrapped in the __except_handler4 frame that
//  docs/msvc-vc8-idioms.md records as unreproducible at this toolchain's
//  fixed /GS- /EHs-c-, so a reconstruction of it scores what its unit twin
//  scores - 0.33, all of the loss in the frame - and cannot be exact.
//  Thunked, the natives that call it reach the real, unhooked resolver,
//  which is also the safer live behaviour.  asm/ has the dump when someone
//  wants the other 0.67.
class CWidget;
CWidget* __fastcall ResolveWidgetHandle(int hWidget);

#endif
