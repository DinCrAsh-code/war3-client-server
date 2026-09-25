//============================================================================
//  A third batch of "make a fresh hidden-ability CAgent" factories, same
//  family cunit_agent8_abilityfactories.cpp and
//  cunit_agent_seh_abilityfactories.cpp already catalogue in full: this
//  agent's own slice of the JASS_UnitAddAbility closure (0x6F3C82A0, part 1)
//  turned up 21 more instances of the identical 78-instruction
//  __except_handler4-shaped SEH template - three addresses substituted per
//  instance (a type-checked slot constructor, a rawcode getter called
//  twice, and that slot's own Assign) over the same four shared calls
//  (AGILE_TYPE_ID::Hash 0x6F4C8520, TSHashTable::Ptr 0x6F001EC0,
//  InitWidgetAgentQuery 0x6F472C20, SubmitWidgetAgentQuery 0x6F48AB80) and
//  the same g_pGameData global (0x6FAB7368). See
//  cunit_agent8_abilityfactories.cpp for the full reasoning; this file
//  supplies the next 20 addresses via the same macro
//  cunit_agent_seh_abilityfactories.cpp already defines its own private
//  copy of, plus a 21st (0x6F13A740) that calls its rawcode getter only
//  once - the same "typeId kept in a callee-saved register and reused"
//  shape as that file's own 0x6F06EF20/Afui.
//
//  Every rawcode getter is again a plain `mov eax, <imm32> / retn` leaf and
//  every one of the 21 factory instances decodes to a real four-character,
//  'A'-first ability rawcode - all confirmed against each getter's own
//  agent_worktrees raw_asm (not the shipped body itself, which stays a
//  self-redirect thunk here per the naked-getter reasoning below). The
//  type-checked slot's own ctor/Assign (and each getter) are naked
//  `mov eax,<addr>/jmp eax` redirects into the real shipped functions,
//  defined at the bottom of this file - none of those addresses are in this
//  agent's own worklist slice, so there is nothing to reconstruct from here,
//  only something to link against (link_check.py).
//
//  Same unreproducible __except_handler4-shaped SEH frame every one of
//  these 21 carries (docs/msvc-vc8-idioms.md) - this build's fixed
//  /GS- /EHs-c- cannot reach that frame shape, so every instance here is
//  capped well short of EXACT by frame bytes alone.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  Same private, per-TU redeclaration as the other two files in this
//  family - only the CAgent* at +0x54 is reached.
namespace {
struct SMadeAgentHolder3
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};
}

#define MAKE_ABILITY_AGENT_FACTORY2(fourcc, addr, ctorAddr, assignAddr, getterAddr) \
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
        held.Assign(((SMadeAgentHolder3*) \
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent); \
        return held.m_value; \
    }

//  0x6F238820 - MakeAbilityAgent_Aasl.  Ctor 0x6F220DC0, Assign 0x6F211F00,
//  rawcode getter 0x6F1961A0 ('Aasl' = 0x4161736C).
MAKE_ABILITY_AGENT_FACTORY2(Aasl, 0x6F238820, 0x6F220DC0, 0x6F211F00, 6F1961A0)

//  0x6F239D40 - MakeAbilityAgent_Aclf.  Ctor 0x6F16A250, Assign 0x6F15D9D0,
//  rawcode getter 0x6F199750 ('Aclf' = 0x41636C66).
MAKE_ABILITY_AGENT_FACTORY2(Aclf, 0x6F239D40, 0x6F16A250, 0x6F15D9D0, 6F199750)

//  0x6F23AB10 - MakeAbilityAgent_Aspd.  Ctor 0x6F221BA0, Assign 0x6F213180,
//  rawcode getter 0x6F0802D0 ('Aspd' = 0x41737064).
MAKE_ABILITY_AGENT_FACTORY2(Aspd, 0x6F23AB10, 0x6F221BA0, 0x6F213180, 6F0802D0)

