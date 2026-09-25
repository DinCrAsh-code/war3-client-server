//============================================================================
//  DELIBERATE BEHAVIOUR CHANGE - not a reconstruction of anything shipped.
//
//  Two debug features, each switched on by uncommenting a `#define` at the
//  top of the .cpp that *calls* into here:
//
//    FEATURE_ITEM_CREATE_LOG   src/item_place.cpp   - log every item as it
//                                                     is placed in the world
//    FEATURE_ITEM_TELEPORT_F3  src/CTimeSync.cpp    - F3 gathers loose items
//
//  WHY THE SWITCHES LIVE IN THE CALLER AND NOT IN A HEADER
//  -------------------------------------------------------
//  tools/build.sh only forces a full rebuild when the *compile flags*
//  change - it fingerprints those and nothing else.  A header edit does not
//  invalidate the .obj/.asm of anything that includes it, so a `#define` in
//  a header would flip for the one TU that got rebuilt and stay stale in
//  every other, producing a .mix that is half-on.  Putting each switch in
//  the single .cpp whose own rebuild it triggers is what makes toggling it
//  reliable.  That is also why there is no feature_itemdebug.h: the two
//  entry points below are declared by hand at each call site instead, so
//  there is no header whose edit could go unnoticed.
//
//  This file itself is compiled unconditionally and is inert until called -
//  it has no funcmap.py entry, so nothing here is ever JMP-patched over a
//  shipped function, and with both switches commented out nothing calls it
//  and every reconstruction keeps its exact score.
//
//  READING THE LOG: DebugView with "Capture Win32" + "Capture Global Win32",
//  or tools/run_war3.py's result["debug_strings"].  Same OutputDebugStringA
//  sink, and the same _vsnprintf-not-wsprintfA reason, as
//  gameaction0x26_handler.cpp's AH26_LOG - user32 is not linked into the
//  .mix (tools/build_mix.py links only gen_storm/libcmt/oldnames/Kernel32),
//  which is also why GetAsyncKeyState below is reached through
//  GetProcAddress rather than called directly.
//============================================================================
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "item.h"
#include "game.h"
#include "boxmath.h"        // SCellBoxF
#include "pathfind.h"       // FindFreeWorldPoint
#include "playercolor.h"    // g_unk6FAB65F4 - the world-object singleton

//  dword_6FAAE4F4 - the fixed placement radius CItem::PlaceFromDescriptor
//  passes to FindFreeWorldPoint; declared the same way it declares it.
extern const CFloat g_slopeThreshold1;

//  CItem vtable slot 89 (+0x164) - the object's own placement mask, the
//  fifth argument of the placement search.
typedef int (__thiscall *Slot0x164Fn)(void* self);

//============================================================================
//  NEVER USE %f (OR %e / %g) IN THIS FILE.  It crashes the game on load with
//
//      Runtime Error! R6002 - floating point not loaded
//
//  and it is not a bug in the format string, it is the link.  The injection
//  DLL's own glue.cpp *defines* `_fltused` itself so the compiler's
//  reference to it resolves; that satisfies the reference without pulling
//  the CRT's own fltused.obj out of libcmt.lib, and fltused.obj is what
//  installs the float-conversion callbacks _vsnprintf needs for %f.  The
//  table stays empty, and the first %f the CRT ever formats raises R6002.
//
//  Items are placed during map load, so a single %f here kills the game on
//  the loading screen before anything else runs.  This is also why the
//  already-working AH26_LOG (gameaction0x26_handler.cpp) uses only %02X /
//  %08X / %d / %u and no float specifier anywhere.
//
//  Print an encoded float as its raw bits (%08X - that is the ground truth,
//  it is what is actually stored) plus, where a readable number helps, its
//  truncated integer value through CFloatToInt (softfloat.cpp): pure
//  integer math, no CRT formatting and no x87 involved.
//============================================================================
#pragma warning(push)
#pragma warning(disable: 4996)   //  _vsnprintf "deprecated" - VC8's own
                                 //  replacement is _vsnprintf_s, and this is
                                 //  debug scaffolding, not shipped code
