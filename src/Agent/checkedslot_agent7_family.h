//============================================================================
//  cunit_agent7 batch: one throwaway "checked slot" type per instance of
//  the "GetXxxAbility" family (unit_getability_agent7_batch.cpp).  See
//  checkedslot_agent7_helpers.cpp for what each member actually does and
//  why the bodies live in a different TU than their declarations.
//============================================================================
#ifndef CHECKEDSLOT_AGENT7_FAMILY_H
#define CHECKEDSLOT_AGENT7_FAMILY_H

#include "agent.h"

//  0x6F220C40 ctor -> 0x6F211D00 Assign -> 0x6F195D20 'Aakb'.
unsigned int GetFourCC_6F195D20();

struct SLocalAbilitySlot_6F2384E0
{
    SLocalAbilitySlot_6F2384E0(CAgent* candidate);
    SLocalAbilitySlot_6F2384E0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F220FA0 ctor -> 0x6F212180 Assign -> 0x6F196D50 'Abar'.
unsigned int GetFourCC_6F196D50();

struct SLocalAbilitySlot_6F238C30
{
    SLocalAbilitySlot_6F238C30(CAgent* candidate);
    SLocalAbilitySlot_6F238C30* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221300 ctor -> 0x6F212600 Assign -> 0x6F1984A0 'ACbf'.
unsigned int GetFourCC_6F1984A0();

struct SLocalAbilitySlot_6F239380
{
    SLocalAbilitySlot_6F239380(CAgent* candidate);
    SLocalAbilitySlot_6F239380* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221540 ctor -> 0x6F212900 Assign -> 0x6F1955E0 'ANcl'.
unsigned int GetFourCC_6F1955E0();

struct SLocalAbilitySlot_6F239AD0
{
    SLocalAbilitySlot_6F239AD0(CAgent* candidate);
    SLocalAbilitySlot_6F239AD0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221780 ctor -> 0x6F212C00 Assign -> 0x6F19A230 'Acoh'.
unsigned int GetFourCC_6F19A230();

struct SLocalAbilitySlot_6F23A150
{
    SLocalAbilitySlot_6F23A150(CAgent* candidate);
    SLocalAbilitySlot_6F23A150* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221A20 ctor -> 0x6F212F80 Assign -> 0x6F07F780 'Acrs'.
unsigned int GetFourCC_6F07F780();

struct SLocalAbilitySlot_6F23A7D0
{
    SLocalAbilitySlot_6F23A7D0(CAgent* candidate);
    SLocalAbilitySlot_6F23A7D0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221D80 ctor -> 0x6F213400 Assign -> 0x6F07CD70 'Adev'.
unsigned int GetFourCC_6F07CD70();

struct SLocalAbilitySlot_6F23B0C0
{
    SLocalAbilitySlot_6F23B0C0(CAgent* candidate);
    SLocalAbilitySlot_6F23B0C0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F222080 ctor -> 0x6F213800 Assign -> 0x6F0DA190 'Aenc'.
unsigned int GetFourCC_6F0DA190();

struct SLocalAbilitySlot_6F23B740
{
    SLocalAbilitySlot_6F23B740(CAgent* candidate);
    SLocalAbilitySlot_6F23B740* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2223E0 ctor -> 0x6F213C80 Assign -> 0x6F0818C0 'Afak'.
unsigned int GetFourCC_6F0818C0();

struct SLocalAbilitySlot_6F23BE90
{
    SLocalAbilitySlot_6F23BE90(CAgent* candidate);
    SLocalAbilitySlot_6F23BE90* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223400 ctor -> 0x6F215200 Assign -> 0x6F112BD0 'ANpa'.
unsigned int GetFourCC_6F112BD0();

struct SLocalAbilitySlot_6F23E730
{
    SLocalAbilitySlot_6F23E730(CAgent* candidate);
    SLocalAbilitySlot_6F23E730* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F224600 ctor -> 0x6F216A00 Assign -> 0x6F1D7B10 'AIsh'.
unsigned int GetFourCC_6F1D7B10();

struct SLocalAbilitySlot_6F241240
{
    SLocalAbilitySlot_6F241240(CAgent* candidate);
    SLocalAbilitySlot_6F241240* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F225080 ctor -> 0x6F217800 Assign -> 0x6F1960A0 'AOae'.
unsigned int GetFourCC_6F1960A0();

struct SLocalAbilitySlot_6F242B70
{
    SLocalAbilitySlot_6F242B70(CAgent* candidate);
    SLocalAbilitySlot_6F242B70* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F225380 ctor -> 0x6F217C00 Assign -> 0x6F1D3BA0 'ANbr'.
unsigned int GetFourCC_6F1D3BA0();

struct SLocalAbilitySlot_6F2431F0
{
    SLocalAbilitySlot_6F2431F0(CAgent* candidate);
    SLocalAbilitySlot_6F2431F0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F1A8810 ctor -> 0x6F19B190 Assign -> 0x6F198D10 'AOcl'.
unsigned int GetFourCC_6F198D10();

struct SLocalAbilitySlot_6F243870
{
    SLocalAbilitySlot_6F243870(CAgent* candidate);
    SLocalAbilitySlot_6F243870* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F225920 ctor -> 0x6F218380 Assign -> 0x6F025EB0 'AUds'.
unsigned int GetFourCC_6F025EB0();

struct SLocalAbilitySlot_6F243EF0
{
    SLocalAbilitySlot_6F243EF0(CAgent* candidate);
    SLocalAbilitySlot_6F243EF0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F225C20 ctor -> 0x6F218780 Assign -> 0x6F19AA40 'ANdb'.
unsigned int GetFourCC_6F19AA40();

struct SLocalAbilitySlot_6F244570
{
    SLocalAbilitySlot_6F244570(CAgent* candidate);
    SLocalAbilitySlot_6F244570* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F225F20 ctor -> 0x6F218B80 Assign -> 0x6F195460 'ANfd'.
unsigned int GetFourCC_6F195460();

struct SLocalAbilitySlot_6F244BF0
{
    SLocalAbilitySlot_6F244BF0(CAgent* candidate);
    SLocalAbilitySlot_6F244BF0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F226220 ctor -> 0x6F218F80 Assign -> 0x6F156E20 'AUfn'.
unsigned int GetFourCC_6F156E20();

struct SLocalAbilitySlot_6F245270
{
    SLocalAbilitySlot_6F245270(CAgent* candidate);
    SLocalAbilitySlot_6F245270* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F226880 ctor -> 0x6F219800 Assign -> 0x6F113B50 'AEpa'.
unsigned int GetFourCC_6F113B50();

struct SLocalAbilitySlot_6F246040
{
    SLocalAbilitySlot_6F246040(CAgent* candidate);
    SLocalAbilitySlot_6F246040* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F226B80 ctor -> 0x6F219C00 Assign -> 0x6F1D5800 'AOsh'.
unsigned int GetFourCC_6F1D5800();

struct SLocalAbilitySlot_6F2466C0
{
    SLocalAbilitySlot_6F2466C0(CAgent* candidate);
    SLocalAbilitySlot_6F2466C0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F226E80 ctor -> 0x6F21A000 Assign -> 0x6F159BA0 'ANsg'.
unsigned int GetFourCC_6F159BA0();

struct SLocalAbilitySlot_6F246D40
{
    SLocalAbilitySlot_6F246D40(CAgent* candidate);
    SLocalAbilitySlot_6F246D40* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2272A0 ctor -> 0x6F21A580 Assign -> 0x6F158660 'AEtq'.
unsigned int GetFourCC_6F158660();

struct SLocalAbilitySlot_6F247630
{
    SLocalAbilitySlot_6F247630(CAgent* candidate);
    SLocalAbilitySlot_6F247630* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2275A0 ctor -> 0x6F21A980 Assign -> 0x6F194880 'ANcr'.
unsigned int GetFourCC_6F194880();

struct SLocalAbilitySlot_6F247CB0
{
    SLocalAbilitySlot_6F247CB0(CAgent* candidate);
    SLocalAbilitySlot_6F247CB0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F227840 ctor -> 0x6F21AD00 Assign -> 0x6F157300 'ANsy'.
unsigned int GetFourCC_6F157300();

struct SLocalAbilitySlot_6F248330
{
    SLocalAbilitySlot_6F248330(CAgent* candidate);
    SLocalAbilitySlot_6F248330* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F227B40 ctor -> 0x6F21B100 Assign -> 0x6F0DCB20 'AIam'.
unsigned int GetFourCC_6F0DCB20();

struct SLocalAbilitySlot_6F2489B0
{
    SLocalAbilitySlot_6F2489B0(CAgent* candidate);
    SLocalAbilitySlot_6F2489B0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F227E40 ctor -> 0x6F21B500 Assign -> 0x6F0DD320 'AIem'.
unsigned int GetFourCC_6F0DD320();

struct SLocalAbilitySlot_6F249030
{
    SLocalAbilitySlot_6F249030(CAgent* candidate);
    SLocalAbilitySlot_6F249030* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228140 ctor -> 0x6F21B900 Assign -> 0x6F07FFD0 'AIfd'.
unsigned int GetFourCC_6F07FFD0();

struct SLocalAbilitySlot_6F2496B0
{
    SLocalAbilitySlot_6F2496B0(CAgent* candidate);
    SLocalAbilitySlot_6F2496B0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228440 ctor -> 0x6F21BD00 Assign -> 0x6F082220 'AIgl'.
unsigned int GetFourCC_6F082220();

struct SLocalAbilitySlot_6F249D30
{
    SLocalAbilitySlot_6F249D30(CAgent* candidate);
    SLocalAbilitySlot_6F249D30* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228740 ctor -> 0x6F21C100 Assign -> 0x6F0DCE20 'AIms'.
unsigned int GetFourCC_6F0DCE20();

struct SLocalAbilitySlot_6F24A3B0
{
    SLocalAbilitySlot_6F24A3B0(CAgent* candidate);
    SLocalAbilitySlot_6F24A3B0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228A40 ctor -> 0x6F21C500 Assign -> 0x6F1993C0 'AIco'.
unsigned int GetFourCC_6F1993C0();

struct SLocalAbilitySlot_6F24AB00
{
    SLocalAbilitySlot_6F24AB00(CAgent* candidate);
    SLocalAbilitySlot_6F24AB00* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228D40 ctor -> 0x6F21C900 Assign -> 0x6F111510 'AIma'.
unsigned int GetFourCC_6F111510();

struct SLocalAbilitySlot_6F24B180
{
    SLocalAbilitySlot_6F24B180(CAgent* candidate);
    SLocalAbilitySlot_6F24B180* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F229040 ctor -> 0x6F21CD00 Assign -> 0x6F1D32E0 'AIra'.
unsigned int GetFourCC_6F1D32E0();

struct SLocalAbilitySlot_6F24B800
{
    SLocalAbilitySlot_6F24B800(CAgent* candidate);
    SLocalAbilitySlot_6F24B800* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F229340 ctor -> 0x6F21D100 Assign -> 0x6F10FA10 'AIvu'.
unsigned int GetFourCC_6F10FA10();

struct SLocalAbilitySlot_6F24BE80
{
    SLocalAbilitySlot_6F24BE80(CAgent* candidate);
    SLocalAbilitySlot_6F24BE80* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2295E0 ctor -> 0x6F21D480 Assign -> 0x6F1596A0 'AImo'.
unsigned int GetFourCC_6F1596A0();

struct SLocalAbilitySlot_6F24C500
{
    SLocalAbilitySlot_6F24C500(CAgent* candidate);
    SLocalAbilitySlot_6F24C500* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F229940 ctor -> 0x6F21D900 Assign -> 0x6F0DDB20 'AIsr'.
unsigned int GetFourCC_6F0DDB20();

struct SLocalAbilitySlot_6F24CC50
{
    SLocalAbilitySlot_6F24CC50(CAgent* candidate);
    SLocalAbilitySlot_6F24CC50* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F280E40 ctor -> 0x6F275630 Assign -> 0x6F0D91A0 'AUbu'.
unsigned int GetFourCC_6F0D91A0();

struct SLocalAbilitySlot_6F29AC70
{
    SLocalAbilitySlot_6F29AC70(CAgent* candidate);
    SLocalAbilitySlot_6F29AC70* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F0E7E30 ctor -> 0x6F0DF6C0 Assign -> 0x6F200960 'Amov'.
unsigned int GetFourCC_6F200960();

struct SLocalAbilitySlot_6F29B2F0
{
    SLocalAbilitySlot_6F29B2F0(CAgent* candidate);
    SLocalAbilitySlot_6F29B2F0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F220D00 ctor -> 0x6F211E00 Assign -> 0x6F022E90 'Aoar'.
unsigned int GetFourCC_6F022E90();

struct SLocalAbilitySlot_6F238680
{
    SLocalAbilitySlot_6F238680(CAgent* candidate);
    SLocalAbilitySlot_6F238680* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221B40 ctor -> 0x6F213100 Assign -> 0x6F080250 'Asod'.
unsigned int GetFourCC_6F080250();

struct SLocalAbilitySlot_6F23AA40
{
    SLocalAbilitySlot_6F23AA40(CAgent* candidate);
    SLocalAbilitySlot_6F23AA40* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2224A0 ctor -> 0x6F213D80 Assign -> 0x6F15AA90 'Afsh'.
unsigned int GetFourCC_6F15AA90();

struct SLocalAbilitySlot_6F23C030
{
    SLocalAbilitySlot_6F23C030(CAgent* candidate);
    SLocalAbilitySlot_6F23C030* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F222740 ctor -> 0x6F214100 Assign -> 0x6F082660 'Agyd'.
unsigned int GetFourCC_6F082660();

struct SLocalAbilitySlot_6F23C780
{
    SLocalAbilitySlot_6F23C780(CAgent* candidate);
    SLocalAbilitySlot_6F23C780* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2229E0 ctor -> 0x6F214480 Assign -> 0x6F1D4CA0 'Ahid'.
unsigned int GetFourCC_6F1D4CA0();

struct SLocalAbilitySlot_6F23CE00
{
    SLocalAbilitySlot_6F23CE00(CAgent* candidate);
    SLocalAbilitySlot_6F23CE00* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F16A1F0 ctor -> 0x6F15D950 Assign -> 0x6F15B410 'Aliq'.
unsigned int GetFourCC_6F15B410();

struct SLocalAbilitySlot_6F23D480
{
    SLocalAbilitySlot_6F23D480(CAgent* candidate);
    SLocalAbilitySlot_6F23D480* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F0925E0 ctor -> 0x6F084970 Assign -> 0x6F1118B0 'Amel'.
unsigned int GetFourCC_6F1118B0();

struct SLocalAbilitySlot_6F23DBD0
{
    SLocalAbilitySlot_6F23DBD0(CAgent* candidate);
    SLocalAbilitySlot_6F23DBD0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2231C0 ctor -> 0x6F214F00 Assign -> 0x6F024C50 'Andt'.
unsigned int GetFourCC_6F024C50();

struct SLocalAbilitySlot_6F23E250
{
    SLocalAbilitySlot_6F23E250(CAgent* candidate);
    SLocalAbilitySlot_6F23E250* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2236A0 ctor -> 0x6F215580 Assign -> 0x6F113C40 'Aply'.
unsigned int GetFourCC_6F113C40();

struct SLocalAbilitySlot_6F23EE80
{
    SLocalAbilitySlot_6F23EE80(CAgent* candidate);
    SLocalAbilitySlot_6F23EE80* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2239A0 ctor -> 0x6F215980 Assign -> 0x6F1D7290 'Arbr'.
unsigned int GetFourCC_6F1D7290();

struct SLocalAbilitySlot_6F23F500
{
    SLocalAbilitySlot_6F23F500(CAgent* candidate);
    SLocalAbilitySlot_6F23F500* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223C40 ctor -> 0x6F215D00 Assign -> 0x6F1D3460 'Arpm'.
unsigned int GetFourCC_6F1D3460();

struct SLocalAbilitySlot_6F23FB80
{
    SLocalAbilitySlot_6F23FB80(CAgent* candidate);
    SLocalAbilitySlot_6F23FB80* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223F40 ctor -> 0x6F216100 Assign -> 0x6F0283A0 'Aroo'.
unsigned int GetFourCC_6F0283A0();

struct SLocalAbilitySlot_6F240200
{
    SLocalAbilitySlot_6F240200(CAgent* candidate);
    SLocalAbilitySlot_6F240200* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2242A0 ctor -> 0x6F216580 Assign -> 0x6F07FD10 'Ashs'.
unsigned int GetFourCC_6F07FD10();

struct SLocalAbilitySlot_6F240A20
{
    SLocalAbilitySlot_6F240A20(CAgent* candidate);
    SLocalAbilitySlot_6F240A20* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F224660 ctor -> 0x6F216A80 Assign -> 0x6F198720 'Astd'.
unsigned int GetFourCC_6F198720();

struct SLocalAbilitySlot_6F241310
{
    SLocalAbilitySlot_6F241310(CAgent* candidate);
    SLocalAbilitySlot_6F241310* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2249C0 ctor -> 0x6F216F00 Assign -> 0x6F1565E0 'Atpi'.
unsigned int GetFourCC_6F1565E0();

struct SLocalAbilitySlot_6F241A60
{
    SLocalAbilitySlot_6F241A60(CAgent* candidate);
    SLocalAbilitySlot_6F241A60* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F224CC0 ctor -> 0x6F217300 Assign -> 0x6F1D7780 'Avng'.
unsigned int GetFourCC_6F1D7780();

struct SLocalAbilitySlot_6F2421B0
{
    SLocalAbilitySlot_6F2421B0(CAgent* candidate);
    SLocalAbilitySlot_6F2421B0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2251A0 ctor -> 0x6F217980 Assign -> 0x6F196690 'AUau'.
unsigned int GetFourCC_6F196690();

struct SLocalAbilitySlot_6F242DE0
{
    SLocalAbilitySlot_6F242DE0(CAgent* candidate);
    SLocalAbilitySlot_6F242DE0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2266A0 ctor -> 0x6F219580 Assign -> 0x6F111610 'ANms'.
unsigned int GetFourCC_6F111610();

struct SLocalAbilitySlot_6F245C30
{
    SLocalAbilitySlot_6F245C30(CAgent* candidate);
    SLocalAbilitySlot_6F245C30* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F168450 ctor -> 0x6F15D230 Assign -> 0x6F157580 'ANcs'.
unsigned int GetFourCC_6F157580();

struct SLocalAbilitySlot_6F247F20
{
    SLocalAbilitySlot_6F247F20(CAgent* candidate);
    SLocalAbilitySlot_6F247F20* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228B60 ctor -> 0x6F21C680 Assign -> 0x6F024BF0 'AIta'.
unsigned int GetFourCC_6F024BF0();

struct SLocalAbilitySlot_6F24AD70
{
    SLocalAbilitySlot_6F24AD70(CAgent* candidate);
    SLocalAbilitySlot_6F24AD70* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F280F60 ctor -> 0x6F2757B0 Assign -> 0x6F0278B0 'Aque'.  The leaf is
//  already real (Misc/misc_trivial_getters.cpp) - declared, not redefined.
unsigned int GetFourCC_6F0278B0();

struct SLocalAbilitySlot_6F29AEE0
{
    SLocalAbilitySlot_6F29AEE0(CAgent* candidate);
    SLocalAbilitySlot_6F29AEE0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

#endif
