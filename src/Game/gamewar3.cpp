//============================================================================
//  CGameWar3's own module (".\CGameWar3.cpp" per its own Storm_401
//  allocation tags): the constructor and two of its six vtable slots.
//  See gamewar3.h for the class layout and the checksum-provider
//  identification.
//============================================================================
#include "gamewar3.h"
#include "storm.h"

AllySlot::~AllySlot()
{
    if (m_field10)
        SMemFree(m_field10, ".?AUAllySlot@@", -2, 0);
}

//  0x6F3AF3B0.
CGameWar3::CGameWar3()
{
    m_vtable = (void*)0x6F941B10;   // ??_7CGameWar3@@6B@ - vtable not fully
                                     // reconstructed (slots 1/3/4/5 still
                                     // thunked), so this stores the shipped
                                     // address rather than our own -
                                     // ctor_vtable_audit.py's own rule.

    m_field04 = 0;
    m_currentJassInstanceSlot = 0;
    m_field0C = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;

    m_gameSlotValue = CGameWar3_ConstructField1C(&m_gameSlotValue, 0);

    m_field38.Construct(0);

    m_buildConstant = 0x17AB;
    m_field28       = 0x10;
    m_field2C       = 0;
    m_field30       = 0;
    m_field34       = 0;

    m_field44 = 0xC;
    m_field48 = 0;
    m_field4C = 0;
    m_field50 = 0xC;
    m_field54 = 0x10;
    //  m_playerSlots[16] default-constructs here (SCheckedPlayerSlot(0) x16,
    //  through the compiler's own eh-vector-constructor-iterator, matching
    //  the shipped 0x6F3A1A30 wrapper).

    m_field98 = 0;
    m_field9C = 0;
    m_fieldA0 = 0;
    m_fieldA4 = 0;

    m_fieldA8 = 0xC;
    //  m_allySlots[12] default-constructs here (AllySlot() x12, through the
    //  same compiler machinery, matching the shipped 0x6F3A4620 wrapper).
    m_field2BC = 0xC;

    m_field2F0.Construct(0);
    m_field2FC.Construct(0);
    m_field308.Construct(0);
    m_field314.Construct(0);

    m_field320 = 0;
    m_field324 = 0;
    m_field328 = 0;
    m_field32C = 0;
    m_field334 = 0;
    m_field33C = 0;

    m_field3C0 = 0;
    m_field3C4 = 0;
    m_field3C8 = 0;
    m_field3CC.Construct(0);
    m_field3D8 = 0;
    m_field3DC = 0;
    m_field3E0 = 0;
    m_field3E4 = 0;
    m_field3E8 = 0;
    m_field3EC = 0;
    m_field3F0 = 0;
    m_field3F4 = 0;
    m_field3F8 = 0;
    m_field3FC = 0;

    m_field400 = CGameWar3_ConstructField400(&m_field400, 0);
    m_field404 = 0;

    void* block20 = SMemAlloc(0x144, ".\\CGameWar3.cpp", 0x209, 0);
    m_field20 = block20 ? CGameWar3_ConstructField20(block20) : 0;

    void* block30 = SMemAlloc(0x64, ".\\CGameWar3.cpp", 0x20A, 0);
    m_field30 = block30 ? CGameWar3_ConstructField30(block30) : 0;

    void* block34 = SMemAlloc(0x8C, ".\\CGameWar3.cpp", 0x20B, 0);
    m_field34 = block34 ? CGameWar3_ConstructField34(block34) : 0;
}

//  0x6F40FEB0 - one player record's own vtable-slot-0 checksum
//  contribution.  Real reconstruction now - see
//  Player/playerrecordchecksumhash.cpp.
unsigned int __fastcall PlayerRecordChecksumHash(void* record);

//  0x6F406CF0 - m_field34's own vtable slot 0.  Out of scope; naked thunk.
extern "C" unsigned int __fastcall Field34ChecksumHash(void* field34);

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed
//  (Misc/misc_field_getters.cpp).  CGameWar3 *is* that same holder - this
//  file declares its own narrow view rather than pull in the header that
//  names it, the same way every other caller of At() in this repo does.
struct IndexedArrayHolder_6F3A1650
{
    int At(unsigned int index);
};

//  Slot 0 - 0x6F3A1690.  ebx (the running hash) starts at 0xC and is only
//  updated for a live player record - each contributes
//  `rol(hash ^ PlayerRecordChecksumHash(record), 3)`.  m_field34's own
//  checksum (Field34ChecksumHash) is called last but its result is
//  discarded: `mov eax, ebx` happens *after* that call, so the shipped
//  return value is the twelve-record fold alone.
unsigned int CGameWar3::ComputeChecksum()
{
    unsigned int hash = 0xC;

    for (unsigned int i = 0; i < 0xC; ++i)
    {
        int record = ((IndexedArrayHolder_6F3A1650*)this)->At(i);
        if (record)
        {
            hash ^= PlayerRecordChecksumHash((void*)record);
            hash = (hash << 3) | (hash >> 29);
        }
    }

    Field34ChecksumHash(m_field34);   // side effect only; its result is discarded

    return hash;
}