static void ITEMDBG_LOG(const char* fmt, ...)
{
    char line[256];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(line, sizeof(line) - 1, fmt, ap);
    va_end(ap);
    line[sizeof(line) - 1] = '\0';
    OutputDebugStringA(line);
}
#pragma warning(pop)

//============================================================================
//  Reused, never redefined.
//
//  This is *declared* here and defined elsewhere in src/ - a
//  second definition of it would be the duplicate-symbol defect
//  docs/notes/verifier-gate-link-failures.md catalogues (and the one that
//  already cost IndexedArrayHolder_6F3A1650::At its EXACT score once).  The
//  struct shapes have to match their real definitions exactly or the
//  mangled names will not meet at link time.
//
//    IndexedArrayHolder_6F3A1650::At   defined in misc_field_getters.cpp
//============================================================================
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

struct SPlayerRecord
{
    char  m_reserved00[0x34];
    void* m_selection;    // +0x34
};

//----------------------------------------------------------------------------
//  The selection record keeps the selected UNIT and the selected ITEM in two
//  adjacent, mutually exclusive slots.  Established by dumping the record
//  live with a hero selected and then with an item selected (the diagnostic
//  that used to live below), which gave exactly complementary values:
//
//      hero selected   sel+1E0: 0F560014 00000000     <- unit set, item null
//      item selected   sel+1E0: 00000000 0F5A008C     <- unit null, item set
//
//  sub_6F41FA10 (`mov eax,[ecx+1E4h]`, the getter CItem::Method_0x188 uses)
//  reads the *item* slot - correct for that function, which only ever asks
//  "am I the selected item", and the reason an earlier pass here reported
//  "nothing selected" for every hero.
//
//  +0x1E0 is a field offset read live, not a reconstructed member of any
//  named class, so everything that comes out of it goes through
//  LooksLikeGameObject below before it is dereferenced.
//----------------------------------------------------------------------------
enum { kSelectedUnitOffset = 0x1E0, kSelectedItemOffset = 0x1E4 };

static void* SelectionSlot(void* selectionRecord, int offset)
{
    return *(void**)((char*)selectionRecord + offset);
}

//  Is this safe to read a vtable pointer out of and call slot 46 on?
//
//  +0x1E0 was found by inspection rather than from a disassembly of whatever
//  writes it, so this refuses anything that is not plainly a live
//  polymorphic game object: readable, and with a vtable pointer inside
//  Game.dll's own image (the module is based near 0x6F000000, and every
//  vtable seen in this record - 6F94C9F4 - sits in that range).  A wrong
//  guess then logs and returns instead of faulting.
static int LooksLikeGameObject(const void* obj)
{
    if (!obj)
        return 0;
    if (IsBadReadPtr(obj, sizeof(void*)))
        return 0;

    unsigned int vtable = *(const unsigned int*)obj;
    if (vtable < 0x6F000000 || vtable >= 0x70000000)
        return 0;
    if (IsBadReadPtr((const void*)vtable, 0xC0))
        return 0;

    return 1;
}

//  CWidget vtable slot 46 (+0xB8).  Returns the object's handle-bearing
//  sub-object, or NULL - CWidget's own body is a plain `return 0`
//  (widget_vtable_leaves.cpp), so the result is null for anything that does
//  not override the slot and every call below has to check it.
typedef SHandleWithType* (__thiscall *GetHandleObjectFn)(void*);

//  5.0f.  CFloat stores an IEEE-754 single's own bit pattern (CFloat.h), so
//  the constant is spelled as those bits rather than built at runtime; the
//  add itself goes through CFloat::operator+ so it stays the deterministic
//  software add the simulation uses everywhere else.
static const int kCFloatBits_5_0 = 0x40A00000;

