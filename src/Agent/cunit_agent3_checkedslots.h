//============================================================================
//  cunit_agent3 batch - CUnit closure BFS depth 1-2, thirty-eight
//  near-identical "make one ability-effect agent" factories reached from
//  CUnit's own vtable closure.
//
//  Every member is the same five-call shape widget_slot68.cpp and
//  jassnatives_locationmake.cpp already document for their own agile types:
//  look one ability's own FourCC up in the game-wide agile type registry,
//  take that type record's own allocator, hand pool and type to the global
//  agent factory through the same 0x2C-byte request block, park the CAgent*
//  the returned holder carries at +0x54 in a type-checked scoped slot, and
//  return it - no fields written on the made object, unlike MakeOrderAgent.
//
//  Each has its own constructor and its own Assign, at its own address -
//  the image was linked without /OPT:ICF (agentdefaults.cpp), so identical
//  bodies at distinct addresses stay distinct functions, and they really are
//  distinct C++ types because the required FourCC is a compile-time
//  constant baked into each Assign.  Byte-for-byte the same body as
//  SCheckedItemSlot::Assign (itemtypedassign.cpp) with one operand changed,
//  exactly as agenttypedslots.h's own JASS-handle family is.
//
//  The FourCC is a real, decoded four-character ability rawcode (every one
//  starts with 'A', the WC3 custom-ability convention) - MSB first the way
//  agiletype.h's own family is read - but nothing in this call tree's own
//  dump says which ability object each belongs to, so each type and getter
//  is named after its rawcode rather than a guessed ability name, the same
//  honesty misc_rawcode_getters.cpp already applies to its own unidentified
//  members ("a plausible-looking guess is worse than an honest address").
//
//  Struct names are keyed off the rawcode (unique per member); ctor/Assign
//  addresses are documented on each declaration below and in funcmap.py.
//============================================================================
#ifndef CUNIT_AGENT3_CHECKEDSLOTS_H
#define CUNIT_AGENT3_CHECKEDSLOTS_H

#include "agent.h"

