//============================================================================
//  Naked pass-through thunks for W3GS action handlers that are declared and
//  dispatched by the reconstructed switches, but not reconstructed yet.
//
//  Without these the tree does not link at all (LNK1120), so no reconstruction
//  anywhere in the repo can be tested in a running game.  Each body is the same
//  shape already used for QueryGroundHeightHelper (item.cpp) and
//  GetItemSlotSingleton (itemhandlemain.cpp): jump to the shipped function at
//  the address the symbol is named after, so the original code runs.
//
//  These are NOT reconstructions and must never be offered as hookable - a
//  detour cannot detour a detour.  Delete each one as its real body lands.
//
//  Generated from the linker's unresolved-symbol list; do not hand-edit.
//============================================================================
#include "cdatastore.h"

extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);

//  0x6F663C70 - reconstructed for real now (trivialaccessors_06.cpp: the
//  whole shipped body is a bare `retn`), so its thunk is gone from here per
//  this file's own rule.

//  0x6F685230 - now reconstructed for real in netmsgio_684.cpp (next to
//  NameField16::ReadFrom, 0x6F684C30, which it wraps), so only a
//  declaration lives here for the caller below.
CDataStore* __fastcall ReadPrefixedField_6F685230(CDataStore* self, unsigned char* out);

//  0x6F665580 - stdcall(self, arg4=dataPtr, arg8=declaredLen), retn 0Ch;
//  `self` is read nowhere in the shipped body - a pure field-length
//  validation with no record side effects.  Builds a real (vtable-carrying)
//  local CDataStore over `dataPtr`/`declaredLen` directly (not through a
//  separate FieldValidator_ helper), reads one prefixed field through the
//  not-in-scope ReadPrefixedField_6F685230, and reports status 2 if the
//  read stayed within the declared length or 1 if it overran - after first
//  rejecting any `declaredLen` outside [0x12, 0x112]. Same unreproducible
//  SEH/`/GS`-cookie frame this whole batch's FieldValidator_ family already
//  carries.
int __stdcall W3GSAction_6F665580(void* self, unsigned int arg4, unsigned int declaredLen)
{
    (void)self;
    //  The local's SEH-frame scope wraps the whole shipped body, including
    //  the early-return range check below - declaring it first (rather than
    //  after the check) is what gets this build to establish that frame
    //  before the check too, matching the shipped instruction order.
    CDataStore local;
    unsigned char scratch[0x13];
    int status;

    if (declaredLen - 0x12 > 0x100)
    {
        status = 1;
    }
    else
    {
        local.m_field4 = (void*)arg4;
        local.m_field8 = 0;
        local.m_field0xC = -1;
        local.m_field10 = declaredLen;
        local.m_readPos = 0;

        ReadPrefixedField_6F685230(&local, scratch);

        status = ((unsigned int)local.m_readPos <= (unsigned int)local.m_field10) ? 2 : 1;

        if (local.m_field0xC != -1)
            g_pfnRecycleBuffer(&local.m_field4, &local.m_field8, &local.m_field0xC);
    }

    return status;
}

//  0x6F667C10 - reconstructed for real now (w3gsaction_6f667c10.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F667F30 - reconstructed for real now (w3gsaction_6f667f30.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F667FC0/0x6F667FF0/0x6F668010/0x6F668030 - reconstructed for real now
//  (w3gsaction_tier1.cpp), so their thunks are gone from here per this
//  file's own rule.

//  0x6F66C4B0 - reconstructed for real now (w3gsaction_6f66c4b0.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F66C500 - reconstructed for real now (w3gsaction_6f66c500.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F66DFE0 - reconstructed for real now (w3gsaction_tier9.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F66E0E0 - reconstructed for real now (w3gsaction_tier10.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F66E240 - reconstructed for real now (w3gsaction_tier4.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F66E300 - now reconstructed for real in w3gsaction_tier25.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F66E430 - reconstructed for real now (w3gsaction_tier4.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F66E4F0 - reconstructed for real now (w3gsaction_tier26.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F66E7A0 - reconstructed for real now (w3gsaction_tier6.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F66E890 - reconstructed for real now (w3gsaction_tier27.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F66EAF0 - shipped code
__declspec(naked) int __stdcall W3GSAction_6F66EAF0(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F66EAF0
        jmp     eax
    }
}

//  0x6F66ED20 - shipped code
__declspec(naked) int __stdcall W3GSAction_6F66ED20(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F66ED20
        jmp     eax
    }
}