//============================================================================
//  WHY THE ITEMS ARE SPREAD OUT AND NOT ALL PUT ON THE ONE POINT
//
//  CItem::JASS_SetItemPosition_callee1 places the item exactly where it is
//  told.  CItem::PlaceFromDescriptor - the path the game itself uses when an
//  item enters the world - does not: it runs FindFreeWorldPoint first and
//  only then places, so a new item never lands on top of an existing one.
//
//  Sending every item to the same hero+(5,0) therefore stacked them all on
//  one point, and only one of the pile could be picked up or clicked at all;
//  the rest sat there inert, because a mouse/pick query against that cell
//  resolves to a single object however many are registered there.
//
//  So each item gets its own slot in a grid around the target *and* is then
//  put through the same FindFreeWorldPoint call PlaceFromDescriptor makes,
//  with the same thirteen arguments and the item's own placement mask from
//  vtable slot 89.  Two mechanisms on purpose: the grid alone guarantees
//  they are not co-located even if the search declines to move anything,
//  and the search is what makes each resting place actually legal.
//
//  64.0 world units is half a WC3 tile (a tile is 128), which is enough to
//  separate item footprints without throwing the pile far from the hero.
//============================================================================
static const int kCFloatBits_64_0 = 0x42800000;
enum { kSpreadColumns = 4 };

//============================================================================
//  The tracked-item table.
//
//  Populated by FeatureItemDebug_OnItemPlaced, walked by the F3 handler.
//  Deliberately NOT a walk of SItemSlotTableHolder::m_records: that table's
//  own element count is not established anywhere in this repo
//  (itemhandletable.h names only the m_records pointer), so walking it
//  would mean reading until something unmapped stops us.  Remembering the
//  items we actually watched get placed is bounded and cannot fault.
//
//  A fixed array, not a Storm allocation: this is debug scaffolding that
//  must not perturb the allocator the reconstruction is being judged
//  against.  Overflow drops new entries and says so once.
//============================================================================
enum { kMaxTrackedItems = 512 };
static CItem* g_trackedItems[kMaxTrackedItems];
static int    g_trackedCount = 0;
static int    g_trackedOverflowed = 0;

static int IsTrackedItem(const void* obj)
{
    for (int i = 0; i < g_trackedCount; ++i)
    {
        if (g_trackedItems[i] == obj)
            return 1;
    }
    return 0;
}

static void TrackItem(CItem* item)
{
    if (IsTrackedItem(item))
        return;                     //  re-placed, not new

    if (g_trackedCount >= kMaxTrackedItems)
    {
        if (!g_trackedOverflowed)
        {
            g_trackedOverflowed = 1;
            ITEMDBG_LOG("[itemdbg] tracking table full at %d - "
                        "later items will not teleport\n", kMaxTrackedItems);
        }
        return;
    }

    g_trackedItems[g_trackedCount++] = item;
}

//----------------------------------------------------------------------------
//  Is this object's handle-bearing sub-object currently resolvable?
//
//  `(handle & typeTag) == -1` is this codebase's own "unset" test for a
//  nullable {handle, typeTag} pair - the same one JASS_SetItemPosition
//  (jasssetitemposition_native.cpp) and Position::m_placementRef
//  (position.h) use.  It matters because CPathRef::ToWorldVec3 hands
//  LookupHandle's result straight to PointAtDistance with no null check of
//  its own (CPathRef.cpp): asking an unset ref where it is would fault.
//----------------------------------------------------------------------------
static SHandleWithType* ResolvableSubObject(void* obj)
{
    if (!obj)
        return 0;

    SHandleWithType* sub = ((GetHandleObjectFn)(*(void***)obj)[0xB8 / 4])(obj);
    if (!sub)
        return 0;
    if ((sub->m_handle & (unsigned int)sub->m_typeTag) == (unsigned int)-1)
        return 0;
    return sub;
}

