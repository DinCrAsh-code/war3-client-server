//============================================================================
//  0x6F4CBF60 - PostSysMessage: put one already-formatted line into the
//  system message log.
//
//  Two states, and the whole function is the difference between them.
//  Before the message *frame* exists there is nowhere to draw, so the line
//  is queued: one MSGBUFFER node on the head of the list at 0x6FA860A4,
//  with its text and type handed to AUMsgBuffer::SetText.  Once the frame
//  is up, the queue is drained into it first - oldest last, walking the
//  list from its tail - and then the new line goes straight in, and the
//  queue is freed outright.
//
//  Both posts go through the same filter and the same colour build: a
//  message is shown only when its type reaches the current threshold
//  (dword_6FA8609C), and when the "exact type only" flag
//  (dword_6FAB7D2C) is up it has to *equal* it; the colour is three bytes
//  out of a per-type table packed under an opaque alpha, and the lifetime
//  is a float out of the same table's fourth word.
//
//  The table is spelled as four separate arrays for the reason
//  docs/msvc-vc8-idioms.md records: IDA gives each of the three bytes its
//  own symbol, and the canonicaliser compares operand symbols.  Each array's
//  element carries the whole eight-byte stride with the half it does not own
//  left unnamed.
//
//  This is where the class name MSGBUFFER comes from, and it corrects one:
//  the allocation tag at 0x6FA860B8 is ".?AUMSGBUFFER@@", so IDA's label
//  `aAumsgbuffer` is the `AU` of the `struct` mangling prefix glued to the
//  name.  Misc/sysmessage.h calls the class `AUMsgBuffer` and this session
//  has not renamed it - that would move a hookable key belonging to another
//  target - but the bytes say MSGBUFFER.
//============================================================================
#include "sysmessage.h"
#include "messageframe.h"
#include "storm.h"
#include <stdio.h>
#include <stdarg.h>

//  0x6F4CB920 / 0x6F60A030 - Misc/misc_trivial_getters.cpp's own two
//  global reads, under the names this repo already binds to them.
//  Both spelled exactly as misc_accessors.cpp defines them - MSVC puts the
//  calling convention and the return type in the mangled name, so
//  `void* ReadUnk6FACE754()` is a *different symbol* from the real
//  `int __fastcall ReadUnk6FACE754()` and resolves to nothing at link time
//  (tools/link_check.py's first check; verify.py cannot see it).
int   GetGlobal_6F4CB920();
int __fastcall ReadUnk6FACE754();

struct SMessageFrameHost
{
    CMessageFrame* EnsureMessageFrame();   // 0x6F60A600
};

//  unk_6FA860A4 - the queue.  Declared, never defined: it is the real
//  game's global (tools/link_check.py's fifth check).
extern AUMsgList g_sysMessages;

//  dword_6FA8609C / dword_6FAB7D2C - the display threshold and the
//  "exactly this type" switch.
extern int g_sysMessageMinType;
extern int g_sysMessageExactType;

//  The per-type table at 0x6FA86060: three colour bytes and a lifetime,
//  eight bytes to the entry.
struct SMsgColourByte { unsigned char m_value; char m_rest[7]; };
struct SMsgTimerSlot  { float m_value; char m_rest[4]; };

extern const SMsgColourByte g_sysMessageRed[];     // byte_6FA86060
extern const SMsgColourByte g_sysMessageGreen[];   // byte_6FA86061
extern const SMsgColourByte g_sysMessageBlue[];    // byte_6FA86062
extern const SMsgTimerSlot  g_sysMessageTimer[];   // flt_6FA86064

int __fastcall PostSysMessage(const char* message, int type)
{
    if (GetGlobal_6F4CB920() == 0)
        return 0;

    SMessageFrameHost* host = (SMessageFrameHost*)ReadUnk6FACE754();
    if (host == 0)
    {
        AUMsgBuffer* node = (AUMsgBuffer*)SMemAlloc(0x14, ".?AUMSGBUFFER@@",
                                                    -2, 8);
        if (node != 0)
        {
            node->m_link.m_next = 0;
            node->m_timer = 0.0f;
            node->m_link.m_prevlink = 0;
            node->m_text = 0;
            node->m_type = 3;
        }

        ((TSList<AUMsgBuffer, 0>*)&g_sysMessages)->LinkToHead(node);
        node->SetText(message, type);
        return 1;
    }

    CMessageFrame* frame = host->EnsureMessageFrame();

    AUMsgBuffer* node = g_sysMessages.m_terminator.m_prevlink > 0
                            ? (AUMsgBuffer*)g_sysMessages.m_terminator.m_prevlink
                            : 0;
    while ((int)node > 0)
    {
        const char* text = node->m_text;
        if (text != 0)
        {
            int shown = node->m_type;
            if (shown >= g_sysMessageMinType
                && (g_sysMessageExactType == 0
                    || shown == g_sysMessageMinType))
            {
                int colour = (int)(((((0xFFFFFF00u
                                       | g_sysMessageRed[shown].m_value) << 8)
                                     | g_sysMessageGreen[shown].m_value) << 8)
                                   | g_sysMessageBlue[shown].m_value);
                frame->AddMessage(text, g_sysMessageTimer[shown].m_value, 0,
                                  &colour, 0);
            }
        }
        node = (AUMsgBuffer*)node->m_link.m_prevlink;
    }

    g_sysMessages.FreeAll();

    if (type >= g_sysMessageMinType
        && (g_sysMessageExactType == 0 || type == g_sysMessageMinType))
    {
        int colour = (int)(((((0xFFFFFF00u | g_sysMessageRed[type].m_value) << 8)
                             | g_sysMessageGreen[type].m_value) << 8)
                           | g_sysMessageBlue[type].m_value);
        frame->AddMessage(message, g_sysMessageTimer[type].m_value, 0,
                          &colour, 0);
    }

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4CC180 - format `format`/`args` into a stack buffer and post the
//  result through PostSysMessage above, with `type` carried straight
//  through as the message type.  `__fastcall`, `retn 4`: `type` in ecx,
//  `format` in edx, the already-built `va_list` on the stack - the "V"
//  half of a printf/vprintf pair, called by SLogWarningV's own naked
//  redirect (0x6F4CC1E0, Widget/widgetsoundlabel.cpp) after it builds the
//  va_list from its own `...` tail.
//
//  The buffer is one byte longer than the count handed to `_vsnprintf`
//  (0x100 requested, a 0x101-byte array) with that last byte forced to 0
//  right before the post - `_vsnprintf` does not itself guarantee
//  termination when the formatted text fills or overruns the buffer, and
//  the shipped code's extra `mov byte ptr ..., 0` right after the call is
//  exactly that belt-and-braces truncation.
//
//  The /GS buffer cookie (dword_6FAAE140 load, `xor esp`, the
//  SecurityCheckCookie call on the way out) is the same loss every other
//  buffer-carrying leaf in this tree has at this repo's fixed /GS-; see
//  Net/netsysmessagef.cpp's SysMessagePrintf for the sibling that takes
//  `...` directly instead of an already-built va_list.
//----------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4996)   //  _vsnprintf "deprecated" - the shipped
                                 //  code calls this CRT import, not the
                                 //  VC8-added _vsnprintf_s.
void __fastcall SysMessageVPrintf(int type, const char* format, va_list args)
{
    char buffer[0x101];

    _vsnprintf(buffer, 0x100, format, args);
    buffer[0x100] = 0;

    PostSysMessage(buffer, type);
}
#pragma warning(pop)
