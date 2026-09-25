//============================================================================
//  0x6F4D5530 - Sprite.cpp's own module-teardown: release three tagged
//  records (two plain file/line tags, one carrying a real RTTI type
//  descriptor name for the third - "AuChildNode").
//============================================================================
//  0x6F4C1A60 is CDataAllocator::ReleaseAllBlocks (Net/dataallocator.cpp) -
//  reached from several otherwise unrelated modules (this one, Database.cpp,
//  the config profile module), each handing it a small static object plus
//  its own file/line for the free tag.  This file used to carry its own
//  naked redirect to the same address, under a locally-declared empty
//  `TypeTagRecord` struct with a `Destroy` method (Misc/typetagteardown.h,
//  shared with configprofiletypetagteardown.cpp/databasetypetagteardown.
//  cpp) - a duplicate of the real reconstruction; the three globals below
//  really are `CDataAllocator` instances (ReleaseAllBlocks's own third,
//  unused stack argument is the tag/line/flags shape this call site's
//  literal `1` was already passing through unread), so they call it
//  directly now instead.
#include "dataallocator.h"

extern CDataAllocator g_spriteMiniAllocator; // unk_6FAB7EA0 (funcmap.py DATA)
extern CDataAllocator g_spriteUberAllocator; // unk_6FAB7EB4 (funcmap.py DATA)
extern CDataAllocator g_spriteChildNodeTag;  // unk_6FAB7EC8

void TeardownSpriteTypeTags()
{
    g_spriteMiniAllocator.ReleaseAllBlocks(".\\Sprite.cpp", 0x8A9, 1);
    g_spriteUberAllocator.ReleaseAllBlocks(".\\Sprite.cpp", 0x8AA, 1);
    //  IDA's own label ("aAuchildnode") is a truncated auto-name for an
    //  RTTI-mangled string; the exact spelling is not recoverable from
    //  this dump (no `ida_query strings` answer for it yet) and does not
    //  affect the instruction stream either way - only that a string
    //  literal's address is pushed here.
    g_spriteChildNodeTag.ReleaseAllBlocks(".?AUChildNode@@", -2, 1);
}
