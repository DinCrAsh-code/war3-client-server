//============================================================================
//  CGameWar3 - the "game" checksum provider (array slot 6, offset 0x18, of
//  the per-thread CGameDataContext's m_pCache - see gamecontext.h and
//  docs/notes/checksum-provider-registry.md) and, independently, the
//  per-game player-slot table/"game world" singleton this repo's
//  GameUI/worldframelocalplayer.cpp and friends already read through
//  g_unk6FAB65F4 - both are the SAME object.  Allocated and constructed by
//  AcquireGameSingleton (GameUI/acquiregamesingleton.cpp) the first time
//  anything asks the registry for index 6, and cached at
//  CGameDataContext::m_pCache[6] / g_unk6FAB65F4 from then on.
//
//  Layout is read straight off its own constructor (0x6F3AF3B0,
//  ".\CGameWar3.cpp" line 0x209/0x20A/0x20B in its own three Storm_401
//  allocation tags) and cross-checked against the *serializer* method
//  (vtable slot 4, 0x6F3A6C70) which walks nearly every field in order -
//  the two agree everywhere they overlap.  Only what either one actually
//  touches is named; the rest is reserved.
//
//  sizeof(CGameWar3) == 0x408, matching AcquireGameSingleton's own
//  SMemAlloc(0x408, ...) exactly.
//============================================================================
#ifndef GAMEWAR3_H
#define GAMEWAR3_H

#include "rcstring.h"
#include "agenttypedslots.h"

class CDataStoreScratch;
class CDataStore;

//----------------------------------------------------------------------------
//  The +0xAC array's element (12 of them, 0x2C bytes each).  The
//  constructor (0x6F3A4620) zeroes ten of the eleven dwords and seeds the
//  eleventh (+0x20) to -1; the destructor (0x6F3A27B0) frees +0x10 through
//  Storm_403 if it is non-null, tagged with a `.?AU...` RTTI-name-shaped
//  string this call tree never reads past the free call, so the element's
//  own C++ name (an "ally" or per-player relation record, matching the
//  '12' element count seen nowhere else in the object) is left as an
//  offset rather than guessed.
//----------------------------------------------------------------------------
struct AllySlot
{
    AllySlot()
        : m_field00(0), m_field04(0), m_field08(0), m_field0C(0),
          m_field10(0), m_field14(0), m_field18(0), m_field1C(0),
          m_field20(-1), m_field24(0), m_field28(0)
    {
    }

    //  0x6F3A27B0 - retn 0.  Frees m_field10 (a Storm_401 block, per its
    //  own allocation tag) if it was ever set; nothing in this call tree
    //  ever sets it, so the free is real but never exercised here.
    ~AllySlot();

    int   m_field00;
    int   m_field04;
    int   m_field08;
    int   m_field0C;
    void* m_field10;             // +0x10 - Storm_401 block, freed by ~AllySlot
    int   m_field14;
    int   m_field18;
    int   m_field1C;
    int   m_field20;             // +0x20 - seeded -1
    int   m_field24;
    int   m_field28;
};

//----------------------------------------------------------------------------
//  CGameWar3 itself.
//----------------------------------------------------------------------------
class CGameWar3
{
public:
    //  0x6F3AF3B0.  SEH-framed (__CxxFrameHandler3-shaped, the same
    //  unreproducible ceiling documented throughout this repo for a
    //  constructor with more than one sub-object that can throw): the
    //  happy-path field-by-field construction below is the whole
    //  reconstructible instruction stream.
    CGameWar3();

    //  Slot 0 - 0x6F3A1690.  The "game checksum" itself: fold every one of
    //  the twelve player records' own vtable-slot-0 hash
    //  (0x6F40FEB0, xor-rotate-3 into a running value seeded 0xC) into one
    //  32-bit result, then let this table's own vtable slot 0
    //  (0x6F406CF0, on m_field34) contribute last.  This is the function
    //  GameDataContextChecksumProviderHash (Config/miscdata.cpp) ends up
    //  calling through the checksum-provider array's slot 6 - see
    //  docs/notes/checksum-provider-registry.md, "still open" section,
    //  now answered.
    unsigned int ComputeChecksum();