//  0x6F23BDC0 - MakeAbilityAgent_Aflk.  Ctor 0x6F222380, Assign 0x6F213C00,
//  rawcode getter 0x6F15AA10 ('Aflk' = 0x41666C6B).
MAKE_ABILITY_AGENT_FACTORY2(Aflk, 0x6F23BDC0, 0x6F222380, 0x6F213C00, 6F15AA10)

//  0x6F23C100 - MakeAbilityAgent_Afrz.  Ctor 0x6F16A190, Assign 0x6F15D8D0,
//  rawcode getter 0x6F15B210 ('Afrz' = 0x4166727A).
MAKE_ABILITY_AGENT_FACTORY2(Afrz, 0x6F23C100, 0x6F16A190, 0x6F15D8D0, 6F15B210)

//  0x6F23C5E0 - MakeAbilityAgent_Agld.  Ctor 0x6F03D6B0, Assign 0x6F02C190,
//  rawcode getter 0x6F0824F0 ('Agld' = 0x41676C64).
MAKE_ABILITY_AGENT_FACTORY2(Agld, 0x6F23C5E0, 0x6F03D6B0, 0x6F02C190, 6F0824F0)

//  0x6F23C850 - MakeAbilityAgent_Agyb.  Ctor 0x6F2227A0, Assign 0x6F214180,
//  rawcode getter 0x6F15A690 ('Agyb' = 0x41677962).
MAKE_ABILITY_AGENT_FACTORY2(Agyb, 0x6F23C850, 0x6F2227A0, 0x6F214180, 6F15A690)

//  0x6F23CC60 - MakeAbilityAgent_Anhe.  Ctor 0x6F222920, Assign 0x6F214380,
//  rawcode getter 0x6F0DA980 ('Anhe' = 0x416E6865).
MAKE_ABILITY_AGENT_FACTORY2(Anhe, 0x6F23CC60, 0x6F222920, 0x6F214380, 6F0DA980)

//  0x6F23CED0 - MakeAbilityAgent_Aimp.  Ctor 0x6F222A40, Assign 0x6F214500,
//  rawcode getter 0x6F15A790 ('Aimp' = 0x41696D70).
MAKE_ABILITY_AGENT_FACTORY2(Aimp, 0x6F23CED0, 0x6F222A40, 0x6F214500, 6F15A790)

//  0x6F23D2E0 - MakeAbilityAgent_Alit.  Ctor 0x6F222C20, Assign 0x6F214780,
//  rawcode getter 0x6F10FC80 ('Alit' = 0x416C6974).
MAKE_ABILITY_AGENT_FACTORY2(Alit, 0x6F23D2E0, 0x6F222C20, 0x6F214780, 6F10FC80)

//  0x6F23D550 - MakeAbilityAgent_Amdf.  Ctor 0x6F222CE0, Assign 0x6F214880,
//  rawcode getter 0x6F07C7C0 ('Amdf' = 0x416D6466).
MAKE_ABILITY_AGENT_FACTORY2(Amdf, 0x6F23D550, 0x6F222CE0, 0x6F214880, 6F07C7C0)

//  0x6F23DA30 - MakeAbilityAgent_Amtc.  Ctor 0x6F222EC0, Assign 0x6F214B00,
//  rawcode getter 0x6F1117B0 ('Amtc' = 0x416D7463).
MAKE_ABILITY_AGENT_FACTORY2(Amtc, 0x6F23DA30, 0x6F222EC0, 0x6F214B00, 6F1117B0)

//  0x6F23DCA0 - MakeAbilityAgent_Amec.  Ctor 0x6F222F80, Assign 0x6F214C00,
//  rawcode getter 0x6F159AA0 ('Amec' = 0x416D6563).
MAKE_ABILITY_AGENT_FACTORY2(Amec, 0x6F23DCA0, 0x6F222F80, 0x6F214C00, 6F159AA0)

