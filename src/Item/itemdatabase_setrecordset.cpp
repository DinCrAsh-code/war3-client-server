//============================================================================
//  CItemDatabase::SetRecordSet (0x6F25F120) - the vtable slot 2 override
//  this class's own header (itemdatabase.h) documents, plus the two small
//  helpers underneath it this batch traced while reading it:
//
//    0x6F25C560 - RegisterItemField: resolve one named field against
//    `recordSet` and against the per-thread CItemCustomData pool, writing
//    both results into the caller's own field-pair slot.
//    0x6F2519B0 - QueryCustomFieldViaPool: the pool half of that, a thin
//    forward into the shared RegisterTypedField engine (0x6F7127A0,
//    Item/registertypedfield_thunk.cpp) sourcing its pool/type from the
//    CItemCustomData object itself.
//
//  **`ReleaseWEDBFieldIndexOrWarn`'s own `name` argument is real in the
//  dump but dead in the callee.**  Misc/citemclosure_wedb_depth7.cpp's own
//  header note (and this repo's earlier reconstruction of 0x6F4CA130,
//  confirmed by re-reading its raw disassembly this batch) says what it
//  actually does: releases a WEDB field index off `recordSet+0x10` and
//  falls back to a second release path, neither of which reads the name
//  string this call site loads into `edx` - almost certainly two
//  originally-distinct functions the linker's identical-code-folding
//  merged under one address (`ReleaseWEDBFieldIndexOrWarn`'s own dump
//  never touches `edx` after the `push edx` that reserves its stack
//  slot).  Reproduced here exactly as shipped regardless: `edx` is loaded
//  with the field name immediately before the call, matching the dump,
//  even though the callee that runs discards it.  `ReleaseWEDBFieldIndexOrWarn`
//  is declared `__cdecl` with no parameters (its own file's own
//  no-live-call-site convention), so the `ecx`/`edx` setup the shipped
//  call site needs is written in-line rather than through a mismatched
//  C++ signature - the same technique
//  Item/abilitygenericfieldrecord.cpp's own SFieldNameSource::ResolveFormatted
//  already uses for the identical symbol.
//============================================================================
#include "itemdatabase.h"
#include "customdatasiblings.h"

//  citemclosure_wedb_depth7.cpp's own declaration, verbatim - not a second
//  name for 0x6F4CA130, just visibility into this TU (see this file's own
//  header note on its real, name-ignoring behaviour).
void __cdecl ReleaseWEDBFieldIndexOrWarn();

//  Item/getitemcustomdata.cpp.
CItemCustomData* GetOrCreateItemCustomData();

//  Item/registertypedfield_thunk.cpp - see that file for the real shape.
class SFieldRegistrationPool
{
public:
    void __thiscall RegisterTypedField(const char* name, int type,
                                       int zero1, unsigned int* out, int zero2);
};

//----------------------------------------------------------------------------
//  0x6F2519B0 - `retn 4`.  `this` (ecx) is the CItemCustomData pool
//  object; its own +0x04 (m_type) and +0x0C (m_pPool) - CCustomData's own
//  layout, Item/abilitycustomdata.h - become RegisterTypedField's type and
//  receiver.  Accessed here through a raw same-layout view rather than
//  CCustomData itself: that class keeps both fields `private`
//  (Item/abilitycustomdata.h), reachable only from its own member
//  functions, and this is a free function operating on an opaque pool
//  object from outside that hierarchy.
//----------------------------------------------------------------------------
struct SCustomDataRawView
{
    void*        m_vfptr;
    unsigned int m_type;      // +0x04
    void*        m_ownedRef;  // +0x08
    void*        m_pPool;     // +0x0C
};

unsigned int QueryCustomFieldViaPool(CItemCustomData* customData, const char* name)
{
    SCustomDataRawView* raw = (SCustomDataRawView*)customData;
    unsigned int local[5];
    local[0] = 0;
    local[1] = 0;
    local[3] = 0;
    local[2] = 0;
    local[4] = 0;

    ((SFieldRegistrationPool*)raw->m_pPool)->RegisterTypedField(
        name, (int)raw->m_type, 0, local, 0);

    return local[1];
}