//----------------------------------------------------------------------------
//  'ARal' - ctor 0x  0x6F16DFC0, Assign 0x6F15FF80, type id getter 0x6F027BD0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ARal
{
    //  0x6F16DFC0
    SCheckedAbilitySlot_ARal(CAgent* candidate);

    ~SCheckedAbilitySlot_ARal() { Release(); }

    //  0x6F15FF80
    SCheckedAbilitySlot_ARal* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Aast' - ctor 0x  0x6F220AC0, Assign 0x6F211B00, type id getter 0x6F195120
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Aast
{
    //  0x6F220AC0
    SCheckedAbilitySlot_Aast(CAgent* candidate);

    ~SCheckedAbilitySlot_Aast() { Release(); }

    //  0x6F211B00
    SCheckedAbilitySlot_Aast* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Aadm' - ctor 0x  0x6F220E20, Assign 0x6F211F80, type id getter 0x6F0D99E0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Aadm
{
    //  0x6F220E20
    SCheckedAbilitySlot_Aadm(CAgent* candidate);

    ~SCheckedAbilitySlot_Aadm() { Release(); }

    //  0x6F211F80
    SCheckedAbilitySlot_Aadm* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Abli' - ctor 0x  0x6F221180, Assign 0x6F212400, type id getter 0x6F023DE0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Abli
{
    //  0x6F221180
    SCheckedAbilitySlot_Abli(CAgent* candidate);

    ~SCheckedAbilitySlot_Abli() { Release(); }

    //  0x6F212400
    SCheckedAbilitySlot_Abli* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Adro' - ctor 0x  0x6F092640, Assign 0x6F0849F0, type id getter 0x6F15BB20
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Adro
{
    //  0x6F092640
    SCheckedAbilitySlot_Adro(CAgent* candidate);

    ~SCheckedAbilitySlot_Adro() { Release(); }

    //  0x6F0849F0
    SCheckedAbilitySlot_Adro* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Acmg' - ctor 0x  0x6F221660, Assign 0x6F212A80, type id getter 0x6F1994C0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Acmg
{
    //  0x6F221660
    SCheckedAbilitySlot_Acmg(CAgent* candidate);

    ~SCheckedAbilitySlot_Acmg() { Release(); }

    //  0x6F212A80
    SCheckedAbilitySlot_Acmg* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ACrn' - ctor 0x  0x6F2218A0, Assign 0x6F212D80, type id getter 0x6F1D3020
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ACrn
{
    //  0x6F2218A0
    SCheckedAbilitySlot_ACrn(CAgent* candidate);

    ~SCheckedAbilitySlot_ACrn() { Release(); }

    //  0x6F212D80
    SCheckedAbilitySlot_ACrn* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Adef' - ctor 0x  0x6F221C60, Assign 0x6F213280, type id getter 0x6F07C720
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Adef
{
    //  0x6F221C60
    SCheckedAbilitySlot_Adef(CAgent* candidate);

    ~SCheckedAbilitySlot_Adef() { Release(); }

    //  0x6F213280
    SCheckedAbilitySlot_Adef* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Adis' - ctor 0x  0x6F221F00, Assign 0x6F213600, type id getter 0x6F0D98E0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Adis
{
    //  0x6F221F00
    SCheckedAbilitySlot_Adis(CAgent* candidate);

    ~SCheckedAbilitySlot_Adis() { Release(); }

    //  0x6F213600
    SCheckedAbilitySlot_Adis* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Aeye' - ctor 0x  0x6F222200, Assign 0x6F213A00, type id getter 0x6F07F980
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Aeye
{
    //  0x6F222200
    SCheckedAbilitySlot_Aeye(CAgent* candidate);

    ~SCheckedAbilitySlot_Aeye() { Release(); }

    //  0x6F213A00
    SCheckedAbilitySlot_Aeye* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ANgl' - ctor 0x  0x6F223220, Assign 0x6F214F80, type id getter 0x6F026970
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ANgl
{
    //  0x6F223220
    SCheckedAbilitySlot_ANgl(CAgent* candidate);

    ~SCheckedAbilitySlot_ANgl() { Release(); }

    //  0x6F214F80
    SCheckedAbilitySlot_ANgl* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ANsa' - ctor 0x  0x6F224060, Assign 0x6F216280, type id getter 0x6F113F70
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ANsa
{
    //  0x6F224060
    SCheckedAbilitySlot_ANsa(CAgent* candidate);

    ~SCheckedAbilitySlot_ANsa() { Release(); }

    //  0x6F216280
    SCheckedAbilitySlot_ANsa* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Aamk' - ctor 0x  0x6F224F00, Assign 0x6F217600, type id getter 0x6F0DCAA0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Aamk
{
    //  0x6F224F00
    SCheckedAbilitySlot_Aamk(CAgent* candidate);

    ~SCheckedAbilitySlot_Aamk() { Release(); }

    //  0x6F217600
    SCheckedAbilitySlot_Aamk* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AUav' - ctor 0x  0x6F225200, Assign 0x6F217A00, type id getter 0x6F196790
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AUav
{
    //  0x6F225200
    SCheckedAbilitySlot_AUav(CAgent* candidate);

    ~SCheckedAbilitySlot_AUav() { Release(); }

    //  0x6F217A00
    SCheckedAbilitySlot_AUav* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AHbz' - ctor 0x  0x6F225500, Assign 0x6F217E00, type id getter 0x6F197B80
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AHbz
{
    //  0x6F225500
    SCheckedAbilitySlot_AHbz(CAgent* candidate);

    ~SCheckedAbilitySlot_AHbz() { Release(); }

    //  0x6F217E00
    SCheckedAbilitySlot_AHbz* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AOcr' - ctor 0x  0x6F2257A0, Assign 0x6F218180, type id getter 0x6F19A8C0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AOcr
{
    //  0x6F2257A0
    SCheckedAbilitySlot_AOcr(CAgent* candidate);

    ~SCheckedAbilitySlot_AOcr() { Release(); }

    //  0x6F218180
    SCheckedAbilitySlot_AOcr* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AUin' - ctor 0x  0x6F225AA0, Assign 0x6F218580, type id getter 0x6F10F0F0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AUin
{
    //  0x6F225AA0
    SCheckedAbilitySlot_AUin(CAgent* candidate);

    ~SCheckedAbilitySlot_AUin() { Release(); }

    //  0x6F218580
    SCheckedAbilitySlot_AUin* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AEer' - ctor 0x  0x6F225DA0, Assign 0x6F218980, type id getter 0x6F07F330
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AEer
{
    //  0x6F225DA0
    SCheckedAbilitySlot_AEer(CAgent* candidate);

    ~SCheckedAbilitySlot_AEer() { Release(); }

    //  0x6F218980
    SCheckedAbilitySlot_AEer* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AEfn' - ctor 0x  0x6F2260A0, Assign 0x6F218D80, type id getter 0x6F081BE0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AEfn
{
    //  0x6F2260A0
    SCheckedAbilitySlot_AEfn(CAgent* candidate);

    ~SCheckedAbilitySlot_AEfn() { Release(); }

    //  0x6F218D80
    SCheckedAbilitySlot_AEfn* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ANht' - ctor 0x  0x6F2263A0, Assign 0x6F219180, type id getter 0x6F082C70
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ANht
{
    //  0x6F2263A0
    SCheckedAbilitySlot_ANht(CAgent* candidate);

    ~SCheckedAbilitySlot_ANht() { Release(); }

    //  0x6F219180
    SCheckedAbilitySlot_ANht* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AHmt' - ctor 0x  0x6F226700, Assign 0x6F219600, type id getter 0x6F025E50
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AHmt
{
    //  0x6F226700
    SCheckedAbilitySlot_AHmt(CAgent* candidate);

    ~SCheckedAbilitySlot_AHmt() { Release(); }

    //  0x6F219600
    SCheckedAbilitySlot_AHmt* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AHre' - ctor 0x  0x6F226A00, Assign 0x6F219A00, type id getter 0x6F1D3740
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AHre
{
    //  0x6F226A00
    SCheckedAbilitySlot_AHre(CAgent* candidate);

    ~SCheckedAbilitySlot_AHre() { Release(); }

    //  0x6F219A00
    SCheckedAbilitySlot_AHre* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AOsf' - ctor 0x  0x6F226D00, Assign 0x6F219E00, type id getter 0x6F1D7A10
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AOsf
{
    //  0x6F226D00
    SCheckedAbilitySlot_AOsf(CAgent* candidate);

    ~SCheckedAbilitySlot_AOsf() { Release(); }

    //  0x6F219E00
    SCheckedAbilitySlot_AOsf* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AUts' - ctor 0x  0x6F227000, Assign 0x6F21A200, type id getter 0x6F156A20
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AUts
{
    //  0x6F227000
    SCheckedAbilitySlot_AUts(CAgent* candidate);

    ~SCheckedAbilitySlot_AUts() { Release(); }

    //  0x6F21A200
    SCheckedAbilitySlot_AUts* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ANwm' - ctor 0x  0x6F227420, Assign 0x6F21A780, type id getter 0x6F1599A0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ANwm
{
    //  0x6F227420
    SCheckedAbilitySlot_ANwm(CAgent* candidate);

    ~SCheckedAbilitySlot_ANwm() { Release(); }

    //  0x6F21A780
    SCheckedAbilitySlot_ANwm* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ANde' - ctor 0x  0x6F2276C0, Assign 0x6F21AB00, type id getter 0x6F157900
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ANde
{
    //  0x6F2276C0
    SCheckedAbilitySlot_ANde(CAgent* candidate);

    ~SCheckedAbilitySlot_ANde() { Release(); }

    //  0x6F21AB00
    SCheckedAbilitySlot_ANde* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'ANso' - ctor 0x  0x6F2279C0, Assign 0x6F21AF00, type id getter 0x6F080800
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_ANso
{
    //  0x6F2279C0
    SCheckedAbilitySlot_ANso(CAgent* candidate);

    ~SCheckedAbilitySlot_ANso() { Release(); }

    //  0x6F21AF00
    SCheckedAbilitySlot_ANso* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIat' - ctor 0x  0x6F227CC0, Assign 0x6F21B300, type id getter 0x6F0DC820
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIat
{
    //  0x6F227CC0
    SCheckedAbilitySlot_AIat(CAgent* candidate);

    ~SCheckedAbilitySlot_AIat() { Release(); }

    //  0x6F21B300
    SCheckedAbilitySlot_AIat* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIfe' - ctor 0x  0x6F227FC0, Assign 0x6F21B700, type id getter 0x6F0DD7A0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIfe
{
    //  0x6F227FC0
    SCheckedAbilitySlot_AIfe(CAgent* candidate);

    ~SCheckedAbilitySlot_AIfe() { Release(); }

    //  0x6F21B700
    SCheckedAbilitySlot_AIfe* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIfs' - ctor 0x  0x6F2282C0, Assign 0x6F21BB00, type id getter 0x6F0801D0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIfs
{
    //  0x6F2282C0
    SCheckedAbilitySlot_AIfs(CAgent* candidate);

    ~SCheckedAbilitySlot_AIfs() { Release(); }

    //  0x6F21BB00
    SCheckedAbilitySlot_AIfs* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIml' - ctor 0x  0x6F2285C0, Assign 0x6F21BF00, type id getter 0x6F0DCD20
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIml
{
    //  0x6F2285C0
    SCheckedAbilitySlot_AIml(CAgent* candidate);

    ~SCheckedAbilitySlot_AIml() { Release(); }

    //  0x6F21BF00
    SCheckedAbilitySlot_AIml* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIsb' - ctor 0x  0x6F2288C0, Assign 0x6F21C300, type id getter 0x6F0DC400
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIsb
{
    //  0x6F2288C0
    SCheckedAbilitySlot_AIsb(CAgent* candidate);

    ~SCheckedAbilitySlot_AIsb() { Release(); }

    //  0x6F21C300
    SCheckedAbilitySlot_AIsb* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIdi' - ctor 0x  0x6F228BC0, Assign 0x6F21C700, type id getter 0x6F0D9A60
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIdi
{
    //  0x6F228BC0
    SCheckedAbilitySlot_AIdi(CAgent* candidate);

    ~SCheckedAbilitySlot_AIdi() { Release(); }

    //  0x6F21C700
    SCheckedAbilitySlot_AIdi* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIrt' - ctor 0x  0x6F228EC0, Assign 0x6F21CB00, type id getter 0x6F025F90
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIrt
{
    //  0x6F228EC0
    SCheckedAbilitySlot_AIrt(CAgent* candidate);

    ~SCheckedAbilitySlot_AIrt() { Release(); }

    //  0x6F21CB00
    SCheckedAbilitySlot_AIrt* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIte' - ctor 0x  0x6F2291C0, Assign 0x6F21CF00, type id getter 0x6F025FF0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIte
{
    //  0x6F2291C0
    SCheckedAbilitySlot_AIte(CAgent* candidate);

    ~SCheckedAbilitySlot_AIte() { Release(); }

    //  0x6F21CF00
    SCheckedAbilitySlot_AIte* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIgo' - ctor 0x  0x6F229460, Assign 0x6F21D280, type id getter 0x6F0DD3A0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIgo
{
    //  0x6F229460
    SCheckedAbilitySlot_AIgo(CAgent* candidate);

    ~SCheckedAbilitySlot_AIgo() { Release(); }

    //  0x6F21D280
    SCheckedAbilitySlot_AIgo* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'AIbl' - ctor 0x  0x6F229760, Assign 0x6F21D680, type id getter 0x6F0D92A0
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_AIbl
{
    //  0x6F229760
    SCheckedAbilitySlot_AIbl(CAgent* candidate);

    ~SCheckedAbilitySlot_AIbl() { Release(); }

    //  0x6F21D680
    SCheckedAbilitySlot_AIbl* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//----------------------------------------------------------------------------
//  'Amai' - ctor 0x  0x6F280FC0, Assign 0x6F275830, type id getter 0x6F028940
//  (see cunit_agent3_abilityids.h for the exact rawcode).
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Amai
{
    //  0x6F280FC0
    SCheckedAbilitySlot_Amai(CAgent* candidate);

    ~SCheckedAbilitySlot_Amai() { Release(); }

    //  0x6F275830
    SCheckedAbilitySlot_Amai* Assign(CAgent* candidate);

    //  Separate decrement-then-test, not `--x==0`: the shipped inline
    //  release at every one of this family's own call sites is
    //  `add [x+4],-1` / `mov ecx,eax` / `add eax,4` / `cmp [eax],0` / `jnz`,
    //  the same re-read shape SCheckedLocationSlot's own Release()
    //  (agenttypedslots.h) documents.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

#endif