    //  0x6F39F5B0 - `retn 4`.  Jass/jassresolvefunctionhandle.cpp.  Resolve
    //  a JASS function name to its (instance-scoped, memoized) handle via
    //  whichever JassInstance m_currentJassInstanceSlot currently names.
    //  ExecuteFunc's own direct callee.
    unsigned int ResolveJassFunctionHandle(const char* funcName);

    //  Slot 2 - 0x6F3A8CE0.  `retn 4`: `(flags & 1) && this` gates a
    //  Storm_403("delete", -1, 0) free of `this`; either way `this` (the
    //  original receiver, not the freed pointer - the shipped body reads
    //  `esi` back, not `eax`) is the return value.  sub_6F3A7E00 (called
    //  unconditionally first, side-effect only, return value unused
    //  here) is out of scope for this batch and stays a naked thunk.
    void* ReleaseIfFlagged(unsigned int flags);

    //  Slot 1 - 0x6F3A16D0.  Debug dump: three fixed-format lines through
    //  Storm_548 (the player-to-team map header, the player count, and
    //  this record's own localPlayerId), then one call through each live
    //  player record's own vtable slot 0x20 (offset 0x80) with `logHandle`
    //  as its one argument, then one more through m_field34's own vtable
    //  slot 1 (offset 4).  Nothing here is checksum-relevant; the vtable
    //  slot count made this batch's own worklist call it out as a sibling
    //  worth finishing alongside slot 0/2/3/4/5 rather than as a checksum
    //  contributor.
    void DumpPlayerDebugLog(void* logHandle);

    //  Slot 3 - 0x6F3A1740.  Formats "War3Build: %d.%d.%d.%d\n" (the
    //  literal build quad 1.26.0.6401 this image was stamped with) into a
    //  0x104-byte stack buffer via Storm_578, then Storm_503's that into
    //  the caller's own `dest`/`maxLen`.  m_field30's own vtable slot 0
    //  gets one call (`dest`, `maxLen`), then every live player record's
    //  own vtable slot 0x1F (offset 0x7C) gets the same two arguments.
    //  Carries the game's own `/GS` stack cookie
    //  (`dword_6FAAE140 xor esp` / `sub_6F7E1059` at the tail) around that
    //  buffer - this build's fixed `/GS-` (pipeline/CLAUDE.md) can never
    //  emit that pair, the same permanent ceiling already documented for
    //  Jass/jassscripterrorformat.cpp's own sibling function.
    void FormatWar3BuildLog(char* dest, unsigned int maxLen);

    //  Slot 4 - 0x6F3A6C70.  Walks (almost) every field CGameWar3's own
    //  constructor sets, writing each through the established
    //  CDataStoreScratch writer family (cdatastorescratch.h) - the network-
    //  sync save side of this class, as opposed to ComputeChecksum's own
    //  read-only fold.  See gamewar3save.cpp for the field-by-field
    //  mapping and which sub-calls this session reconstructed for real
    //  versus left as naked thunks (the m_field20 hash-table walk, the
    //  AllySlot element writer, and the two RCString-array/GameUI-field
    //  fragments near the tail - all out of scope for the checksum family
    //  itself).
    void Save(CDataStoreScratch* stream);

    //  Slot 5 - 0x6F3B0B70.  Save's read-side counterpart: the same field
    //  walk through CDataStore's own reader family, version-gated near the
    //  tail (GetVersion() against three literal build thresholds) for the
    //  fields that were added to the format after CGameWar3's own initial
    //  release.  See gamewar3load.cpp.  Two real field discoveries came
    //  out of tracing this body: `this+0x338` (previously modelled as
    //  untouched reserved padding) is a real dword this function either
    //  reads straight off the stream or, for a pre-0x177E-version stream,
    //  derives from a legacy per-race lookup (sub_6F3A28B0, out of scope,
    //  thunked); and `this+0x32C` is unconditionally stamped to 1 here,
    //  even though the constructor itself zeroes it - i.e. it is a
    //  "has this record been loaded" flag CGameWar3's own scratch
    //  construction path never sets, only Load does.
    void Load(CDataStore* stream);