//  0x6F23E0B0 - MakeAbilityAgent_Amou.  Ctor 0x6F223100, Assign 0x6F214E00,
//  rawcode getter 0x6F0DB520 ('Amou' = 0x416D6F75).
MAKE_ABILITY_AGENT_FACTORY2(Amou, 0x6F23E0B0, 0x6F223100, 0x6F214E00, 6F0DB520)

//  0x6F23E660 - MakeAbilityAgent_Apig.  Ctor 0x6F2233A0, Assign 0x6F215180,
//  rawcode getter 0x6F10E980 ('Apig' = 0x41706967).
MAKE_ABILITY_AGENT_FACTORY2(Apig, 0x6F23E660, 0x6F2233A0, 0x6F215180, 6F10E980)

//  0x6F23ECE0 - MakeAbilityAgent_Apts.  Ctor 0x6F2235E0, Assign 0x6F215480,
//  rawcode getter 0x6F1119B0 ('Apts' = 0x41707473).
MAKE_ABILITY_AGENT_FACTORY2(Apts, 0x6F23ECE0, 0x6F2235E0, 0x6F215480, 6F1119B0)

//  0x6F23EF50 - MakeAbilityAgent_Apos.  Ctor 0x6F223700, Assign 0x6F215600,
//  rawcode getter 0x6F199240 ('Apos' = 0x41706F73).
MAKE_ABILITY_AGENT_FACTORY2(Apos, 0x6F23EF50, 0x6F223700, 0x6F215600, 6F199240)

//  0x6F23F360 - MakeAbilityAgent_Arai.  Ctor 0x6F2238E0, Assign 0x6F215880,
//  rawcode getter 0x6F0DE5C0 ('Arai' = 0x41726169).
MAKE_ABILITY_AGENT_FACTORY2(Arai, 0x6F23F360, 0x6F2238E0, 0x6F215880, 6F0DE5C0)

//  0x6F23F5D0 - MakeAbilityAgent_Arej.  Ctor 0x6F223A00, Assign 0x6F215A00,
//  rawcode getter 0x6F0DAB80 ('Arej' = 0x4172656A).
MAKE_ABILITY_AGENT_FACTORY2(Arej, 0x6F23F5D0, 0x6F223A00, 0x6F215A00, 6F0DAB80)

//  0x6F23F9E0 - MakeAbilityAgent_Arpb.  Ctor 0x6F223B80, Assign 0x6F215C00,
//  rawcode getter 0x6F1D3360 ('Arpb' = 0x41727062).
MAKE_ABILITY_AGENT_FACTORY2(Arpb, 0x6F23F9E0, 0x6F223B80, 0x6F215C00, 6F1D3360)

#undef MAKE_ABILITY_AGENT_FACTORY2


