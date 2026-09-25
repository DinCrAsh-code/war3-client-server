//============================================================================
//  CGxRawInputBridge's own small callees in the same 0x6F6Cxxxx
//  neighbourhood as CGxRawInputBridge itself (0x6F6C6AA0) - PostInputEvent
//  (0x6F6C6040), CenterCursorInActiveWindow (0x6F6C66E0),
//  CacheLastMousePosition (0x6F6C6680) and MapButtonMessageToBit
//  (0x6F6C65A0).  See wininputbridge.h for what each does.
//
//  Two of these (CacheLastMousePosition, MapButtonMessageToBit) read a
//  second argument through `eax`, which no MSVC calling convention this
//  toolchain can express places an argument in - both are declared and
//  defined with an ordinary register convention instead (the real second
//  argument arriving through `edx`/a normal stack slot) and carry a
//  `DIFFERS` BEHAVIOUR entry for exactly that one instruction-level gap.
//  Everything else about each is a real, working reconstruction.
//============================================================================
#include "wininputbridge.h"

//  0x6F6C6040 - append one event to the 16-slot ring, dropping the oldest
//  unread one if the write pointer has lapped the read pointer.
void __stdcall PostInputEvent(int type, int a, int b, int c, int d)
{
    unsigned int writeIndex = g_inputEventWriteIndex;
    unsigned int nextWrite = (writeIndex == 15) ? 0 : writeIndex + 1;

    if (nextWrite == g_inputEventReadIndex)
    {
        g_inputEventReadIndex =
            (g_inputEventReadIndex == 15) ? 0 : g_inputEventReadIndex + 1;
    }

    SInputEvent* slot = &g_inputEventRing[writeIndex];
    slot->m_type = type;
    slot->m_a = a;
    slot->m_b = b;
    slot->m_c = c;
    slot->m_d = d;

    g_inputEventWriteIndex = nextWrite;
}

//  0x6F6C66E0 - see wininputbridge.h.
void CenterCursorInActiveWindow()
{
    RECT rect;
    HWND hWnd = GetSelectedWindow(0);
    GetWindowRect(hWnd, &rect);

    int cx = rect.right / 2;
    int cy = rect.bottom / 2;

    g_lastCenterX = cx;
    g_lastCenterY = cy;

    SetCursorPos(cx, cy);
}

//  0x6F6C6680 - see wininputbridge.h and this file's own header comment.
void __fastcall CacheLastMousePosition(HWND hWnd, const POINT* point)
{
    int x = point->x;
    int y = point->y;
    g_lastMouseWindow = hWnd;
    g_lastMouseX = x;
    g_lastMouseY = y;
}

//  0x6F6C65A0 - see wininputbridge.h and this file's own header comment.
int __fastcall MapButtonMessageToBit(unsigned int msg, unsigned int* outBit,
                                     unsigned int xButtonInfo)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        *outBit = 1;
        return 1;

    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        *outBit = 2;
        return 1;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        *outBit = 4;
        return 1;

    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    {
        unsigned short button = (unsigned short)(xButtonInfo >> 16);
        if (button == 1)
        {
            *outBit = 8;
            return 1;
        }
        if (button == 2)
        {
            *outBit = 0x10;
            return 1;
        }
        *outBit = 0;
        return 0;
    }

    default:
        *outBit = 0;
        return 0;
    }
}