    void*          m_vtable;              // +0x00
    unsigned int   m_field04;             // +0x04 = 0
    //  +0x08 - the JASS thread-instance slot index ExecuteFunc
    //  (Jass/jassruntimeexecutefunc.cpp) reads via `[this+8]` and hands to
    //  LookupInstanceBySlot to find "the JassInstance the calling thread is
    //  currently running in".  Zeroed by the constructor (slot 0 - the
    //  main/default JASS thread).  **Write site found**
    //  (jass-instance-targeting-D): `sub_6F3AEF80` (CGameWar3's own map-
    //  (re)load driver, `.\CGameWar3.cpp`'s own module per its callee
    //  sub_6F3AEB20's Storm_401 tags, still TODO/unreconstructed - see
    //  docs/notes/jass-runtime-instance-targeting.md) is the only writer
    //  this repo's own closure has found: right after its own
    //  `LoadAndCompileCommonScripts` call, it feeds that call's return
    //  value into `GetOrRunJassInstanceForScript`
    //  (Jass/jassgetorrunscriptinstance.cpp) and stores THAT function's
    //  own return value (a freshly-assigned JassThreadLocal instance-handle
    //  slot index, from a brand-new root JassInstance
    //  `GetOrRunJassInstanceForScript` just constructed) straight into this
    //  field.  Whether anything else re-points it for a spawned JASS
    //  thread (as opposed to the single main-thread CGameWar3 singleton
    //  this closure covers) is still unconfirmed.
    unsigned int   m_currentJassInstanceSlot; // +0x08 = 0
    unsigned int   m_field0C;             // +0x0C = 0
    unsigned int   m_field10;             // +0x10 = 0
    unsigned int   m_field14;             // +0x14 = 0
    unsigned int   m_field18;             // +0x18 = 0
    void*          m_gameSlotValue;       // +0x1C - a SCheckedGameSlot-shaped value; see
                                          //  CGameWar3_ConstructField1C (gamewar3ctorthunks.cpp)
    void*          m_field20;             // +0x20 - lazily-allocated 0x144-byte block
                                          //  (CGameWar3_ConstructField20): two
                                          //  TAllocatorHashTable<IdStringReg,HASHKEY_4CC,256>
                                          //  vftable stamps at its own +0x00/+0x11C
    unsigned int   m_buildConstant;       // +0x24 = 0x17AB - the same build constant
                                          //  CNetData::FireTickNotification XORs the
                                          //  "net" checksum leg with (netcommand_dispatch.cpp)
    unsigned short m_field28;             // +0x28 = 0x10
    char           m_reserved2A[2];
    unsigned short m_field2C;             // +0x2C = 0
    char           m_reserved2E[2];
    void*          m_field30;             // +0x30 - lazily-allocated 0x64-byte block
                                          //  (CGameWar3_ConstructField30)
    void*          m_field34;             // +0x34 - lazily-allocated 0x8C-byte block
                                          //  (CGameWar3_ConstructField34).  The same
                                          //  field GameUI/worldframelocalplayer.cpp's
                                          //  own IndexedArrayHolder_6F3A1650 view already
                                          //  names m_field34 and reads as
                                          //  `m_field188 = table->m_field34` - kept
                                          //  consistent with that name here
    RCString       m_field38;             // +0x38, 12 bytes - Construct(0) in the ctor body
    unsigned int   m_field44;             // +0x44 = 0xC
    unsigned int   m_field48;             // +0x48 = 0
    unsigned int   m_field4C;             // +0x4C = 0
    unsigned int   m_field50;             // +0x50 = 0xC
    unsigned int   m_field54;             // +0x54 = 0x10
    SCheckedPlayerSlot m_playerSlots[16]; // +0x58..+0x98
    unsigned int   m_field98;             // +0x98 = 0
    unsigned int   m_field9C;             // +0x9C = 0
    unsigned int   m_fieldA0;             // +0xA0 = 0
    unsigned int   m_fieldA4;             // +0xA4 = 0
    unsigned int   m_fieldA8;             // +0xA8 = 0xC
    AllySlot m_allySlots[12];    // +0xAC..+0x2BC
    //  +0x2BC = 0xC in the constructor - the count over the +0x2C0 array
    //  below (12 elements at 4 bytes each, matching the array's own 0x30
    //  bytes).  Neither field is set together at construction (the array is
    //  never zeroed - genuinely uninitialised until Load populates it), but
    //  Save (0x6F3A6C70) walks `m_field2C0[0..m_field2BC)` unconditionally
    //  and Load (0x6F3B0B70) reads the same range back when the stream's
    //  version is >= 0x11DC.
    unsigned int   m_field2BC;            // +0x2BC = 0xC
    unsigned int   m_field2C0[(0x2F0 - 0x2C0) / 4];   // +0x2C0, 12 dwords -
                                                       //  uninitialised by
                                                       //  the ctor; see above
    RCString       m_field2F0;            // +0x2F0, 12 bytes
    RCString       m_field2FC;            // +0x2FC, 12 bytes
    RCString       m_field308;            // +0x308, 12 bytes
    RCString       m_field314;            // +0x314, 12 bytes
    unsigned int   m_field320;            // = 0
    unsigned int   m_field324;            // = 0
    unsigned int   m_field328;            // = 0
    //  = 0 in the constructor, but Load (slot 5, 0x6F3B0B70) unconditionally
    //  stamps this to 1 after reading the stream's version - a "this record
    //  has been loaded" flag the ctor's own scratch-construction path never
    //  sets.
    unsigned int   m_field32C;            // = 0, Load() stamps 1
    char           m_reserved330[4];      // untouched by the ctor
    unsigned int   m_field334;            // = 0
    //  = 0 in the constructor (never explicitly written), but both Save and
    //  Load touch it for real: Save (0x6F3A6C70) reads it unconditionally;
    //  Load (0x6F3B0B70) either reads it straight off a >=0x177E-version
    //  stream or, for an older one, derives it from a legacy per-race
    //  lookup (sub_6F3A28B0, thunked - out of scope for this batch).  Not
    //  reserved padding; left untyped past "a dword" since neither call
    //  site names what it means.
    unsigned int   m_field338;            // Save/Load touch; ctor leaves 0
    unsigned short m_field33C;            // = 0
    char           m_reserved33E[0x3C0 - 0x33E];
    unsigned int   m_field3C0;            // +0x3C0 = 0 - the same offset
                                          //  worldframelocalplayer.cpp's own view already
                                          //  names m_field3C0
    unsigned int   m_field3C4;            // = 0
    unsigned int   m_field3C8;            // = 0
    RCString       m_field3CC;            // +0x3CC, 12 bytes
    unsigned int   m_field3D8;            // = 0
    unsigned int   m_field3DC;            // = 0
    unsigned int   m_field3E0;            // = 0
    unsigned int   m_field3E4;            // = 0
    unsigned int   m_field3E8;            // = 0
    unsigned int   m_field3EC;            // = 0
    unsigned int   m_field3F0;            // = 0
    unsigned int   m_field3F4;            // = 0
    unsigned int   m_field3F8;            // = 0
    unsigned int   m_field3FC;            // = 0
    void*          m_field400;            // +0x400 - constructed via
                                          //  CGameWar3_ConstructField400
    unsigned int   m_field404;            // = 0
};