//============================================================================
//  FEATURE 1 - log every item as it is placed.
//
//  Called from CItem::PlaceFromDescriptor (item_place.cpp, 0x6F2B7C60,
//  CItem vtable slot 107), which is where a brand new item enters the
//  world.  Not CItem::CItem (0x6F2B7520): that constructor is on the
//  do-not-hook list - it crashes the game when patched, along with the rest
//  of its base-constructor family (see its BEHAVIOUR entry in funcmap.py),
//  so there is no way to log from the real constructor at all.
//
//  Logged twice per item, because PlaceFromDescriptor mutates the
//  descriptor it is handed: FindFreeWorldPoint may move x/y to a free spot,
//  and m_z is overwritten with the sampled ground height.  "req" is what
//  the caller asked for, "got" is what the item was actually initialised
//  to.
//============================================================================
void FeatureItemDebug_OnItemPlaced(CItem* item, unsigned int footprintType,
                                   const SItemPlacementDescriptor* desc,
                                   int afterPlacement)
{
    //  Raw bits plus the truncated integer, never %f - see the R6002 note
    //  on ITEMDBG_LOG above.  World coordinates run to the thousands, so the
    //  integer is readable enough to follow an item around; the bits next to
    //  it are what is actually stored.
    ITEMDBG_LOG("[itemdbg] item=%08X %s fp=%08X "
                "x=%08X(%d) y=%08X(%d) z=%08X(%d) facing=%08X(%d)\n",
                (unsigned int)item,
                afterPlacement ? "got" : "req",
                footprintType,
                desc->m_x.m_bits,      CFloatToInt(desc->m_x),
                desc->m_y.m_bits,      CFloatToInt(desc->m_y),
                desc->m_z.m_bits,      CFloatToInt(desc->m_z),
                desc->m_facing.m_bits, CFloatToInt(desc->m_facing));

    if (!afterPlacement)
        return;

    //  The rest of what the descriptor initialises the item from - logged
    //  once, on the way out, since PlaceFromDescriptor does not change any
    //  of these.
    //  animScale/timeScale are hardware floats, logged as bits only: they
    //  sit around 1.0, so a truncated integer would say "1" and nothing
    //  more, and %f is not available here at all (see above).
    ITEMDBG_LOG("[itemdbg] item=%08X      owner=%u flags=%08X teamColor=%d "
                "detail=%d animScale=%08X timeScale=%08X field50=%08X\n",
                (unsigned int)item,
                desc->m_owningPlayer, desc->m_widgetFlags, desc->m_teamColor,
                desc->m_detailLevel,
                *(const unsigned int*)&desc->m_animationScale,
                *(const unsigned int*)&desc->m_timeScale,
                desc->m_field50);

    TrackItem(item);
}

//============================================================================
//  FEATURE 2 - F3 gathers every loose item to the hero.
//
//  Called from CTimeSync::Extrapolate (CTimeSync.cpp, 0x6F4885A0), which is
//  the hottest hooked function in the build - it runs for every object whose
//  position is queried, so it is effectively a per-frame poll.  It is a
//  polling site and nothing more; none of its own arguments are used here.
//
//  "the hero" is the local player's currently selected *unit*.  A true hero
//  flag lives in unit-type data this repo has never reconstructed, so the
//  selection is the closest thing reachable.  The route to the selection
//  record is the one CItem::Method_0x188 (item_selected.cpp) establishes and
//  which scores 27/31 against the shipped function - world object -> local
//  player index at +0x28 -> player record -> its selection record - but the
//  slot read at the end is the unit one (+0x1E0), NOT the item one (+0x1E4)
//  that Method_0x188's own getter reads.  See kSelectedUnitOffset above.
//
//  "a free item on the ground" is taken to mean a tracked item whose own
//  handle-bearing sub-object still resolves to a world position.  A carried
//  item is not in the world and has no position to move, so it drops out of
//  the same check that keeps this from faulting.  This is an operational
//  definition, not a reconstructed carried/on-ground flag: no such flag is
//  established anywhere in this repo, and the count of items skipped is
//  logged on every press so the definition can be checked against what the
//  game actually shows.
//============================================================================
typedef SHORT (WINAPI *GetAsyncKeyStateFn)(int vKey);

static GetAsyncKeyStateFn ResolveGetAsyncKeyState()
{
    //  user32 is not linked into the .mix, so this is resolved at runtime.
    //  GetModuleHandleA and not LoadLibraryA: the game is a GUI process, so
    //  user32 is already mapped, and this must not be the thing that loads
    //  a DLL into it.
    static GetAsyncKeyStateFn fn = 0;
    static int tried = 0;
    if (!tried)
    {
        tried = 1;
        HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32)
            fn = (GetAsyncKeyStateFn)GetProcAddress(user32, "GetAsyncKeyState");
        if (!fn)
            ITEMDBG_LOG("[itemdbg] GetAsyncKeyState unavailable - "
                        "F3 teleport disabled\n");
    }
    return fn;
}

