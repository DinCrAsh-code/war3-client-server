//============================================================================
//  0x6F4CC130 - ReportStatusToSysMessage: turn whatever a CStatus collected
//  into one system message.
//
//  An empty status is success and nothing is shown - which is why the early
//  answer is 1 and not 0: the return value is "everything is fine", not
//  "a message was posted".
//
//  Otherwise the entries are joined into one Storm-allocated string, that
//  string is posted, and the buffer is freed with *this* module's own
//  ".\\SysMessage.cpp" tag rather than with the ".\\Status.cpp" one
//  CStatus::BuildText allocated it under - the shipped code does exactly
//  that, and it is worth saying so because it looks like a mistake and is
//  not: Storm's log tag is per call site, not per block.
//
//  The message kind is read off the status through the same +0x0C
//  indirection Misc/trivialpredicates.cpp already names, *after* the text
//  has been built: the shipped stream orders the two that way, and the build
//  cannot change it because it allocates.
//============================================================================
#include "cstatus.h"

//  0x6F4C3680 - Misc/trivialpredicates.cpp.
int __fastcall GetIndirectField0xC_6F4C3680(const void* self);
//  0x6F4CBF60 - PostSysMessage, Misc/sysmessagepost.cpp.  `retn 0`: the text
//  in ecx and the kind in edx, nothing pushed.  This file used to declare it
//  locally as `ShowSysMessageText`, which was a second C++ name for one
//  shipped address: two mangled symbols, so the call site lost its readable
//  operand and this function fell from 28/28 to 27/28 the moment the real
//  body landed.  One address, one name.
int __fastcall PostSysMessage(const char* message, int type);

static const char kSysMessageFile[] = ".\\SysMessage.cpp";

int __fastcall ReportStatusToSysMessage(CStatus* status)
{
    if (status->IsEmpty())
        return 1;

    char* text = status->BuildText(0);

    int posted = PostSysMessage(text,
                                GetIndirectField0xC_6F4C3680(status));

    SMemFree(text, kSysMessageFile, 0x94, 0);
    return posted;
}
