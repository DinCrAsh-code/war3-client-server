//============================================================================
//  cunit_agent3 batch - one FourCC getter per ability-effect agile type,
//  the `mov eax, <imm32> / retn` leaf misc_rawcode_getters.cpp's own family
//  already documents.  See cunit_agent3_checkedslots.h for how each is used.
//============================================================================
#ifndef CUNIT_AGENT3_ABILITYIDS_H
#define CUNIT_AGENT3_ABILITYIDS_H

//  0x6F027BD0 - 'ARal', MSB first. Already reconstructed as
//  GetFourCC_6F027BD0 (src/Misc/misc_trivial_getters.cpp) before this batch
//  started - use that, not a second name for the same address.

//  0x6F195120 - 'Aast', MSB first.
unsigned int GetAbilityAgileTypeId_Aast();

//  0x6F0D99E0 - 'Aadm', MSB first.
unsigned int GetAbilityAgileTypeId_Aadm();

//  0x6F023DE0 - 'Abli', MSB first.
unsigned int GetAbilityAgileTypeId_Abli();

//  0x6F15BB20 - 'Adro', MSB first.
unsigned int GetAbilityAgileTypeId_Adro();

//  0x6F1994C0 - 'Acmg', MSB first.
unsigned int GetAbilityAgileTypeId_Acmg();

//  0x6F1D3020 - 'ACrn', MSB first.
unsigned int GetAbilityAgileTypeId_ACrn();

//  0x6F07C720 - 'Adef', MSB first.
unsigned int GetAbilityAgileTypeId_Adef();

//  0x6F0D98E0 - 'Adis', MSB first.
unsigned int GetAbilityAgileTypeId_Adis();

//  0x6F07F980 - 'Aeye', MSB first.
unsigned int GetAbilityAgileTypeId_Aeye();

//  0x6F026970 - 'ANgl', MSB first.
unsigned int GetAbilityAgileTypeId_ANgl();

//  0x6F113F70 - 'ANsa', MSB first.
unsigned int GetAbilityAgileTypeId_ANsa();

//  0x6F0DCAA0 - 'Aamk', MSB first.
unsigned int GetAbilityAgileTypeId_Aamk();

//  0x6F196790 - 'AUav', MSB first.
unsigned int GetAbilityAgileTypeId_AUav();

//  0x6F197B80 - 'AHbz', MSB first.
unsigned int GetAbilityAgileTypeId_AHbz();

//  0x6F19A8C0 - 'AOcr', MSB first.
unsigned int GetAbilityAgileTypeId_AOcr();

//  0x6F10F0F0 - 'AUin', MSB first.
unsigned int GetAbilityAgileTypeId_AUin();

//  0x6F07F330 - 'AEer', MSB first.
unsigned int GetAbilityAgileTypeId_AEer();

//  0x6F081BE0 - 'AEfn', MSB first.
unsigned int GetAbilityAgileTypeId_AEfn();

//  0x6F082C70 - 'ANht', MSB first.
unsigned int GetAbilityAgileTypeId_ANht();

//  0x6F025E50 - 'AHmt', MSB first.
unsigned int GetAbilityAgileTypeId_AHmt();

//  0x6F1D3740 - 'AHre', MSB first.
unsigned int GetAbilityAgileTypeId_AHre();

//  0x6F1D7A10 - 'AOsf', MSB first.
unsigned int GetAbilityAgileTypeId_AOsf();

//  0x6F156A20 - 'AUts', MSB first.
unsigned int GetAbilityAgileTypeId_AUts();

//  0x6F1599A0 - 'ANwm', MSB first.
unsigned int GetAbilityAgileTypeId_ANwm();

//  0x6F157900 - 'ANde', MSB first.
unsigned int GetAbilityAgileTypeId_ANde();

//  0x6F080800 - 'ANso', MSB first.
unsigned int GetAbilityAgileTypeId_ANso();

//  0x6F0DC820 - 'AIat', MSB first.
unsigned int GetAbilityAgileTypeId_AIat();

//  0x6F0DD7A0 - 'AIfe', MSB first.
unsigned int GetAbilityAgileTypeId_AIfe();

//  0x6F0801D0 - 'AIfs', MSB first.
unsigned int GetAbilityAgileTypeId_AIfs();

//  0x6F0DCD20 - 'AIml', MSB first.
unsigned int GetAbilityAgileTypeId_AIml();

//  0x6F0DC400 - 'AIsb', MSB first.
unsigned int GetAbilityAgileTypeId_AIsb();

//  0x6F0D9A60 - 'AIdi', MSB first.
unsigned int GetAbilityAgileTypeId_AIdi();

//  0x6F025F90 - 'AIrt', MSB first.
unsigned int GetAbilityAgileTypeId_AIrt();

//  0x6F025FF0 - 'AIte', MSB first.
unsigned int GetAbilityAgileTypeId_AIte();

//  0x6F0DD3A0 - 'AIgo', MSB first.
unsigned int GetAbilityAgileTypeId_AIgo();

//  0x6F0D92A0 - 'AIbl', MSB first.
unsigned int GetAbilityAgileTypeId_AIbl();

//  0x6F028940 - 'Amai', MSB first.
unsigned int GetAbilityAgileTypeId_Amai();

#endif