static void TeleportLooseItemsToHero()
{
    //  --- where is the hero -------------------------------------------
    void* world = g_unk6FAB65F4;
    if (!world)
        return;

    unsigned int playerIdx = *(const unsigned short*)((const char*)world + 0x28);
    SPlayerRecord* player =
        (SPlayerRecord*)((IndexedArrayHolder_6F3A1650*)world)->At(playerIdx);

    if (!player || !player->m_selection)
    {
        ITEMDBG_LOG("[itemdbg] F3: no local selection record "
                    "(world=%08X playerIdx=%u player=%08X)\n",
                    (unsigned int)world, playerIdx, (unsigned int)player);
        return;
    }

    void* selection = player->m_selection;
    void* unit = SelectionSlot(selection, kSelectedUnitOffset);
    void* selectedItem = SelectionSlot(selection, kSelectedItemOffset);

    if (!unit)
    {
        //  Nothing in the unit slot.  Say which of the two cases it is, and
        //  dump the window the unit/item slots were found in so a further
        //  surprise is diagnosable from one capture instead of another round.
        if (selectedItem)
            ITEMDBG_LOG("[itemdbg] F3: an item (%08X) is selected, not a unit "
                        "- select the hero instead; F3 gathers every item on "
                        "its own, they do not need selecting\n",
                        (unsigned int)selectedItem);
        else
            ITEMDBG_LOG("[itemdbg] F3: nothing selected\n");

        const unsigned int* words =
            (const unsigned int*)((const char*)selection + 0x1C0);
        for (int row = 0; row < 4; ++row)
        {
            ITEMDBG_LOG("[itemdbg] F3: sel+%03X: %08X %08X %08X %08X\n",
                        0x1C0 + row * 16,
                        words[row * 4 + 0], words[row * 4 + 1],
                        words[row * 4 + 2], words[row * 4 + 3]);
        }
        return;
    }

    //  +0x1E0 is an offset read off a live record, so it is checked before
    //  it is trusted - see LooksLikeGameObject.
    if (!LooksLikeGameObject(unit))
    {
        ITEMDBG_LOG("[itemdbg] F3: selected unit slot holds %08X, which does "
                    "not look like a game object - not dereferencing it\n",
                    (unsigned int)unit);
        return;
    }

    CAgent* hero = (CAgent*)unit;

    //  Belt and braces: an item should never appear in the unit slot, but if
    //  the offset is ever wrong again this stops F3 gathering every item onto
    //  one of its own.
    if (IsTrackedItem(hero))
    {
        ITEMDBG_LOG("[itemdbg] F3: unit slot holds tracked item %08X - "
                    "refusing\n", (unsigned int)hero);
        return;
    }

    SHandleWithType* heroSub = ResolvableSubObject(hero);
    if (!heroSub)
    {
        //  Expected for anything that does not override CWidget's slot 46,
        //  and for a selection that is not a positioned world object.
        ITEMDBG_LOG("[itemdbg] F3: selected %08X has no resolvable "
                    "position\n", (unsigned int)hero);
        return;
    }

    CWorldVec3 heroPos = ((const CPathRef*)heroSub)->SmartPtrToWorldVec3();

    //  hero + (5.0, 0), through the software float the simulation uses.
    CFloat targetX = heroPos.m_x + CFloat(kCFloatBits_5_0);
    CFloat targetY = heroPos.m_y;

    ITEMDBG_LOG("[itemdbg] F3: hero=%08X at (%d, %d) -> target (%d, %d) "
                "[bits %08X %08X], %d tracked\n",
                (unsigned int)hero,
                CFloatToInt(heroPos.m_x), CFloatToInt(heroPos.m_y),
                CFloatToInt(targetX), CFloatToInt(targetY),
                targetX.m_bits, targetY.m_bits,
                g_trackedCount);

    //  --- move everything loose ---------------------------------------
    int moved = 0;
    int skipped = 0;
    for (int i = 0; i < g_trackedCount; ++i)
    {
        CItem* item = g_trackedItems[i];
        if (!item || !ResolvableSubObject(item))
        {
            ++skipped;
            continue;
        }

        //  This item's own slot in the grid - see the note on
        //  kCFloatBits_64_0.  Indexed by `moved`, not by `i`, so items that
        //  were skipped do not leave holes in the pile.
        CFloat spacing = CFloat(kCFloatBits_64_0);
        CFloat offsetX = CFloatFromInt(moved % kSpreadColumns) * spacing;
        CFloat offsetY = CFloatFromInt(moved / kSpreadColumns) * spacing;
        CFloat px = targetX + offsetX;
        CFloat py = targetY + offsetY;

        //  Then snap that to somewhere the item may actually rest, exactly
        //  the way CItem::PlaceFromDescriptor does it (item_place.cpp) -
        //  same degenerate one-point box, same thirteen arguments, same
        //  radius, and this item's own placement mask.  px/py are updated
        //  in place when it finds a better spot; a zero return just means
        //  the point it was given is already fine.
        SCellBoxF box(py, px, py, px);
        int placementMask = ((Slot0x164Fn)(*(void***)item)[0x164 / 4])(item);
        FindFreeWorldPoint(&box, &px, &py, &g_slopeThreshold1, placementMask,
                           2, 1, 0, 0, -1, 0, 0, -1);

        //  The same call JASS's own SetItemPosition native ends in
        //  (jasssetitemposition_native.cpp) - it re-runs the whole
        //  placement path (ground sample, sprite and orientation update),
        //  which is what makes the item actually appear somewhere new
        //  rather than just having a field rewritten.
        item->JASS_SetItemPosition_callee1(&px, &py, -1);
        ++moved;

        ITEMDBG_LOG("[itemdbg] F3: moved item=%08X to (%d, %d)\n",
                    (unsigned int)item, CFloatToInt(px), CFloatToInt(py));
    }

    ITEMDBG_LOG("[itemdbg] F3: %d moved, %d skipped (not in world)\n",
                moved, skipped);
}