//----------------------------------------------------------------------------
//  The three field constructors sub_6F3A18B0/sub_6F3AE870/sub_6F3A0D80/
//  sub_6F408C00/sub_6F3A1910 that CGameWar3's own constructor calls are
//  each themselves SEH-framed (43-78 instructions apiece, one full
//  __CxxFrameHandler3 frame each) and were dumped this session but not yet
//  reconstructed - dumped bodies are in asm/, `retn` bytes below read off
//  each one's own epilogue.  Declared here as naked thunks so
//  CGameWar3::CGameWar3 itself can be written and scored for real; a
//  follow-up session's next step for this class.
//----------------------------------------------------------------------------
//  0x6F3A18B0 - `this`=&CGameWar3::m_gameSlotValue (as void*), one stack
//  arg (always 0 from this caller).  `retn 4`.
void* __fastcall CGameWar3_ConstructField1C(void* field, int candidate);
//  0x6F3AE870 - `this`=freshly Storm_401'd 0x144-byte block.  `retn 0`,
//  returns `this` (MSVC's own constructor-return convention).
void* __fastcall CGameWar3_ConstructField20(void* block);
//  0x6F3A0D80 - `this`=freshly Storm_401'd 0x64-byte block.  `retn 0`.
void* __fastcall CGameWar3_ConstructField30(void* block);
//  0x6F408C00 - `this`=freshly Storm_401'd 0x8C-byte block.  `retn 0`.
void* __fastcall CGameWar3_ConstructField34(void* block);
//  0x6F3A1910 - `this`=&CGameWar3::m_field400, one stack arg (always 0).
//  `retn 4`.
void* __fastcall CGameWar3_ConstructField400(void* field, int candidate);

#endif
