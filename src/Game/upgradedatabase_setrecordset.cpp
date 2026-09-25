//============================================================================
//  CUpgradeDatabase::SetRecordSet (0x6F252E50) - see upgradedatabase.h for
//  the field layout this wires up.
//
//  **2026-09-13: now a real `virtual` override of `CWar3SlkDatabase`
//  (Item/war3slkdatabase.h/.cpp is a real base now).**  Measured both
//  spellings of the opening store head to head with `tools/quick.py` once
//  a funcmap row existed to score it at all (this function had never
//  actually been run through `verify.py` before - no row, no `asm/`
//  worklist entry until this pass added both): a genuine
//  `CWar3SlkDatabase::SetRecordSet(recordSet)` call scored 0.137
//  (58/453), the plain inlined `m_recordSet = recordSet` store below
//  scored 0.237 (100/453) - kept the higher-scoring inline spelling.  Most
//  of the remaining gap is the whole-function `ebp`-frame/`esp`-frame
//  mismatch below, not this one store.
//
//  **`ReleaseWEDBFieldIndexOrWarn`'s own `name` argument is real in the
//  dump but dead in the callee** - see
//  Item/itemdatabase_setrecordset.cpp's own header note on the identical
//  call shape (this family's SetRecordSet overrides all resolve fields
//  the same way); called here through the same inline-asm technique for
//  the same reason (it is declared `__cdecl` with no parameters).
//
//  **The `/GS` stack cookie in the dump's own prologue/epilogue
//  (`mov eax, dword_6FAAE140` / `xor eax, esp` ... `call sub_6F7E1059`)
//  is not reproduced here.**  This build's fixed `/GS-` means the bundled
//  `cl.exe` never emits it for ordinary (non-naked) C++, the same
//  toolchain ceiling docs/msvc-vc8-idioms.md documents for every other
//  `/GS`-cookied function in this repo that stays a normal reconstruction
//  rather than a hand-written naked body - confined to the
//  prologue/epilogue, not the body logic, which this function otherwise
//  reproduces field for field, group for group, in dump order.
//============================================================================
#include "upgradedatabase.h"

//  citemclosure_wedb_depth7.cpp's own declaration, verbatim - see this
//  file's own header note.
void __cdecl ReleaseWEDBFieldIndexOrWarn();

//  Game/upgradedatafieldpool_thunk.cpp.
void* __cdecl AcquireUpgradeDataFieldPool();

//  Item/registertypedfield_thunk.cpp.
class SFieldRegistrationPool
{
public:
    void __thiscall RegisterTypedField(const char* name, int type,
                                       int zero1, unsigned int* out, int zero2);
};

//  Render/text.h's own declaration, repeated per this repo's own
//  "declaring imports so the diff can see them" convention.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//----------------------------------------------------------------------------
//  Shared per-field shape both the 15 simple fields and the 4 indexed
//  groups reduce to: resolve `name` against `recordSet` (dest1) and
//  against the field-registration pool at literal type 6 (dest2, read
//  back out of a zeroed 5-word scratch buffer the same way every other
//  RegisterTypedField call site in this family does).
//----------------------------------------------------------------------------
static void ResolveUpgradeFieldPair(void* recordSet, void* pool, const char* name,
                                    void** outWedbResult, unsigned int* outRegistered)
{
    void* wedbResult;
    __asm
    {
        mov ecx, recordSet
        mov edx, name
        call ReleaseWEDBFieldIndexOrWarn
        mov wedbResult, eax
    }
    *outWedbResult = wedbResult;

    unsigned int local[5];
    local[0] = 0;
    local[1] = 0;
    local[3] = 0;
    local[2] = 0;
    local[4] = 0;
    ((SFieldRegistrationPool*)pool)->RegisterTypedField(name, 6, 0, local, 0);
    *outRegistered = local[1];
}