//----------------------------------------------------------------------------
//  REENTRANCY - the reason this needs a hard guard and not just an edge test.
//
//  The poll site is CTimeSync::Extrapolate, and the teleport reads positions:
//
//      CTimeSync::Extrapolate (hooked)  ->  this poll
//        ->  TeleportLooseItemsToHero
//        ->  CPathRef::SmartPtrToWorldVec3 -> ToWorldVec3
//        ->  CPathTrace::GetTravelDistance   (CPathTrace.cpp)
//        ->  g_pTimeSync->Extrapolate(...)   -- straight back into the hook
//
//  So the teleport re-enters its own poll site, several times per item.  The
//  first version updated `wasDown` *after* the call, which meant every one of
//  those nested polls still saw wasDown==0 / isDown==1 and started the whole
//  teleport again: unbounded recursion, and a C00000FD stack overflow inside
//  GetTravelDistance the moment F3 was pressed with something selected.
//
//  Two things fix it, and both are kept because they fail independently:
//  the edge state is committed *before* the call, and `g_inPoll` refuses any
//  nested entry outright.  The guard is the load-bearing one - it holds even
//  if the key is somehow read as still-down, and it is what makes it safe for
//  anything on this call path to reach Extrapolate again.
//----------------------------------------------------------------------------
static int g_inPoll = 0;

void FeatureItemDebug_PollHotkey()
{
    //  Already inside our own teleport - Extrapolate is on that call path.
    if (g_inPoll)
        return;

    GetAsyncKeyStateFn getKey = ResolveGetAsyncKeyState();
    if (!getKey)
        return;

    //  Edge-triggered.  This runs many times per frame, so a level test
    //  would re-teleport every item on every call for as long as the key is
    //  held - which is both a flood of work and a flood of log lines.
    static int wasDown = 0;
    int isDown = (getKey(VK_F3) & 0x8000) != 0;
    int fire = (isDown && !wasDown);

    //  Committed before the call, not after: see the note above.
    wasDown = isDown;

    if (fire)
    {
        g_inPoll = 1;
        TeleportLooseItemsToHero();
        g_inPoll = 0;
    }
}
