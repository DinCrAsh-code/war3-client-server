//============================================================================
//  0x6F378C80 - AcquireUnitTip.
//
//  The lazily-created unit tip.  Once it exists the whole body is one load
//  and a return, which is why the "already there" edge jumps straight to the
//  epilogue's *reload* of the global rather than keeping the pointer it just
//  tested.
//
//  Whether the tip is currently up (+0x94) decides whether the revision bump
//  at +0x84 has to be bracketed by a hide and a show: the pair of vtable
//  slots 0x64/0x68 with +0x90 set to 0 and then 1 either side of it is what
//  names them.
//
//  The scope table's tail-call to nullsub_45 says the `__try` here has an
//  *empty* `__finally`, the same shape AgentSlotArrayAppend has - and it is
//  what puts the `__except_handler4`-shaped frame this toolchain cannot
//  reproduce around the body (docs/msvc-vc8-idioms.md).
//
//  The allocation's `__FILE__` tag is truncated in the dump
//  ("e:\\Drive1\\temp\\buildwar3x\\War3\\Sou"...), so the string below is
//  the visible prefix completed by convention.  It is a Storm leak-log tag
//  and nothing reads it; both sides canonicalise the operand to an anonymous
//  symbol either way.
//
//  Own translation unit: 0x6F35B380 is a real call and a different shipped
//  module.
//============================================================================
#include "gameui.h"
#include "storm.h"
#include <excpt.h>

//: dword_6FAB6298 - the singleton.
extern CUnitTip* g_pUnitTip;

//  slot 0x64 / 0x68 of the tip: show and hide.
typedef void (__thiscall *TipToggleFn)(void* self);

static const char kUnitTipFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\CUnitTip.cpp";

CUnitTip* AcquireUnitTip()
{
    if (g_pUnitTip != 0)
        return g_pUnitTip;

    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);

    CUnitTip* tip;

    __try
    {
        void* block = SMemAlloc(0x150, kUnitTipFile, 0x31, 0);

        if (block != 0)
            tip = ((CUnitTip*)block)->Construct(ui->m_unitTipStyle);
        else
            tip = 0;
    }
    __finally
    {
    }

    g_pUnitTip = tip;

    int shown = tip->m_shown;
    int revision = tip->m_revision;

    if (shown != 0)
    {
        tip->m_hidden = 0;
        ((TipToggleFn)(*(void***)tip)[0x64 / 4])(tip);
    }

    tip->m_revision = revision + 2;

    if (shown != 0)
    {
        tip->m_hidden = 1;
        ((TipToggleFn)(*(void***)tip)[0x68 / 4])(tip);
    }

    return g_pUnitTip;
}
