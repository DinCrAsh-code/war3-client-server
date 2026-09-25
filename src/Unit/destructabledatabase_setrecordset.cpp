//============================================================================
//  CDestructableDatabase::SetRecordSet (0x6F260E30) - see
//  destructabledatabase.h for the field layout this wires up.  Same
//  ResolveWEDBFieldIndexOrWarn/RegisterTypedField shape
//  Game/upgradedatabase_setrecordset.cpp already establishes for
//  CUpgradeDatabase's own SetRecordSet (this class shares its field pool
//  helper's own address neighbourhood but not the pool object itself -
//  0x6F71C770, Unit/destructabledatafieldpool_thunk.cpp - a distinct
//  singleton from CUpgradeDatabase's 0x6F71C9B0).
//
//  **Reconstructs the 37-field wiring only - not the ~150-instruction
//  trailing pass.**  Past the 37th field (`Name`, +0x130) the real
//  function does substantially more: it re-reads the record count off
//  `recordSet` (sub_6F4C8780) and walks every row backwards
//  (sub_6F25BD60/sub_6F4C9D20/sub_6F4C8990/sub_6F4C3F50 resolving each
//  row's own key text), fetching that row's `pathTex`/`pathTexDeath`
//  strings through sub_6F25B7E0/sub_6F25B1A0/sub_6F25C250/sub_6F25C270/
//  sub_6F25C080, checking each for a literal `"_"` via Storm_508, calling
//  sub_6F32CF20 up to twice per row with the result, and logging
//  "Bogus destructable SLK data in row %d\n" through sub_6F4CC1E0 for any
//  row sub_6F25C080 rejects - a genuine cross-row "does this destructable
//  have a matching death-texture pair" validation/logging pass, not
//  instruction noise.  Ten further not-yet-reconstructed callees deep,
//  its own separate investigation - out of this batch's scope.  Left
//  unimplemented rather than guessed at; the function returns once the
//  field table is wired, which is a real, understood behavioural gap
//  (a bogus row would go unlogged), not a register/scheduling difference -
//  see this repo's own worktree verdict for the address.
//============================================================================
#include "destructabledatabase.h"

//  citemclosure_wedb_depth7.cpp's own declaration, verbatim - see
//  Item/itemdatabase_setrecordset.cpp's own header note on this call's
//  real, name-ignoring behaviour.
void __cdecl ReleaseWEDBFieldIndexOrWarn();

//  Unit/destructabledatafieldpool_thunk.cpp.
void* __cdecl AcquireDestructableDataFieldPool();

//  Item/registertypedfield_thunk.cpp.
class SFieldRegistrationPool
{
public:
    void __thiscall RegisterTypedField(const char* name, int type,
                                       int zero1, unsigned int* out, int zero2);
};

//----------------------------------------------------------------------------
//  Same shared per-field shape Game/upgradedatabase_setrecordset.cpp's own
//  ResolveUpgradeFieldPair already documents: resolve `name` against
//  `recordSet` (dest1) and against the field-registration pool at literal
//  type 2 - this class uses 2, not CUpgradeDatabase's 6, per the dump's
//  own `push 2` at every field site here.
//----------------------------------------------------------------------------
static void RegisterDestructableField(void* recordSet, void* pool, const char* name,
                                      SDestructableFieldPair* dest)
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

    unsigned int local[5];
    local[0] = 0;
    local[1] = 0;
    local[3] = 0;
    local[2] = 0;
    local[4] = 0;
    ((SFieldRegistrationPool*)pool)->RegisterTypedField(name, 2, 0, local, 0);
    dest->m_registeredValue = local[1];
}

