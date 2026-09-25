//============================================================================
//  0x6F5C9650 - resolve a game string into a caller-supplied buffer.
//
//  Two tables, in order: the override table at 0x6FACD23C first, the base
//  table at 0x6FACD214 behind it, and an override entry that exists but
//  carries a null value falls through to the base rather than answering.
//  The buffer is emptied before either lookup, so a caller that ignores the
//  return value sees "" and not whatever was there.
//
//  `__fastcall`: key in ecx, buffer in edx, size on the stack.  It is what
//  CGameUI::SetGamePaused resolves PAUSE_GAME_NOTIFY /
//  PAUSE_GAME_NOTIFY_NO_TIMEOUT / RESUME_GAME_NOTIFY with, and what the
//  pause button's caption comes from.
//
//  Own translation unit: two real calls to the table's own lookup.
//============================================================================
#include "gamestrings.h"

int __fastcall GetGameString(const char* key, char* buffer, int size)
{
    if (key == 0 || *key == 0 || buffer == 0 || size == 0)
        return 0;

    *buffer = 0;

    STRINGHASHNODE* node = g_gameStringOverrides.Ptr(key);
    if (node == 0 || node->m_value == 0)
        node = g_gameStrings.Ptr(key);

    if (node == 0)
        return 0;

    const char* value = node->m_value;
    if (value == 0)
        return 0;

    Storm_501(buffer, value, size);
    return 1;
}