//  0x6F3A7E00 - out of scope; naked thunk.
extern "C" void __fastcall CGameWar3_SideEffect7E00(void* self);

//  Slot 2 - 0x6F3A8CE0.  The `this` null test after the flag test is the
//  shipped code's own second, separate test (`test esi,esi`) - not
//  reachable from source in a member function that is already executing,
//  but written explicitly here because that is exactly the two-test shape
//  the listing has.
void* CGameWar3::ReleaseIfFlagged(unsigned int flags)
{
    CGameWar3_SideEffect7E00(this);

    if (flags & 1)
    {
        if (this)
            SMemFree(this, "delete", -1, 0);
    }

    return this;
}

//  0x6F6EB5FA - Storm ordinal 548, a printf-shaped log line: buffer,
//  format, varargs.  See Misc/cheatdata.cpp's own declaration comment -
//  matched verbatim (no `extern "C"`, plain default calling convention) so
//  this stays the same C++ symbol as that file's own declaration.
int __cdecl Storm_548(char* buffer, const char* format, ...);

//  Slot 1 - 0x6F3A16D0.  See gamewar3.h's own doc comment.  `logHandle`
//  plays Storm_548's own first (buffer) parameter at all three call sites -
//  confirmed from the push order (`push offset "...text"`; `push
//  logHandle`; arguments go right to left, so `logHandle` is Storm_548's
//  first parameter, the format strings its second).  Each live player
//  record's own vtable slot 0x20 (offset 0x80) takes `logHandle` as its one
//  argument; the record's own class is not established anywhere in this
//  repo, so the dispatch is written as a raw slot call rather than a
//  guessed method name, the same pattern CLAUDE.md's own house rules show
//  for an unnamed vtable slot.
typedef void (__thiscall *PlayerRecordDebugDumpFn)(void* record, void* logHandle);

void CGameWar3::DumpPlayerDebugLog(void* logHandle)
{
    Storm_548((char*)logHandle, "Player to team map:\n");
    Storm_548((char*)logHandle, "Player count: %d\n", 0xC);
    Storm_548((char*)logHandle, "Log for player with localPlayerId %d\n",
              m_field28);

    for (unsigned int i = 0; i < 0xC; ++i)
    {
        int record = ((IndexedArrayHolder_6F3A1650*)this)->At(i);
        if (record)
            ((PlayerRecordDebugDumpFn)(*(void***)record)[0x20])((void*)record,
                                                                  logHandle);
    }

    ((PlayerRecordDebugDumpFn)(*(void***)m_field34)[1])(m_field34, logHandle);
}

//  0x6F6EB5A6 / 0x6F6EB5BE - Storm ordinals 578/503.  Storm_578 is the
//  variadic sized-format writer already declared throughout GameUI/ (see
//  chatbarcolonmessage.cpp - matched verbatim); Storm_503 is the plain
//  bounded string copy already declared in Game/gamemain.cpp (matched
//  verbatim too).
int  Storm_578(char* dest, unsigned int size, const char* format, ...);
void __stdcall Storm_503(char* dest, const char* src, unsigned int maxLen);

//  Slot 3 - 0x6F3A1740.  See gamewar3.h's own doc comment.  m_field30's own
//  vtable slot 0 and each live player record's own vtable slot 0x1F
//  (offset 0x7C) both take (dest, maxLen) - two raw slot dispatches for
//  the same reason DumpPlayerDebugLog's is one: no established class name
//  for either receiver.
typedef void (__thiscall *AppendBuildLogFn)(void* self, char* dest, unsigned int maxLen);

void CGameWar3::FormatWar3BuildLog(char* dest, unsigned int maxLen)
{
    //  This build's own /GS- flags cannot reproduce the cookie
    //  (`dword_6FAAE140 xor esp` on entry, `SecurityCheckCookie` on exit)
    //  the shipped body carries around this buffer - see the class's own
    //  header comment and Jass/jassscripterrorformat.cpp's identical gap.
    char buffer[0x104];

    Storm_578(buffer, sizeof(buffer), "War3Build: %d.%d.%d.%d\n",
              1, 0x1A, 0, 0x1901);
    Storm_503(dest, buffer, maxLen);

    if (m_field30)
        ((AppendBuildLogFn)(*(void***)m_field30)[0])(m_field30, dest, maxLen);

    for (unsigned int i = 0; i < 0xC; ++i)
    {
        int record = ((IndexedArrayHolder_6F3A1650*)this)->At(i);
        if (record)
            ((AppendBuildLogFn)(*(void***)record)[0x1F])((void*)record, dest,
                                                           maxLen);
    }
}