//----------------------------------------------------------------------------
//  0x6F260E30 - vtable slot 2.  Opens with a real `call sub_6F005D30`
//  (CAbilityDatabase::SetRecordSet, Item/abilitydatabase_checksum.cpp -
//  the shared base slot 2 address) - inlined here as the same
//  one-instruction simplification Item/itemdatabase_setrecordset.cpp's
//  own header note documents.  See this file's own header note for the
//  trailing validation pass this reconstruction does not cover.
//----------------------------------------------------------------------------
void __thiscall CDestructableDatabase::SetRecordSet(void* recordSet)
{
    m_recordSet = recordSet;   // 0x6F005D30's own store, inlined - see header note

    void* pool = AcquireDestructableDataFieldPool();

    RegisterDestructableField(m_recordSet, pool, "DestructableID", &m_DestructableID);
    RegisterDestructableField(m_recordSet, pool, "dir",            &m_dir);
    RegisterDestructableField(m_recordSet, pool, "file",           &m_file);
    RegisterDestructableField(m_recordSet, pool, "targType",       &m_targType);
    RegisterDestructableField(m_recordSet, pool, "armor",          &m_armor);
    RegisterDestructableField(m_recordSet, pool, "HP",             &m_HP);
    RegisterDestructableField(m_recordSet, pool, "lightweight",    &m_lightweight);
    RegisterDestructableField(m_recordSet, pool, "fatLOS",         &m_fatLOS);
    RegisterDestructableField(m_recordSet, pool, "numVar",         &m_numVar);
    RegisterDestructableField(m_recordSet, pool, "maxPitch",       &m_maxPitch);
    RegisterDestructableField(m_recordSet, pool, "maxRoll",        &m_maxRoll);
    RegisterDestructableField(m_recordSet, pool, "radius",         &m_radius);
    RegisterDestructableField(m_recordSet, pool, "fogRadius",      &m_fogRadius);
    RegisterDestructableField(m_recordSet, pool, "fogVis",         &m_fogVis);
    RegisterDestructableField(m_recordSet, pool, "pathTex",        &m_pathTex);
    RegisterDestructableField(m_recordSet, pool, "pathTexDeath",   &m_pathTexDeath);
    RegisterDestructableField(m_recordSet, pool, "deathSnd",       &m_deathSnd);
    RegisterDestructableField(m_recordSet, pool, "shadow",         &m_shadow);
    RegisterDestructableField(m_recordSet, pool, "texID",          &m_texID);
    RegisterDestructableField(m_recordSet, pool, "texFile",        &m_texFile);
    RegisterDestructableField(m_recordSet, pool, "occH",           &m_occH);
    RegisterDestructableField(m_recordSet, pool, "flyH",           &m_flyH);
    RegisterDestructableField(m_recordSet, pool, "walkable",       &m_walkable);
    RegisterDestructableField(m_recordSet, pool, "cliffHeight",    &m_cliffHeight);
    RegisterDestructableField(m_recordSet, pool, "fixedRot",       &m_fixedRot);
    RegisterDestructableField(m_recordSet, pool, "goldRep",        &m_goldRep);
    RegisterDestructableField(m_recordSet, pool, "lumberRep",      &m_lumberRep);
    RegisterDestructableField(m_recordSet, pool, "buildTime",      &m_buildTime);
    RegisterDestructableField(m_recordSet, pool, "repairTime",     &m_repairTime);
    RegisterDestructableField(m_recordSet, pool, "colorR",         &m_colorR);
    RegisterDestructableField(m_recordSet, pool, "colorG",         &m_colorG);
    RegisterDestructableField(m_recordSet, pool, "colorB",         &m_colorB);
    RegisterDestructableField(m_recordSet, pool, "version",        &m_version);
    RegisterDestructableField(m_recordSet, pool, "selectable",     &m_selectable);
    RegisterDestructableField(m_recordSet, pool, "selcircsize",    &m_selcircsize);
    RegisterDestructableField(m_recordSet, pool, "portraitmodel",  &m_portraitmodel);
    RegisterDestructableField(m_recordSet, pool, "Name",           &m_Name);

    //  Trailing cross-row Name-uniqueness validation/logging pass -
    //  not reconstructed this batch, see this file's own header note.
}
