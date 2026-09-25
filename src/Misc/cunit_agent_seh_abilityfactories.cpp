//============================================================================
//  A second batch of "make a fresh hidden-ability CAgent" factories, from
//  this agent's own slice of the CUnit vtable BFS closure: 20 arrived
//  flagged "same 78-instruction __except_handler4-shaped SEH template,
//  unreproducible ceiling" - which turned out to mean exactly what
//  cunit_agent8_abilityfactories.cpp already found for its own 39: this is
//  the same family, instruction-for-instruction, with three addresses
//  substituted per instance - a type-checked slot constructor, a rawcode
//  getter (called twice), and that slot's own Assign - over the same four
//  shared calls (AGILE_TYPE_ID::Hash 0x6F4C8520, TSHashTable::Ptr
//  0x6F001EC0, InitWidgetAgentQuery 0x6F472C20, SubmitWidgetAgentQuery
//  0x6F48AB80) and the same g_pGameData global (0x6FAB7368). See that file
//  for the full reasoning; this one just supplies the next 20 addresses via
//  a macro instead of writing each out longhand.
//
//  A 21st (0x6F06EF20) is the same SEH-shaped family but calls its rawcode
//  getter only once, and a 22nd (0x6F280F60) carries the *same*
//  __except_handler4-shaped SEH frame despite having no destructible local
//  of its own to unwind - it is the plain, generic checked-slot constructor
//  itself (`m_value = 0; Assign(candidate);`), and MSVC still wraps a
//  constructor body in this frame whenever the body it protects (Assign,
//  reached through a call the compiler cannot prove won't throw) runs
//  before the object is fully constructed - see each one's own header
//  comment below.
//
//  Every rawcode getter is again a plain `mov eax, <imm32> / retn` leaf and
//  every one of the 21 factory instances decodes to a real four-character,
//  'A'-first ability rawcode. As before, the type-checked slot's own
//  ctor/Assign (and each getter) are naked `mov eax,<addr>/jmp eax`
//  redirects into the real shipped functions, defined at the bottom of
//  this file - none of those addresses are in this agent's own worklist
//  slice, so there is nothing to reconstruct from here, only something to
//  link against (link_check.py).
//
//  Same unreproducible __except_handler4-shaped SEH frame every one of
//  these 22 carries (docs/msvc-vc8-idioms.md) - this build's fixed
//  /GS- /EHs-c- cannot reach that frame shape, so every instance here is
//  capped well short of EXACT by frame bytes alone.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  Same private, per-TU redeclaration as cunit_agent8_abilityfactories.cpp -
//  only the CAgent* at +0x54 is reached.
namespace {
struct SMadeAgentHolder2
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};
}