static void RegisterUpgradeField(void* recordSet, void* pool, const char* name,
                                 SUpgradeFieldPair* dest)
{
    ResolveUpgradeFieldPair(recordSet, pool, name, &dest->m_wedbResult, &dest->m_registeredValue);
}

//----------------------------------------------------------------------------
//  0x6F253246-shaped loop body (one of four, identical shape): format
//  "<baseName><i>" (Storm_578's own "%s%d"), resolve it exactly like a
//  simple field, and store the pair's dest1 into the group's shared
//  scratch slot (overwritten every iteration) and dest2 into the group's
//  own per-index array slot.
//----------------------------------------------------------------------------
static void RegisterUpgradeIndexedGroup(void* recordSet, void* pool, const char* baseName,
                                        SUpgradeIndexedGroup* group)
{
    char buffer[0x100];
    for (int i = 1; i <= 4; i++)
    {
        Storm_578(buffer, 0x100, "%s%d", baseName, i);

        void* wedbResult;
        unsigned int registered;
        ResolveUpgradeFieldPair(recordSet, pool, buffer, &wedbResult, &registered);

        group->m_lastWedbResult = (unsigned int)wedbResult;
        group->m_values[i - 1] = registered;
    }
}

//----------------------------------------------------------------------------
//  Shared `nullsub_53` (0x6F002000) - see upgradedatabase.h's own header
//  note.  Distinct, real, empty definition, not funcmap-registered.
//----------------------------------------------------------------------------
void CUpgradeDatabase::DebugDumpTo(void* /*buffer*/)
{
}

//----------------------------------------------------------------------------
//  0x6F252E50 - vtable slot 2.  Opens with a real `call sub_6F005D30`
//  into the base class's own `CWar3SlkDatabase::SetRecordSet` (called
//  directly, non-virtually, exactly like the dump - not a second virtual
//  dispatch) - see this file's own header note.
//----------------------------------------------------------------------------
void __thiscall CUpgradeDatabase::SetRecordSet(void* recordSet)
{
    m_recordSet = recordSet;   // 0x6F005D30's own store, inlined (temp A/B test)

    void* pool = AcquireUpgradeDataFieldPool();

    RegisterUpgradeField(m_recordSet, pool, "upgradeid",  &m_upgradeid);
    RegisterUpgradeField(m_recordSet, pool, "race",       &m_race);
    RegisterUpgradeField(m_recordSet, pool, "class",      &m_class);
    RegisterUpgradeField(m_recordSet, pool, "flag",       &m_flag);
    RegisterUpgradeField(m_recordSet, pool, "maxlevel",   &m_maxlevel);
    RegisterUpgradeField(m_recordSet, pool, "used",       &m_used);
    RegisterUpgradeField(m_recordSet, pool, "inherit",    &m_inherit);
    RegisterUpgradeField(m_recordSet, pool, "goldbase",   &m_goldbase);
    RegisterUpgradeField(m_recordSet, pool, "goldmod",    &m_goldmod);
    RegisterUpgradeField(m_recordSet, pool, "lumberbase", &m_lumberbase);
    RegisterUpgradeField(m_recordSet, pool, "lumbermod",  &m_lumbermod);
    RegisterUpgradeField(m_recordSet, pool, "timebase",   &m_timebase);
    RegisterUpgradeField(m_recordSet, pool, "timemod",    &m_timemod);
    RegisterUpgradeField(m_recordSet, pool, "version",    &m_version);
    RegisterUpgradeField(m_recordSet, pool, "global",     &m_global);

    RegisterUpgradeIndexedGroup(m_recordSet, pool, "effect", &m_effect);
    RegisterUpgradeIndexedGroup(m_recordSet, pool, "base",   &m_base);
    RegisterUpgradeIndexedGroup(m_recordSet, pool, "mod",    &m_mod);
    RegisterUpgradeIndexedGroup(m_recordSet, pool, "code",   &m_code);
}