//  0x6F66EF60 - reconstructed for real now (w3gsaction_tier28.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F66F050 - reconstructed for real now (w3gsaction_tier28.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F66FBE0 - reconstructed for real now (w3gsaction_tier5.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F66FCA0 is ServerTock_0x27_PacketHandler (Net/servertock_handler.cpp),
//  already reconstructed - funcmap.py has kept the real name for that
//  address since that dump resolved it.  This file used to carry a second
//  naked body for it under the placeholder name, so the dispatcher's case 39
//  reached the shipped code and the reconstruction was never exercised by
//  it.  Gone; the dispatcher calls it by its real name.

//  0x6F66FF90 - reconstructed for real now (w3gsaction_tier29.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F670170 - reconstructed for real now (w3gsaction_tier30.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F671960 - shipped code
__declspec(naked) int __stdcall W3GSAction_6F671960(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F671960
        jmp     eax
    }
}

//  0x6F671D20 - shipped code
__declspec(naked) int __stdcall W3GSAction_6F671D20(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F671D20
        jmp     eax
    }
}

//  0x6F67AA80 - reconstructed for real now (w3gsaction_tier11.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F67AB30 - now reconstructed for real in w3gsaction_tier13.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67AC60 - reconstructed for real now (w3gsaction_tier12.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F67AD90 - reconstructed for real now (w3gsaction_tier8.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F67AE70 - reconstructed for real now (w3gsaction_tier7.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F67AF30 - reconstructed for real now (w3gsaction_tier8.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F67AFF0 - shipped code
__declspec(naked) int __stdcall W3GSAction2_6F67AFF0(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F67AFF0
        jmp     eax
    }
}

//  0x6F67B370 - reconstructed for real now (w3gsaction_tier8.cpp), so
//  its thunk is gone from here per this file's own rule.

//  0x6F67B640 - reconstructed for real now (w3gsaction_tier2.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67B660 - reconstructed for real now (w3gsaction_tier3.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67B710 - reconstructed for real now (w3gsaction_tier31.cpp), so its
//  thunk is gone from here per this file's own rule.

//  0x6F67B830 - reconstructed for real now (w3gsaction_tier3.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67B8C0 - shipped code
__declspec(naked) int __stdcall W3GSAction2_6F67B8C0(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F67B8C0
        jmp     eax
    }
}

//  0x6F67BB60 - reconstructed for real now (w3gsaction_tier2.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67BB80 - reconstructed for real now (w3gsaction_tier3.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67CE30 - now reconstructed for real in w3gsaction_tier23.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67D040 - now reconstructed for real in w3gsaction_tier24.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67D270 - now reconstructed for real in w3gsaction_tier16.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67E8B0 - reconstructed for real now (w3gsaction_tier2.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67E8D0 - reconstructed for real now (w3gsaction_tier2.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F67EC10 - shipped code
__declspec(naked) int __stdcall W3GSAction2_6F67EC10(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F67EC10
        jmp     eax
    }
}

//  0x6F67EE10 - now reconstructed for real in w3gsaction_tier21.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67EF90 - now reconstructed for real in w3gsaction_tier19.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67F0E0 - now reconstructed for real in w3gsaction_tier20.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67F930 - shipped code
__declspec(naked) int __stdcall W3GSAction2_6F67F930(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F67F930
        jmp     eax
    }
}

//  0x6F67FC60 - now reconstructed for real in w3gsaction_tier17.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F67FD30 - now reconstructed for real in w3gsaction_tier18.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F680600 - reconstructed for real now (w3gsaction_tier2.cpp), so its thunk
//  is gone from here per this file's own rule.

//  0x6F680620 - now reconstructed for real in w3gsaction_tier15.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F680740 - now reconstructed for real in w3gsaction_tier22.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F680870 - now reconstructed for real in w3gsaction_tier14.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F680900 - shipped code
__declspec(naked) int __stdcall W3GSAction2_6F680900(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F680900
        jmp     eax
    }
}

//  0x6F680F40 - now reconstructed for real in w3gsaction2_6f680f40.cpp; its
//  thunk is gone from here per this file's own rule.

//  0x6F6817D0 - shipped code
__declspec(naked) int __stdcall W3GSAction2_6F6817D0(void *,unsigned int,unsigned int)
{
    __asm
    {
        mov     eax, 0x6F6817D0
        jmp     eax
    }
}

