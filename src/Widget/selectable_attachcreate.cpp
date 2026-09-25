//============================================================================
//  0x6F2C7950 - CSelectable's vtable slot 101 (+0x194), inherited unchanged
//  by CItem.
//
//  Create this selectable's selection-circle attachment if it does not have
//  one yet, then tell it whether it should currently be lit, and finally -
//  only when the game UI is up and the caller asked for it - run slot 103.
//
//  Five stack arguments (`retn 14h`) of which three are read: `wantUi`
//  gates the CGameUI lookup, `runSlot103` the last call, and `wantCircle`
//  the whole middle block.  The last two are pushed by callers this call
//  tree does not contain and are never read here, so nothing is invented
//  for them beyond making the `ret` count agree - which
//  tools/abi_audit.py's whole FATAL bucket exists to catch.
//
//  Own translation unit: six real calls out of it, none to each other.
//============================================================================
#include "selectable.h"
#include "fvec3.h"
#include "stormnotifysingleton.h"   // NotifyStormSingleton - 0x6F00D990

//  0x6F00A8B0 - configcolor.cpp.
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);
//  0x6F00D9D0 - stormsingleton_attach.cpp.
void __fastcall SetVisualAttachmentColor(int index, const unsigned int* color);

//----------------------------------------------------------------------------
//  0x6F300710 - the lazily-constructed CGameUI singleton (its own source
//  file is named in the shipped allocation tag: ".\\CGameUI.cpp", line
//  0x1AAF).  __fastcall: `create` in ecx, `reset` in edx - the second
//  argument clears the cached pointer on the way out, which is why the
//  callers here pass 1/0.  Behind the same __except_handler4-shaped SEH
//  frame this toolchain cannot reproduce; redirected.
//----------------------------------------------------------------------------
struct SGameUI
{
    char          m_reserved00[0x2A0];
    unsigned int  m_uiUp;           // +0x2A0
    unsigned int  m_circlesLit;     // +0x2A4
};

SGameUI* __fastcall GetGameUI(int create, int reset);

__declspec(naked) SGameUI* __fastcall GetGameUI(int, int)
{
    __asm
    {
        mov     eax, 06F300710h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F00DA90 - create a visual attachment from a scale vector and a world
//  position and hand back its index.  __fastcall (scale in ecx, position in
//  edx) with one stack argument.  A 0x11C-byte frame of Storm-side sprite
//  setup behind a /GS cookie; out of scope, redirected.
//----------------------------------------------------------------------------
int __fastcall CreateVisualAttachment(const FVec3* scale, const FVec3* pos, int flags);

__declspec(naked) int __fastcall CreateVisualAttachment(const FVec3*, const FVec3*, int)
{
    __asm
    {
        mov     eax, 06F00DA90h
        jmp     eax
    }
}

//  slot 66 (+0x108) - the selection-circle radius, in st0.
typedef float (__thiscall *Slot0x108Fn)(void* self);
//  slot 56 (+0xE0) - CWidget::GetPosition.
typedef FVec3* (__thiscall *Slot0xE0Fn)(void* self, FVec3* out, int wantGround,
                                        int a3, int a4);
//  slot 103 (+0x19C) - CSelectable's own, not reconstructed.
typedef void (__thiscall *Slot0x19CFn)(void* self);

void CSelectable::EnsureSelectionCircle(int wantUi, int runSlot103,
                                        int wantCircle, int, int)
{
    //  Two separate CGameUI lookups, not one hoisted local: the shipped
    //  code calls 0x6F300710 again further down rather than keeping the
    //  pointer, which is what two independent `GetGameUI(1,0)->field`
    //  expressions give.
    //  An explicit if/else and not `int uiUp = 0; if (...) uiUp = 1;`: the
    //  shipped code assigns 1 on one edge and 0 on the other
    //  (`mov edi,1` / `jmp` / `xor edi,edi`), where the initialise-then-
    //  overwrite spelling hoists the zero above the whole test and then
    //  reuses that register as the constant every comparison below
    //  compares against, which costs a push and a register.
    int uiUp;
    if (wantUi != 0 && GetGameUI(1, 0)->m_uiUp != 0)
        uiUp = 1;
    else
        uiUp = 0;

    if (wantCircle)
    {
        if (m_attachmentSlot1 == (unsigned int)-1)
        {
            //  The radius is fetched once and used as all three components
            //  of the scale vector - the shipped `fst`/`fst`/`fstp` triple
            //  off one loaded value, which is what assigning one float to
            //  three members gives and three separate calls would not.
            float radius = ((Slot0x108Fn)(*(void***)this)[0x108 / 4])(this);

            FVec3 scale;
            scale.m_x = radius;
            scale.m_y = radius;
            scale.m_z = radius;

            FVec3 pos;
            ((Slot0xE0Fn)(*(void***)this)[0xE0 / 4])(this, &pos, 0, -1, 0);

            m_attachmentSlot1 =
                (unsigned int)CreateVisualAttachment(&scale, &pos, 0);

            unsigned int color;
            GetConfigColor(&color, "SelectionCircle", "ColorNeutral", 0);
            SetVisualAttachmentColor((int)m_attachmentSlot1, &color);
        }

        //  Lit only when the UI is up *and* it says circles are on; the two
        //  are separate fields of the same singleton, read on separate
        //  visits to it.
        int lit;
        if (uiUp != 0 && GetGameUI(1, 0)->m_circlesLit != 0)
            lit = 1;
        else
            lit = 0;

        NotifyStormSingleton((int)m_attachmentSlot1, lit);
    }

    if (uiUp && runSlot103)
        ((Slot0x19CFn)(*(void***)this)[0x19C / 4])(this);
}
