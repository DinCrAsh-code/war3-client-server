//============================================================================
//  See cunit_agent3_abilityids.h.  Every one of these is the same two
//  instructions - `mov eax, <imm32>` / `retn` - misc_rawcode_getters.cpp's
//  own family already is; kept in their own translation unit purely by
//  agent-batch convention (cunit_agent3_*), not because anything here calls
//  another.
//
//  One member (GetAbilityAgileTypeId_Amai, 0x6F028940) is NOT defined here:
//  that address is agent5's own worklist target in this parallel run
//  (cross-checked against asm/cunit_agent5_worklist.md), so this file only
//  declares it (cunit_agent3_abilityids.h) and leaves the body for that
//  branch - defining it here too would be a duplicate reconstruction of an
//  address outside this agent's assigned slice.
//============================================================================
#include "cunit_agent3_abilityids.h"

//  0x6F027BD0 - already GetFourCC_6F027BD0 (misc_trivial_getters.cpp);
//  not redefined here, see the header.
unsigned int GetFourCC_6F027BD0();

//  0x6F195120
unsigned int GetAbilityAgileTypeId_Aast() { return 0x41617374; }

//  0x6F0D99E0
unsigned int GetAbilityAgileTypeId_Aadm() { return 0x4161646D; }

//  0x6F023DE0
unsigned int GetAbilityAgileTypeId_Abli() { return 0x41626C69; }

//  0x6F15BB20
unsigned int GetAbilityAgileTypeId_Adro() { return 0x4164726F; }

//  0x6F1994C0
unsigned int GetAbilityAgileTypeId_Acmg() { return 0x41636D67; }

//  0x6F1D3020
unsigned int GetAbilityAgileTypeId_ACrn() { return 0x4143726E; }

//  0x6F07C720
unsigned int GetAbilityAgileTypeId_Adef() { return 0x41646566; }

//  0x6F0D98E0
unsigned int GetAbilityAgileTypeId_Adis() { return 0x41646973; }

//  0x6F07F980
unsigned int GetAbilityAgileTypeId_Aeye() { return 0x41657965; }

//  0x6F026970
unsigned int GetAbilityAgileTypeId_ANgl() { return 0x414E676C; }

//  0x6F113F70
unsigned int GetAbilityAgileTypeId_ANsa() { return 0x414E7361; }

//  0x6F0DCAA0
unsigned int GetAbilityAgileTypeId_Aamk() { return 0x41616D6B; }

//  0x6F196790
unsigned int GetAbilityAgileTypeId_AUav() { return 0x41556176; }

//  0x6F197B80
unsigned int GetAbilityAgileTypeId_AHbz() { return 0x4148627A; }

//  0x6F19A8C0
unsigned int GetAbilityAgileTypeId_AOcr() { return 0x414F6372; }

//  0x6F10F0F0
unsigned int GetAbilityAgileTypeId_AUin() { return 0x4155696E; }

//  0x6F07F330
unsigned int GetAbilityAgileTypeId_AEer() { return 0x41456572; }

//  0x6F081BE0
unsigned int GetAbilityAgileTypeId_AEfn() { return 0x4145666E; }

//  0x6F082C70
unsigned int GetAbilityAgileTypeId_ANht() { return 0x414E6874; }

//  0x6F025E50
unsigned int GetAbilityAgileTypeId_AHmt() { return 0x41486D74; }

//  0x6F1D3740
unsigned int GetAbilityAgileTypeId_AHre() { return 0x41487265; }

//  0x6F1D7A10
unsigned int GetAbilityAgileTypeId_AOsf() { return 0x414F7366; }

//  0x6F156A20
unsigned int GetAbilityAgileTypeId_AUts() { return 0x41557473; }

//  0x6F1599A0
unsigned int GetAbilityAgileTypeId_ANwm() { return 0x414E776D; }

//  0x6F157900
unsigned int GetAbilityAgileTypeId_ANde() { return 0x414E6465; }

//  0x6F080800
unsigned int GetAbilityAgileTypeId_ANso() { return 0x414E736F; }

//  0x6F0DC820
unsigned int GetAbilityAgileTypeId_AIat() { return 0x41496174; }

//  0x6F0DD7A0
unsigned int GetAbilityAgileTypeId_AIfe() { return 0x41496665; }

//  0x6F0801D0
unsigned int GetAbilityAgileTypeId_AIfs() { return 0x41496673; }

//  0x6F0DCD20
unsigned int GetAbilityAgileTypeId_AIml() { return 0x41496D6C; }

//  0x6F0DC400
unsigned int GetAbilityAgileTypeId_AIsb() { return 0x41497362; }

//  0x6F0D9A60
unsigned int GetAbilityAgileTypeId_AIdi() { return 0x41496469; }

//  0x6F025F90
unsigned int GetAbilityAgileTypeId_AIrt() { return 0x41497274; }

//  0x6F025FF0
unsigned int GetAbilityAgileTypeId_AIte() { return 0x41497465; }

//  0x6F0DD3A0
unsigned int GetAbilityAgileTypeId_AIgo() { return 0x4149676F; }

//  0x6F0D92A0
unsigned int GetAbilityAgileTypeId_AIbl() { return 0x4149626C; }

//  0x6F028940 - 'Amai' - agent5's own target, declared not defined here.


//----------------------------------------------------------------------------
//  Link-completion thunk - 0x6F028940 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent5's own GetConstant_6F028940,
//  tools/funcmap.py), but under a different name, so the declaration in
//  cunit_agent3_abilityids.h never resolves against it. Same redirect-thunk
//  pattern as this repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) unsigned int GetAbilityAgileTypeId_Amai()
{
    __asm { mov eax, 06F028940h }
    __asm { jmp eax }
}
