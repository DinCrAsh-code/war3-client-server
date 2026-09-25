//============================================================================
//  0x6F537840 - format a diagnostic into a 256-byte buffer and post it as a
//  system message of type 5.
//
//  `__cdecl` and variadic - the caller pops - and the formatting is the CRT
//  import at 0x6F86D3E0 (`call ds:__imp__vsnprintf`), not one of Storm's
//  string ordinals.  The message goes to the same sink AUMsgBuffer's family
//  uses (0x6F4CBF60, PostSysMessage in Missile/missile_apply.cpp): text in
//  ecx, message type in edx.  The missile path posts type 3 and this one
//  posts 5.
//
//  The 0x100-byte buffer is what puts a /GS cookie in the shipped prologue -
//  the cookie load, the `xor esp` and the check call.  This repo builds at a
//  fixed /GS-, so those three instructions and the stack-slot displacement
//  they cause cannot come back from any source shape.
//
//  Own translation unit: CNetSession::SetGameState calls it for real.
//============================================================================
#include "netdata.h"
#include <stdio.h>
#include <stdarg.h>

//  0x6F4CBF60 - post one already-formatted line into the system message
//  list (Misc/sysmessagepost.cpp).  Declared exactly as that file defines
//  it - C++ linkage, and `int` rather than `void`: MSVC puts both the
//  linkage and the return type into the mangled name, so a second spelling
//  here would be a second symbol for one shipped address.
int __fastcall PostSysMessage(const char* message, int type);

#pragma warning(push)
#pragma warning(disable: 4996)   //  _vsnprintf "deprecated" - VC8's own
                                 //  replacement is _vsnprintf_s, and the
                                 //  shipped code calls this one.
void SysMessagePrintf(const char* format, ...)
{
    char buffer[0x100];
    va_list args;

    va_start(args, format);
    _vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    PostSysMessage(buffer, 5);
}
#pragma warning(pop)
