//============================================================================
//  CFrame's own slots 25/26 (+0x64/+0x68) - a real, shared "arm/disarm this
//  layer's own shortcut table" pair (same address on a fresh CSimpleTop and
//  CWorldFrameWar3 pull), mirroring CSimpleTop's own
//  RegisterShortcuts/UnregisterShortcuts (Frame/simpletopshortcuts.cpp) one
//  level down, at the per-CLayer level.  Bit 4 of the CLayer state word
//  (m_stateBits, clayer.h) tracks whether this layer's own table is
//  currently armed, so a repeat call is a no-op.
//
//  Slot 26 additionally releases the keyboard first (SetKeyboardFocus(0,0,0),
//  already named on CLayer) when this layer currently holds it
//  (dword_6FACE67C = g_pKeyboardFocusLayer, already named).
//
//  The two Storm-registration helpers (0x6F6012F0/0x6F601420) are real but
//  left naked thunks - both walk m_shortcuts (SHORTCUTREGTABLE, clayer.h)
//  registering/unregistering every entry against the global shortcut
//  dispatch table, which this session's time budget did not extend to.
//============================================================================
#include "frame.h"

#define FRAME_SHORTCUTFLAG_THUNK(addr)   \
    {                                    \
        __asm { mov eax, addr }         \
        __asm { jmp eax }               \
    }

//  0x6F6012F0 / 0x6F601420 - see this file's own header comment.
__declspec(naked) void CFrame::ArmShortcutTable(void*, void*) FRAME_SHORTCUTFLAG_THUNK(0x6F6012F0)
__declspec(naked) void CFrame::DisarmShortcutTable(void*, void*) FRAME_SHORTCUTFLAG_THUNK(0x6F601420)

//  +0x7C / +0x8C are each their own list's own "tail" word (m_list74's own
//  +8 and m_shortcuts' inner TSExplicitList's own +8 respectively) - "is
//  this list non-empty", read raw rather than through either type because
//  m_shortcuts is deliberately untyped storage (clayer.h's own comment on
//  why) and m_list74 is a plain CFrameNodeList with no such "am I empty"
//  accessor of its own.
void CFrame::Method_0x64()
{
    if (*(int*)((char*)this + 0x7C) <= 0)
    {
        if (*(int*)((char*)this + 0x8C) <= 0)
            return;
    }

    ArmShortcutTable((char*)this + 0x74, (char*)this + 0x80);
    m_stateBits |= 0x10;
}

void CFrame::Method_0x68()
{
    if (g_pKeyboardFocusLayer == this)
        SetKeyboardFocus(0, 0, 0);

    if (m_stateBits & 0x10)
    {
        DisarmShortcutTable((char*)this + 0x74, (char*)this + 0x80);
        m_stateBits &= ~0x10u;
    }
}
