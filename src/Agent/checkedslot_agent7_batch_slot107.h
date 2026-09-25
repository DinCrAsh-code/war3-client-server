//============================================================================
//  Twenty more instances of the cunit_agent7 "throwaway checked slot" family
//  (checkedslot_agent7_family.h / unit_getability_agent7_batch.cpp), reached
//  from CUnit's slot107 closure (per-template initialiser, 0x6F2A0E30)
//  rather than from that batch's own worklist. Same shape, same
//  unreproducible __except_handler4-shaped frame documented in
//  docs/msvc-vc8-idioms.md ("An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce") - every one of these twenty is DIFFERS for
//  that reason, not argued to EXACT.
//
//  As with the sibling family, none of the three addresses per instance
//  (leaf, Assign, converting ctor) is this batch's own worklist heading -
//  all three sit past this batch's own BFS cut - so none carries a funcmap
//  entry or a claimed verdict here; they are declared and defined only so
//  the outer accessor's own calls compile and stay real out-of-line `call`s.
//  See checkedslot_agent7_batch_slot107_helpers.cpp for the bodies.
//
//  Unlike the sibling batch's own throwaway SLocalAbilitySlot_XXX types
//  (checkedslot_agent7_family.h), each of these twenty declares a real
//  destructor calling Release() - the shipped tail every one of these
//  accessors has (`add dword ptr [eax+4],-1` / re-read / conditional
//  `ReleaseSelf` call, the SCheckedLocationSlot-style "-1 then separate ==0
//  test" shape, not the single `--x==0`) only has anywhere to come from if
//  the local really does have a destructor: with none declared, the
//  compiler's own implicit one is trivial for a raw-pointer member and
//  emits nothing, which is the sibling batch's own still-open gap (its own
//  worklist entries sit at 12/78, "not yet root-caused instruction by
//  instruction" per its own funcmap.py BEHAVIOUR entry). Declaring the
//  destructor here is what lets the scope-exit release actually compile.
//============================================================================
#ifndef CHECKEDSLOT_AGENT7_BATCH_SLOT107_H
#define CHECKEDSLOT_AGENT7_BATCH_SLOT107_H

#include "agent.h"