//----------------------------------------------------------------------------
//  0x6F13A740 - MakeAbilityAgent_Ault.  Same family, same shape, but the
//  rawcode getter (sub_6F112A40, 'Ault' = 0x41756C74) is called only *once*
//  here - the shipped body keeps the first result in a callee-saved
//  register (esi) and reuses it for InitWidgetAgentQuery's own rawcode
//  argument instead of calling the getter a second time, exactly the shape
//  cunit_agent_seh_abilityfactories.cpp's own 0x6F06EF20/Afui documents.
//  Ctor 0x6F122BB0, Assign 0x6F116CF0 - both naked redirects, defined at
//  the bottom of this file (neither address is in this agent's own
//  worklist slice).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Ault
{
    SCheckedAbilitySlot_Ault(CAgent* candidate);          //  0x6F122BB0
    SCheckedAbilitySlot_Ault* Assign(CAgent* candidate);  //  0x6F116CF0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Ault() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F112A40();   //  0x6F112A40 - 'Ault'

CAgent* MakeAbilityAgent_Ault()
{
    SCheckedAbilitySlot_Ault held(0);

    unsigned int typeId = GetFourCC_6F112A40();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, pool);
    held.Assign(((SMadeAgentHolder3*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//============================================================================
//  Out-of-slice bodies, as naked `mov eax,<addr>/jmp eax` redirects into the
//  real, unhooked shipped functions - same convention the other two files in
//  this family use. None of these addresses are in this agent's own worklist
//  slice, so there is nothing to reconstruct from here; the thunk only
//  exists so this translation unit's own 21 addresses above have something
//  real to call and the .mix actually links (link_check.py). Each is marked
//  THUNK in agent_worktrees, not DIFFERS - a thunk has no behaviour of its
//  own to compare against its own address. retn sizes below are read
//  straight off each target's own `raw_bytes` (all thiscall ctor/Assign
//  pairs retn 4; all rawcode getters retn 0), never inferred from the
//  declaration - the same rule thunk_abi_audit.py's own docs give.
//============================================================================
__declspec(naked) SCheckedAbilitySlot_Aasl::SCheckedAbilitySlot_Aasl(CAgent*)
{
    __asm { mov eax, 06F220DC0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aasl* SCheckedAbilitySlot_Aasl::Assign(CAgent*)
{
    __asm { mov eax, 06F211F00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1961A0()
{
    __asm { mov eax, 06F1961A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aclf::SCheckedAbilitySlot_Aclf(CAgent*)
{
    __asm { mov eax, 06F16A250h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aclf* SCheckedAbilitySlot_Aclf::Assign(CAgent*)
{
    __asm { mov eax, 06F15D9D0h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F199750()
{
    __asm { mov eax, 06F199750h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aspd::SCheckedAbilitySlot_Aspd(CAgent*)
{
    __asm { mov eax, 06F221BA0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aspd* SCheckedAbilitySlot_Aspd::Assign(CAgent*)
{
    __asm { mov eax, 06F213180h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0802D0()
{
    __asm { mov eax, 06F0802D0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aflk::SCheckedAbilitySlot_Aflk(CAgent*)
{
    __asm { mov eax, 06F222380h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aflk* SCheckedAbilitySlot_Aflk::Assign(CAgent*)
{
    __asm { mov eax, 06F213C00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15AA10()
{
    __asm { mov eax, 06F15AA10h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afrz::SCheckedAbilitySlot_Afrz(CAgent*)
{
    __asm { mov eax, 06F16A190h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Afrz* SCheckedAbilitySlot_Afrz::Assign(CAgent*)
{
    __asm { mov eax, 06F15D8D0h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15B210()
{
    __asm { mov eax, 06F15B210h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Agld::SCheckedAbilitySlot_Agld(CAgent*)
{
    __asm { mov eax, 06F03D6B0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Agld* SCheckedAbilitySlot_Agld::Assign(CAgent*)
{
    __asm { mov eax, 06F02C190h }
    __asm { jmp eax }
}
//  GetFourCC_6F0824F0 ('Agld') is NOT thunked here: it is already a real,
//  defined reconstruction in src/Misc/misc_trivial_getters.cpp
//  (`return 0x41676C64;`) - the forward declaration up in the macro
//  instantiation above binds to that definition instead.

__declspec(naked) SCheckedAbilitySlot_Agyb::SCheckedAbilitySlot_Agyb(CAgent*)
{
    __asm { mov eax, 06F2227A0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Agyb* SCheckedAbilitySlot_Agyb::Assign(CAgent*)
{
    __asm { mov eax, 06F214180h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15A690()
{
    __asm { mov eax, 06F15A690h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Anhe::SCheckedAbilitySlot_Anhe(CAgent*)
{
    __asm { mov eax, 06F222920h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Anhe* SCheckedAbilitySlot_Anhe::Assign(CAgent*)
{
    __asm { mov eax, 06F214380h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DA980()
{
    __asm { mov eax, 06F0DA980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aimp::SCheckedAbilitySlot_Aimp(CAgent*)
{
    __asm { mov eax, 06F222A40h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aimp* SCheckedAbilitySlot_Aimp::Assign(CAgent*)
{
    __asm { mov eax, 06F214500h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15A790()
{
    __asm { mov eax, 06F15A790h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Alit::SCheckedAbilitySlot_Alit(CAgent*)
{
    __asm { mov eax, 06F222C20h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Alit* SCheckedAbilitySlot_Alit::Assign(CAgent*)
{
    __asm { mov eax, 06F214780h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F10FC80()
{
    __asm { mov eax, 06F10FC80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amdf::SCheckedAbilitySlot_Amdf(CAgent*)
{
    __asm { mov eax, 06F222CE0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Amdf* SCheckedAbilitySlot_Amdf::Assign(CAgent*)
{
    __asm { mov eax, 06F214880h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F07C7C0()
{
    __asm { mov eax, 06F07C7C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amtc::SCheckedAbilitySlot_Amtc(CAgent*)
{
    __asm { mov eax, 06F222EC0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Amtc* SCheckedAbilitySlot_Amtc::Assign(CAgent*)
{
    __asm { mov eax, 06F214B00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1117B0()
{
    __asm { mov eax, 06F1117B0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amec::SCheckedAbilitySlot_Amec(CAgent*)
{
    __asm { mov eax, 06F222F80h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Amec* SCheckedAbilitySlot_Amec::Assign(CAgent*)
{
    __asm { mov eax, 06F214C00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F159AA0()
{
    __asm { mov eax, 06F159AA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amou::SCheckedAbilitySlot_Amou(CAgent*)
{
    __asm { mov eax, 06F223100h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Amou* SCheckedAbilitySlot_Amou::Assign(CAgent*)
{
    __asm { mov eax, 06F214E00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DB520()
{
    __asm { mov eax, 06F0DB520h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apig::SCheckedAbilitySlot_Apig(CAgent*)
{
    __asm { mov eax, 06F2233A0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Apig* SCheckedAbilitySlot_Apig::Assign(CAgent*)
{
    __asm { mov eax, 06F215180h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F10E980()
{
    __asm { mov eax, 06F10E980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apts::SCheckedAbilitySlot_Apts(CAgent*)
{
    __asm { mov eax, 06F2235E0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Apts* SCheckedAbilitySlot_Apts::Assign(CAgent*)
{
    __asm { mov eax, 06F215480h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1119B0()
{
    __asm { mov eax, 06F1119B0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apos::SCheckedAbilitySlot_Apos(CAgent*)
{
    __asm { mov eax, 06F223700h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Apos* SCheckedAbilitySlot_Apos::Assign(CAgent*)
{
    __asm { mov eax, 06F215600h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F199240()
{
    __asm { mov eax, 06F199240h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arai::SCheckedAbilitySlot_Arai(CAgent*)
{
    __asm { mov eax, 06F2238E0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arai* SCheckedAbilitySlot_Arai::Assign(CAgent*)
{
    __asm { mov eax, 06F215880h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DE5C0()
{
    __asm { mov eax, 06F0DE5C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arej::SCheckedAbilitySlot_Arej(CAgent*)
{
    __asm { mov eax, 06F223A00h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arej* SCheckedAbilitySlot_Arej::Assign(CAgent*)
{
    __asm { mov eax, 06F215A00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DAB80()
{
    __asm { mov eax, 06F0DAB80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arpb::SCheckedAbilitySlot_Arpb(CAgent*)
{
    __asm { mov eax, 06F223B80h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arpb* SCheckedAbilitySlot_Arpb::Assign(CAgent*)
{
    __asm { mov eax, 06F215C00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D3360()
{
    __asm { mov eax, 06F1D3360h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ault::SCheckedAbilitySlot_Ault(CAgent*)
{
    __asm { mov eax, 06F122BB0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Ault* SCheckedAbilitySlot_Ault::Assign(CAgent*)
{
    __asm { mov eax, 06F116CF0h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F112A40()
{
    __asm { mov eax, 06F112A40h }
    __asm { jmp eax }
}