#define MAKE_ABILITY_AGENT_FACTORY(fourcc, addr, ctorAddr, assignAddr, getterAddr, byte4) \
    struct SCheckedAbilitySlot_##fourcc \
    { \
        SCheckedAbilitySlot_##fourcc(CAgent* candidate);          /* ctorAddr */ \
        SCheckedAbilitySlot_##fourcc* Assign(CAgent* candidate);  /* assignAddr */ \
        void Release() \
        { \
            CAgent* held = m_value; \
            if (held) \
            { \
                if (--held->m_refcount == 0) \
                    held->ReleaseSelf(); \
            } \
        } \
        ~SCheckedAbilitySlot_##fourcc() { Release(); } \
        CAgent* m_value;   /* +0x00 */ \
    }; \
    unsigned int GetFourCC_##getterAddr(); \
    CAgent* MakeAbilityAgent_##fourcc() \
    { \
        SCheckedAbilitySlot_##fourcc held(0); \
        unsigned int typeId = GetFourCC_##getterAddr(); \
        CGameData* game = g_pGameData; \
        AGILE_TYPE_ID id(typeId); \
        unsigned int hash = id.Hash(); \
        AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id); \
        void* pool = type->m_allocator; \
        SWidgetAgentQuery query; \
        InitWidgetAgentQuery(&query, GetFourCC_##getterAddr(), pool); \
        held.Assign(((SMadeAgentHolder2*) \
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent); \
        return held.m_value; \
    }

//  0x6F238D00 - MakeAbilityAgent_Aroc.  Ctor 0x6F221000, Assign 0x6F212200,
//  rawcode getter 0x6F15AE10 ('Aroc' = 0x41726F63).
MAKE_ABILITY_AGENT_FACTORY(Aroc, 0x6F238D00, 0x6F221000, 0x6F212200, 6F15AE10, 0x41726F63)

//  0x6F23ABE0 - MakeAbilityAgent_Assp.  Ctor 0x6F221C00, Assign 0x6F213200,
//  rawcode getter 0x6F080350 ('Assp' = 0x41737370).
MAKE_ABILITY_AGENT_FACTORY(Assp, 0x6F23ABE0, 0x6F221C00, 0x6F213200, 6F080350, 0x41737370)

//  0x6F23C1D0 - MakeAbilityAgent_Afra.  Ctor 0x6F222500, Assign 0x6F213E00,
//  rawcode getter 0x6F15A990 ('Afra' = 0x41667261).
MAKE_ABILITY_AGENT_FACTORY(Afra, 0x6F23C1D0, 0x6F222500, 0x6F213E00, 6F15A990, 0x41667261)

//  0x6F23C920 - MakeAbilityAgent_Agyv.  Ctor 0x6F222800, Assign 0x6F214200,
//  rawcode getter 0x6F024A10 ('Agyv' = 0x41677976).
MAKE_ABILITY_AGENT_FACTORY(Agyv, 0x6F23C920, 0x6F222800, 0x6F214200, 6F024A10, 0x41677976)

//  0x6F23CFA0 - MakeAbilityAgent_Ainf.  Ctor 0x6F222AA0, Assign 0x6F214580,
//  rawcode getter 0x6F10F500 ('Ainf' = 0x41696E66).
MAKE_ABILITY_AGENT_FACTORY(Ainf, 0x6F23CFA0, 0x6F222AA0, 0x6F214580, 6F10F500, 0x41696E66)

//  0x6F23D620 - MakeAbilityAgent_Amim.  Ctor 0x6F0D4B80, Assign 0x6F0D2D50,
//  rawcode getter 0x6F1104D0 ('Amim' = 0x416D696D).
MAKE_ABILITY_AGENT_FACTORY(Amim, 0x6F23D620, 0x6F0D4B80, 0x6F0D2D50, 6F1104D0, 0x416D696D)

//  0x6F23DD70 - MakeAbilityAgent_Amil.  Ctor 0x6F092520, Assign 0x6F084870,
//  rawcode getter 0x6F111E60 ('Amil' = 0x416D696C).
MAKE_ABILITY_AGENT_FACTORY(Amil, 0x6F23DD70, 0x6F092520, 0x6F084870, 6F111E60, 0x416D696C)

//  0x6F23E9A0 - MakeAbilityAgent_Apiv.  Ctor 0x6F223520, Assign 0x6F215380,
//  rawcode getter 0x6F10F700 ('Apiv' = 0x41706976).
MAKE_ABILITY_AGENT_FACTORY(Apiv, 0x6F23E9A0, 0x6F223520, 0x6F215380, 6F10F700, 0x41706976)

//  0x6F23F020 - MakeAbilityAgent_Aps2.  Ctor 0x6F223760, Assign 0x6F215680,
//  rawcode getter 0x6F1992C0 ('Aps2' = 0x41707332).
MAKE_ABILITY_AGENT_FACTORY(Aps2, 0x6F23F020, 0x6F223760, 0x6F215680, 6F1992C0, 0x41707332)

//  0x6F23F6A0 - MakeAbilityAgent_Arel.  Ctor 0x6F223A60, Assign 0x6F215A80,
//  rawcode getter 0x6F0DCF20 ('Arel' = 0x4172656C).
MAKE_ABILITY_AGENT_FACTORY(Arel, 0x6F23F6A0, 0x6F223A60, 0x6F215A80, 6F0DCF20, 0x4172656C)

//  0x6F23FD20 - MakeAbilityAgent_Arst.  Ctor 0x6F223D00, Assign 0x6F215E00,
//  rawcode getter 0x6F028000 ('Arst' = 0x41727374).
MAKE_ABILITY_AGENT_FACTORY(Arst, 0x6F23FD20, 0x6F223D00, 0x6F215E00, 6F028000, 0x41727374)

//  0x6F2403A0 - MakeAbilityAgent_Alam.  Ctor 0x6F0924C0, Assign 0x6F0847F0,
//  rawcode getter 0x6F1D3E00 ('Alam' = 0x416C616D).
MAKE_ABILITY_AGENT_FACTORY(Alam, 0x6F2403A0, 0x6F0924C0, 0x6F0847F0, 6F1D3E00, 0x416C616D)

//  0x6F240BC0 - MakeAbilityAgent_Aspo.  Ctor 0x6F224360, Assign 0x6F216680,
//  rawcode getter 0x6F113AD0 ('Aspo' = 0x4173706F).
MAKE_ABILITY_AGENT_FACTORY(Aspo, 0x6F240BC0, 0x6F224360, 0x6F216680, 6F113AD0, 0x4173706F)

//  0x6F2414B0 - MakeAbilityAgent_Aste.  Ctor 0x6F224720, Assign 0x6F216B80,
//  rawcode getter 0x6F07C050 ('Aste' = 0x41737465).
MAKE_ABILITY_AGENT_FACTORY(Aste, 0x6F2414B0, 0x6F224720, 0x6F216B80, 6F07C050, 0x41737465)

//  0x6F241C00 - MakeAbilityAgent_Atau.  Ctor 0x6F224A80, Assign 0x6F217000,
//  rawcode getter 0x6F1567F0 ('Atau' = 0x41746175).
MAKE_ABILITY_AGENT_FACTORY(Atau, 0x6F241C00, 0x6F224A80, 0x6F217000, 6F1567F0, 0x41746175)

//  0x6F242350 - MakeAbilityAgent_Awan.  Ctor 0x6F224D80, Assign 0x6F217400,
//  rawcode getter 0x6F159350 ('Awan' = 0x4177616E).
MAKE_ABILITY_AGENT_FACTORY(Awan, 0x6F242350, 0x6F224D80, 0x6F217400, 6F159350, 0x4177616E)

//  0x6F243AE0 - MakeAbilityAgent_AHca.  Ctor 0x6F225740, Assign 0x6F218100,
//  rawcode getter 0x6F199E50 ('AHca' = 0x41486361).
MAKE_ABILITY_AGENT_FACTORY(AHca, 0x6F243AE0, 0x6F225740, 0x6F218100, 6F199E50, 0x41486361)

//  0x6F246930 - MakeAbilityAgent_AEsv.  Ctor 0x6F226CA0, Assign 0x6F219D80,
//  rawcode getter 0x6F1D7680 ('AEsv' = 0x41457376).
MAKE_ABILITY_AGENT_FACTORY(AEsv, 0x6F246930, 0x6F226CA0, 0x6F219D80, 6F1D7680, 0x41457376)

//  0x6F248C20 - MakeAbilityAgent_AIsm.  Ctor 0x6F227C60, Assign 0x6F21B280,
//  rawcode getter 0x6F0DCC20 ('AIsm' = 0x4149736D).
MAKE_ABILITY_AGENT_FACTORY(AIsm, 0x6F248C20, 0x6F227C60, 0x6F21B280, 6F0DCC20, 0x4149736D)

//  0x6F24BA70 - MakeAbilityAgent_AIsa.  Ctor 0x6F229160, Assign 0x6F21CE80,
//  rawcode getter 0x6F1D67C0 ('AIsa' = 0x41497361).
MAKE_ABILITY_AGENT_FACTORY(AIsa, 0x6F24BA70, 0x6F229160, 0x6F21CE80, 6F1D67C0, 0x41497361)

#undef MAKE_ABILITY_AGENT_FACTORY


//----------------------------------------------------------------------------
//  0x6F06EF20 - MakeAbilityAgent_Afui.  Same family, same shape, but the
//  rawcode getter (sub_6F026E60, 'Afui' = 0x41667569) is called only
//  *once* here - the shipped body keeps the first result in a
//  callee-saved register and reuses it for InitWidgetAgentQuery's own
//  rawcode argument instead of calling the getter a second time. That is
//  a genuine source-level difference from the other 59 (a local
//  `typeId` reused vs. `GetFourCC_X()` written twice at both call sites),
//  not a codegen artifact - so it gets a real second call shape instead of
//  the shared macro above. Ctor 0x6F03F820, Assign 0x6F02D480 - both naked
//  redirects, defined at the bottom of this file, same convention as the
//  macro'd 20 (neither address is in this agent's own worklist slice).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Afui
{
    SCheckedAbilitySlot_Afui(CAgent* candidate);          //  0x6F03F820
    SCheckedAbilitySlot_Afui* Assign(CAgent* candidate);  //  0x6F02D480

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Afui() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F026E60();   //  0x6F026E60 - 'Afui'

CAgent* MakeAbilityAgent_Afui()
{
    SCheckedAbilitySlot_Afui held(0);

    unsigned int typeId = GetFourCC_6F026E60();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, pool);
    held.Assign(((SMadeAgentHolder2*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//----------------------------------------------------------------------------
//  0x6F280F60 - a bare, generic checked-slot constructor: zero the held
//  pointer, then hand the candidate straight to this slot's own Assign
//  (0x6F2757B0 - not this agent's own slice; its own first call is
//  GetFourCC_6F0278B0, already named in misc_trivial_getters.cpp, 'Aque').
//  Carries the identical push -1/SEH-offset/fs:0 cookie __except_handler4
//  frame the other 21 in this file do, even though this constructor has no
//  destructible local of its own: the frame is protecting partially-
//  constructed `this` across the call to Assign, which the compiler cannot
//  prove will not throw, the same reasoning docs/msvc-vc8-idioms.md gives
//  for the family's factories. This build's fixed /GS- /EHs-c- cannot
//  reach that frame shape, so the body below - logically an exact,
//  complete reconstruction - is capped well short of EXACT by frame bytes
//  alone, same as the rest of this file.
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Aque
{
    SCheckedAbilitySlot_Aque(CAgent* candidate);
    SCheckedAbilitySlot_Aque* Assign(CAgent* candidate);  //  0x6F2757B0

    CAgent* m_value;   // +0x00
};

SCheckedAbilitySlot_Aque::SCheckedAbilitySlot_Aque(CAgent* candidate)
{
    m_value = 0;
    Assign(candidate);
}


//============================================================================
//  Out-of-slice bodies, as naked `mov eax,<addr>/jmp eax` redirects into the
//  real, unhooked shipped functions - same convention
//  cunit_agent8_abilityfactories.cpp already uses for its own 39 ctor/
//  Assign/getter triples (see e.g. its SCheckedAbilitySlot_Aabs::
//  SCheckedAbilitySlot_Aabs at 0x6F220A00). None of these addresses are in
//  this agent's own worklist slice, so there is nothing to reconstruct from
//  here; the thunk only exists so this translation unit's own 22 addresses
//  above have something real to call and the .mix actually links
//  (link_check.py). Each is marked THUNK in agent_worktrees, not DIFFERS -
//  a thunk has no behaviour of its own to compare against its own address.
//  retn sizes below are read straight off each target's own `raw_bytes`
//  (all thiscall ctor/Assign pairs retn 4; all rawcode getters retn 0),
//  never inferred from the declaration - the same rule
//  thunk_abi_audit.py's own docs give.
//============================================================================
__declspec(naked) SCheckedAbilitySlot_Aroc::SCheckedAbilitySlot_Aroc(CAgent*)
{
    __asm { mov eax, 06F221000h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aroc* SCheckedAbilitySlot_Aroc::Assign(CAgent*)
{
    __asm { mov eax, 06F212200h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15AE10()
{
    __asm { mov eax, 06F15AE10h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Assp::SCheckedAbilitySlot_Assp(CAgent*)
{
    __asm { mov eax, 06F221C00h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Assp* SCheckedAbilitySlot_Assp::Assign(CAgent*)
{
    __asm { mov eax, 06F213200h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F080350()
{
    __asm { mov eax, 06F080350h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afra::SCheckedAbilitySlot_Afra(CAgent*)
{
    __asm { mov eax, 06F222500h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Afra* SCheckedAbilitySlot_Afra::Assign(CAgent*)
{
    __asm { mov eax, 06F213E00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15A990()
{
    __asm { mov eax, 06F15A990h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Agyv::SCheckedAbilitySlot_Agyv(CAgent*)
{
    __asm { mov eax, 06F222800h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Agyv* SCheckedAbilitySlot_Agyv::Assign(CAgent*)
{
    __asm { mov eax, 06F214200h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F024A10()
{
    __asm { mov eax, 06F024A10h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ainf::SCheckedAbilitySlot_Ainf(CAgent*)
{
    __asm { mov eax, 06F222AA0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Ainf* SCheckedAbilitySlot_Ainf::Assign(CAgent*)
{
    __asm { mov eax, 06F214580h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F10F500()
{
    __asm { mov eax, 06F10F500h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amim::SCheckedAbilitySlot_Amim(CAgent*)
{
    __asm { mov eax, 06F0D4B80h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Amim* SCheckedAbilitySlot_Amim::Assign(CAgent*)
{
    __asm { mov eax, 06F0D2D50h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1104D0()
{
    __asm { mov eax, 06F1104D0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amil::SCheckedAbilitySlot_Amil(CAgent*)
{
    __asm { mov eax, 06F092520h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Amil* SCheckedAbilitySlot_Amil::Assign(CAgent*)
{
    __asm { mov eax, 06F084870h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F111E60()
{
    __asm { mov eax, 06F111E60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apiv::SCheckedAbilitySlot_Apiv(CAgent*)
{
    __asm { mov eax, 06F223520h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Apiv* SCheckedAbilitySlot_Apiv::Assign(CAgent*)
{
    __asm { mov eax, 06F215380h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F10F700()
{
    __asm { mov eax, 06F10F700h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aps2::SCheckedAbilitySlot_Aps2(CAgent*)
{
    __asm { mov eax, 06F223760h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aps2* SCheckedAbilitySlot_Aps2::Assign(CAgent*)
{
    __asm { mov eax, 06F215680h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1992C0()
{
    __asm { mov eax, 06F1992C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arel::SCheckedAbilitySlot_Arel(CAgent*)
{
    __asm { mov eax, 06F223A60h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arel* SCheckedAbilitySlot_Arel::Assign(CAgent*)
{
    __asm { mov eax, 06F215A80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DCF20()
{
    __asm { mov eax, 06F0DCF20h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arst::SCheckedAbilitySlot_Arst(CAgent*)
{
    __asm { mov eax, 06F223D00h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arst* SCheckedAbilitySlot_Arst::Assign(CAgent*)
{
    __asm { mov eax, 06F215E00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F028000()
{
    __asm { mov eax, 06F028000h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Alam::SCheckedAbilitySlot_Alam(CAgent*)
{
    __asm { mov eax, 06F0924C0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Alam* SCheckedAbilitySlot_Alam::Assign(CAgent*)
{
    __asm { mov eax, 06F0847F0h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D3E00()
{
    __asm { mov eax, 06F1D3E00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aspo::SCheckedAbilitySlot_Aspo(CAgent*)
{
    __asm { mov eax, 06F224360h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aspo* SCheckedAbilitySlot_Aspo::Assign(CAgent*)
{
    __asm { mov eax, 06F216680h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F113AD0()
{
    __asm { mov eax, 06F113AD0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aste::SCheckedAbilitySlot_Aste(CAgent*)
{
    __asm { mov eax, 06F224720h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aste* SCheckedAbilitySlot_Aste::Assign(CAgent*)
{
    __asm { mov eax, 06F216B80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F07C050()
{
    __asm { mov eax, 06F07C050h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atau::SCheckedAbilitySlot_Atau(CAgent*)
{
    __asm { mov eax, 06F224A80h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Atau* SCheckedAbilitySlot_Atau::Assign(CAgent*)
{
    __asm { mov eax, 06F217000h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1567F0()
{
    __asm { mov eax, 06F1567F0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Awan::SCheckedAbilitySlot_Awan(CAgent*)
{
    __asm { mov eax, 06F224D80h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Awan* SCheckedAbilitySlot_Awan::Assign(CAgent*)
{
    __asm { mov eax, 06F217400h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F159350()
{
    __asm { mov eax, 06F159350h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AHca::SCheckedAbilitySlot_AHca(CAgent*)
{
    __asm { mov eax, 06F225740h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AHca* SCheckedAbilitySlot_AHca::Assign(CAgent*)
{
    __asm { mov eax, 06F218100h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F199E50()
{
    __asm { mov eax, 06F199E50h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AEsv::SCheckedAbilitySlot_AEsv(CAgent*)
{
    __asm { mov eax, 06F226CA0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AEsv* SCheckedAbilitySlot_AEsv::Assign(CAgent*)
{
    __asm { mov eax, 06F219D80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D7680()
{
    __asm { mov eax, 06F1D7680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIsm::SCheckedAbilitySlot_AIsm(CAgent*)
{
    __asm { mov eax, 06F227C60h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AIsm* SCheckedAbilitySlot_AIsm::Assign(CAgent*)
{
    __asm { mov eax, 06F21B280h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DCC20()
{
    __asm { mov eax, 06F0DCC20h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIsa::SCheckedAbilitySlot_AIsa(CAgent*)
{
    __asm { mov eax, 06F229160h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AIsa* SCheckedAbilitySlot_AIsa::Assign(CAgent*)
{
    __asm { mov eax, 06F21CE80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D67C0()
{
    __asm { mov eax, 06F1D67C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afui::SCheckedAbilitySlot_Afui(CAgent*)
{
    __asm { mov eax, 06F03F820h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Afui* SCheckedAbilitySlot_Afui::Assign(CAgent*)
{
    __asm { mov eax, 06F02D480h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F026E60()
{
    __asm { mov eax, 06F026E60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aque* SCheckedAbilitySlot_Aque::Assign(CAgent*)
{
    __asm { mov eax, 06F2757B0h }
    __asm { jmp eax }
}