//  0x6F220A60 ctor -> 0x6F211A80 Assign -> 0x6F026870 'Aall'.
unsigned int GetFourCC_6F026870();
struct SLocalAbilitySlot_6F2380D0
{
    SLocalAbilitySlot_6F2380D0(CAgent* candidate);
    ~SLocalAbilitySlot_6F2380D0() { Release(); }
    SLocalAbilitySlot_6F2380D0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F221840 ctor -> 0x6F212D00 Assign -> 0x6F07CDF0 'ACdv'.
unsigned int GetFourCC_6F07CDF0();
struct SLocalAbilitySlot_6F23A3C0
{
    SLocalAbilitySlot_6F23A3C0(CAgent* candidate);
    ~SLocalAbilitySlot_6F23A3C0() { Release(); }
    SLocalAbilitySlot_6F23A3C0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F222440 ctor -> 0x6F213D00 Assign -> 0x6F081A60 'Afla'.
unsigned int GetFourCC_6F081A60();
struct SLocalAbilitySlot_6F23BF60
{
    SLocalAbilitySlot_6F23BF60(CAgent* candidate);
    ~SLocalAbilitySlot_6F23BF60() { Release(); }
    SLocalAbilitySlot_6F23BF60* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2226E0 ctor -> 0x6F214080 Assign -> 0x6F07E3A0 'Agra'.
unsigned int GetFourCC_6F07E3A0();
struct SLocalAbilitySlot_6F23C6B0
{
    SLocalAbilitySlot_6F23C6B0(CAgent* candidate);
    ~SLocalAbilitySlot_6F23C6B0() { Release(); }
    SLocalAbilitySlot_6F23C6B0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F222980 ctor -> 0x6F214400 Assign -> 0x6F082830 'Ahwd'.
unsigned int GetFourCC_6F082830();
struct SLocalAbilitySlot_6F23CD30
{
    SLocalAbilitySlot_6F23CD30(CAgent* candidate);
    ~SLocalAbilitySlot_6F23CD30() { Release(); }
    SLocalAbilitySlot_6F23CD30* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F222C80 ctor -> 0x6F214800 Assign -> 0x6F1100B0 'Alsh'.
unsigned int GetFourCC_6F1100B0();
struct SLocalAbilitySlot_6F23D3B0
{
    SLocalAbilitySlot_6F23D3B0(CAgent* candidate);
    ~SLocalAbilitySlot_6F23D3B0() { Release(); }
    SLocalAbilitySlot_6F23D3B0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F222F20 ctor -> 0x6F214B80 Assign -> 0x6F111830 'Amed'.
unsigned int GetFourCC_6F111830();
struct SLocalAbilitySlot_6F23DB00
{
    SLocalAbilitySlot_6F23DB00(CAgent* candidate);
    ~SLocalAbilitySlot_6F23DB00() { Release(); }
    SLocalAbilitySlot_6F23DB00* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223160 ctor -> 0x6F214E80 Assign -> 0x6F026790 'Aneu'.
unsigned int GetFourCC_6F026790();
struct SLocalAbilitySlot_6F23E180
{
    SLocalAbilitySlot_6F23E180(CAgent* candidate);
    ~SLocalAbilitySlot_6F23E180() { Release(); }
    SLocalAbilitySlot_6F23E180* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223640 ctor -> 0x6F215500 Assign -> 0x6F1139D0 'Apoi'.
unsigned int GetFourCC_6F1139D0();
struct SLocalAbilitySlot_6F23EDB0
{
    SLocalAbilitySlot_6F23EDB0(CAgent* candidate);
    ~SLocalAbilitySlot_6F23EDB0() { Release(); }
    SLocalAbilitySlot_6F23EDB0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223940 ctor -> 0x6F215900 Assign -> 0x6F1D2D80 'Arav'.
unsigned int GetFourCC_6F1D2D80();
struct SLocalAbilitySlot_6F23F430
{
    SLocalAbilitySlot_6F23F430(CAgent* candidate);
    ~SLocalAbilitySlot_6F23F430() { Release(); }
    SLocalAbilitySlot_6F23F430* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223BE0 ctor -> 0x6F215C80 Assign -> 0x6F1D33E0 'Arpl'.
unsigned int GetFourCC_6F1D33E0();
struct SLocalAbilitySlot_6F23FAB0
{
    SLocalAbilitySlot_6F23FAB0(CAgent* candidate);
    ~SLocalAbilitySlot_6F23FAB0() { Release(); }
    SLocalAbilitySlot_6F23FAB0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F223EE0 ctor -> 0x6F216080 Assign -> 0x6F1D3AA0 'Aroa'.
unsigned int GetFourCC_6F1D3AA0();
struct SLocalAbilitySlot_6F240130
{
    SLocalAbilitySlot_6F240130(CAgent* candidate);
    ~SLocalAbilitySlot_6F240130() { Release(); }
    SLocalAbilitySlot_6F240130* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F224240 ctor -> 0x6F216500 Assign -> 0x6F1D4C20 'Ashm'.
unsigned int GetFourCC_6F1D4C20();
struct SLocalAbilitySlot_6F240950
{
    SLocalAbilitySlot_6F240950(CAgent* candidate);
    ~SLocalAbilitySlot_6F240950() { Release(); }
    SLocalAbilitySlot_6F240950* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F1E3CF0 ctor -> 0x6F1D87A0 Assign -> 0x6F1D73D0 'Aspl'.
unsigned int GetFourCC_6F1D73D0();
struct SLocalAbilitySlot_6F241170
{
    SLocalAbilitySlot_6F241170(CAgent* candidate);
    ~SLocalAbilitySlot_6F241170() { Release(); }
    SLocalAbilitySlot_6F241170* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F224960 ctor -> 0x6F216E80 Assign -> 0x6F156560 'Atlp'.
unsigned int GetFourCC_6F156560();
struct SLocalAbilitySlot_6F241990
{
    SLocalAbilitySlot_6F241990(CAgent* candidate);
    ~SLocalAbilitySlot_6F241990() { Release(); }
    SLocalAbilitySlot_6F241990* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F092460 ctor -> 0x6F084770 Assign -> 0x6F158F50 'Auns'.
unsigned int GetFourCC_6F158F50();
struct SLocalAbilitySlot_6F2420E0
{
    SLocalAbilitySlot_6F2420E0(CAgent* candidate);
    ~SLocalAbilitySlot_6F2420E0() { Release(); }
    SLocalAbilitySlot_6F2420E0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F1A45F0 ctor -> 0x6F19AEC0 Assign -> 0x6F194F10 'AUan'.
unsigned int GetFourCC_6F194F10();
struct SLocalAbilitySlot_6F242760
{
    SLocalAbilitySlot_6F242760(CAgent* candidate);
    ~SLocalAbilitySlot_6F242760() { Release(); }
    SLocalAbilitySlot_6F242760* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2265E0 ctor -> 0x6F219480 Assign -> 0x6F1102C0 'Aloc'.
unsigned int GetFourCC_6F1102C0();
struct SLocalAbilitySlot_6F245A90
{
    SLocalAbilitySlot_6F245A90(CAgent* candidate);
    ~SLocalAbilitySlot_6F245A90() { Release(); }
    SLocalAbilitySlot_6F245A90* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F2273C0 ctor -> 0x6F21A700 Assign -> 0x6F159920 'AHwe'.
unsigned int GetFourCC_6F159920();
struct SLocalAbilitySlot_6F2478A0
{
    SLocalAbilitySlot_6F2478A0(CAgent* candidate);
    ~SLocalAbilitySlot_6F2478A0() { Release(); }
    SLocalAbilitySlot_6F2478A0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

//  0x6F228860 ctor -> 0x6F21C280 Assign -> 0x6F0DCCA0 'AIsi'.
unsigned int GetFourCC_6F0DCCA0();
struct SLocalAbilitySlot_6F24A6F0
{
    SLocalAbilitySlot_6F24A6F0(CAgent* candidate);
    ~SLocalAbilitySlot_6F24A6F0() { Release(); }
    SLocalAbilitySlot_6F24A6F0* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

#endif