//----------------------------------------------------------------------------
//  0x6F25C560 - `retn 0Ch`.  RegisterItemField(recordSet, &destPair, name).
//----------------------------------------------------------------------------
void RegisterItemField(void* recordSet, SItemDatabaseFieldPair* dest, const char* name)
{
    void* wedbResult;
    __asm
    {
        mov ecx, recordSet
        mov edx, name
        call ReleaseWEDBFieldIndexOrWarn
        mov wedbResult, eax
    }
    dest->m_wedbResult = wedbResult;

    CItemCustomData* customData = GetOrCreateItemCustomData();
    dest->m_registeredValue = QueryCustomFieldViaPool(customData, name);
}

//----------------------------------------------------------------------------
//  0x6F25F120 - vtable slot 2.  The dump's own first instruction is a real
//  `call sub_6F005D30` (CAbilityDatabase::SetRecordSet,
//  Item/abilitydatabase_checksum.cpp - the identical shared-base address
//  CWar3SlkDatabase's own slot 2 uses everywhere else in this family),
//  called directly rather than virtually - `ecx` is still `this`,
//  unmodified since entry.  That whole function is a one-instruction
//  `this[1] = recordSet` store, so it is inlined here rather than routed
//  through a second C++ name bound to the same address (the "one shipped
//  address, two C++ names" defect class CLAUDE.md warns against) - a
//  deliberate, one-instruction simplification: the store is behaviourally
//  identical, it just costs this function the one `call`/`retn` pair
//  against the dump's own instruction count.  Then wires up this class's
//  own 33 named SLK fields, one RegisterItemField call per field, in dump
//  order.
//----------------------------------------------------------------------------
void __thiscall CItemDatabase::SetRecordSet(void* recordSet)
{
    m_recordSet = recordSet;   // 0x6F005D30's own store, inlined - see header note

    RegisterItemField(m_recordSet, &m_itemID,     "itemID");
    RegisterItemField(m_recordSet, &m_scriptname, "scriptname");
    RegisterItemField(m_recordSet, &m_file,       "file");
    RegisterItemField(m_recordSet, &m_uses,       "uses");
    RegisterItemField(m_recordSet, &m_goldcost,   "goldcost");
    RegisterItemField(m_recordSet, &m_lumbercost, "lumbercost");
    RegisterItemField(m_recordSet, &m_stockMax,   "stockMax");
    RegisterItemField(m_recordSet, &m_usable,     "usable");
    RegisterItemField(m_recordSet, &m_perishable, "perishable");
    RegisterItemField(m_recordSet, &m_droppable,  "droppable");
    RegisterItemField(m_recordSet, &m_powerup,    "powerup");
    RegisterItemField(m_recordSet, &m_sellable,   "sellable");
    RegisterItemField(m_recordSet, &m_pawnable,   "pawnable");
    RegisterItemField(m_recordSet, &m_drop,       "drop");
    RegisterItemField(m_recordSet, &m_HP,         "HP");
    RegisterItemField(m_recordSet, &m_stockRegen, "stockRegen");
    RegisterItemField(m_recordSet, &m_stockStart, "stockStart");
    RegisterItemField(m_recordSet, &m_targType,   "targType");
    RegisterItemField(m_recordSet, &m_armor,      "armor");
    RegisterItemField(m_recordSet, &m_prio,       "prio");
    RegisterItemField(m_recordSet, &m_abilList,   "abilList");
    RegisterItemField(m_recordSet, &m_Level,      "Level");
    RegisterItemField(m_recordSet, &m_morph,      "morph");
    RegisterItemField(m_recordSet, &m_pickRandom, "pickRandom");
    RegisterItemField(m_recordSet, &m_version,    "version");
    RegisterItemField(m_recordSet, &m_oldLevel,   "oldLevel");
    RegisterItemField(m_recordSet, &m_class,      "class");
    RegisterItemField(m_recordSet, &m_cooldownID, "cooldownID");
    RegisterItemField(m_recordSet, &m_ignoreCD,   "ignoreCD");
    RegisterItemField(m_recordSet, &m_scale,      "scale");
    RegisterItemField(m_recordSet, &m_colorR,     "colorR");
    RegisterItemField(m_recordSet, &m_colorG,     "colorG");
    RegisterItemField(m_recordSet, &m_colorB,     "colorB");
}
