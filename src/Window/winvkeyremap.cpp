//============================================================================
//  0x6F6C6160 - map a raw Win32 virtual-key code onto this module's own
//  internal key-code space: digits/letters/VK_F1..VK_F12 pass through
//  (F-keys offset by +0x290), the three modifiers collapse to 0/1/2
//  (shift/ctrl/alt - the same numbering IsModifierKeyDown/GetSelectedWindow
//  use elsewhere in this module), space passes through as 0x20, and every
//  other named key gets a small 0x100+/0x200+ code of its own.  Every case
//  below is a real, individually-confirmed VK_* constant - cross-checked
//  two ways: IDA's own `; jumptable ... case N` comments (which state the
//  case value in decimal, and read directly as the real VK_ code the same
//  way this repo's other switches already do - docs/msvc-vc8-idioms.md,
//  "A switch's case blocks come out in source order"), and independently
//  against the compaction table's own raw bytes
//  (`ida_query bytes 0x6F6C64C4 215`), whose non-default entries land on
//  exactly the same VK_ codes.
//
//  Non-standard calling convention: the vkey arrives in `ecx` (an ordinary
//  __fastcall first argument) but the out-param pointer arrives in `eax`,
//  which no MSVC calling convention places a second argument in.  Declared
//  and defined with an ordinary `edx` second parameter instead - see
//  wininputbridge.cpp's own comment at the one real call site for how that
//  reconciles - and carries a `DIFFERS` BEHAVIOUR entry for that one gap.
//============================================================================
#include "wininputbridge.h"

int __fastcall MapVKeyToInternal(int vkey, unsigned int* out)
{
    //  Written to match the shipped range-test shapes exactly: a signed
    //  `</<=` pair for the digit range (the parameter really is `int`,
    //  not `unsigned int` - a `jl`/`jle` pair in the dump, not `jb`/`jbe`)
    //  and the canonical `(unsigned)(x-lo) <= hi-lo` single-subtract idiom
    //  for the two letter/F-key ranges.
    if (vkey >= '0' && vkey <= '9')
        goto passthrough;
    if ((unsigned int)(vkey - 'A') <= 0x19)
        goto passthrough;
    if ((unsigned int)(vkey - VK_F1) <= 0xB)
    {
        vkey += 0x290;
        goto passthrough;
    }
    goto notPassthrough;

passthrough:
    *out = (unsigned int)vkey;
    return 1;

notPassthrough:
    switch (vkey)
    {
    case VK_SHIFT:      *out = 0;     return 1;
    case VK_CONTROL:    *out = 1;     return 1;
    case VK_MENU:       *out = 2;     return 1;
    case VK_ESCAPE:    *out = 0x200;    return 1;
    case VK_RETURN:    *out = 0x201;    return 1;
    case VK_BACK:      *out = 0x202;    return 1;
    case VK_TAB:       *out = 0x203;    return 1;
    case VK_LEFT:      *out = 0x204;    return 1;
    case VK_UP:        *out = 0x205;    return 1;
    case VK_RIGHT:     *out = 0x206;    return 1;
    case VK_DOWN:      *out = 0x207;    return 1;
    case VK_INSERT:    *out = 0x208;    return 1;
    case VK_DELETE:    *out = 0x209;    return 1;
    case VK_HOME:      *out = 0x20A;    return 1;
    case VK_END:       *out = 0x20B;    return 1;
    case VK_PRIOR:     *out = 0x20C;    return 1;
    case VK_NEXT:      *out = 0x20D;    return 1;
    case VK_CAPITAL:   *out = 0x20E;    return 1;
    case VK_NUMLOCK:   *out = 0x20F;    return 1;
    case VK_SCROLL:    *out = 0x210;    return 1;
    case VK_PAUSE:     *out = 0x211;    return 1;
    case VK_SNAPSHOT:  *out = 0x212;    return 1;
    case VK_SPACE:     *out = 0x20;     return 1;
    case VK_OEM_3:     *out = 0x100;    return 1;
    case VK_NUMPAD0:   *out = 0x101;    return 1;
    case VK_NUMPAD1:   *out = 0x102;    return 1;
    case VK_NUMPAD2:   *out = 0x103;    return 1;
    case VK_NUMPAD3:   *out = 0x104;    return 1;
    case VK_NUMPAD4:   *out = 0x105;    return 1;
    case VK_NUMPAD5:   *out = 0x106;    return 1;
    case VK_NUMPAD6:   *out = 0x107;    return 1;
    case VK_NUMPAD7:   *out = 0x108;    return 1;
    case VK_NUMPAD8:   *out = 0x109;    return 1;
    case VK_NUMPAD9:   *out = 0x10A;    return 1;
    case VK_ADD:       *out = 0x10B;    return 1;
    case VK_SUBTRACT:  *out = 0x10C;    return 1;
    case VK_MULTIPLY:  *out = 0x10D;    return 1;
    case VK_DIVIDE:    *out = 0x10E;    return 1;
    case VK_DECIMAL:   *out = 0x10F;    return 1;
    case VK_OEM_PLUS:  *out = 0x110;    return 1;
    case VK_OEM_MINUS: *out = 0x111;    return 1;
    case VK_OEM_4:     *out = 0x112;    return 1;
    case VK_OEM_6:     *out = 0x113;    return 1;
    case VK_OEM_5:     *out = 0x114;    return 1;
    case VK_OEM_1:     *out = 0x115;    return 1;
    case VK_OEM_7:     *out = 0x116;    return 1;
    case VK_OEM_COMMA: *out = 0x117;    return 1;
    case VK_OEM_PERIOD: *out = 0x118;    return 1;
    case VK_OEM_2:     *out = 0x119;    return 1;

    default:
        *out = 0;
        return 0;
    }
}
