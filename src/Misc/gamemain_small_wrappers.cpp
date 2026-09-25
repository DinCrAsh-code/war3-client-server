//============================================================================
//  Three small, unrelated one-Storm-call leaves and two static-destructor
//  stubs, grouped in one TU for the batch (none call each other or share
//  state, and each is too small to be its own module - the same reasoning
//  Misc/sweep_field_setters.cpp already uses for its own grouped writers).
//============================================================================

#include "dataallocator.h"

//  Storm ordinal 472/474 - unresolved real names, kept as the dump's own
//  Storm_NNN spelling per this repo's convention for every other
//  not-yet-identified ordinal (Storm_501/506/508/578 etc).
void __stdcall Storm_474(const char* src, char* dest, unsigned int maxLen);
int  __stdcall Storm_472(const char* text);
void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);

//----------------------------------------------------------------------------
//  0x6F4C19D0 - copy the incoming text into a fixed 0x104-byte static
//  buffer and hand the buffer's own address back.
//----------------------------------------------------------------------------
extern char g_cachedText[0x104];   // unk_6FAB7AF0

char* __fastcall CacheTextAndReturn(const char* text)
{
    Storm_474(text, g_cachedText, 0x104);
    return g_cachedText;
}

//----------------------------------------------------------------------------
//  0x6F4C19C0 - a pure one-argument forwarder into Storm ordinal 472.
//----------------------------------------------------------------------------
int __fastcall ForwardToStorm472(const char* text)
{
    return Storm_472(text);
}

//----------------------------------------------------------------------------
//  0x6F57DA00 - copy the incoming text into a fixed buffer and remember two
//  more values (a handle-shaped `edx` and a stack argument) in two module
//  globals.
//----------------------------------------------------------------------------
extern char g_cachedText2[];    // byte_6FACCD70 - size not established by this call tree
extern int  g_cachedHandle;     // dword_6FA8B104
extern int  g_cachedExtra;      // dword_6FA8B420

void __fastcall CacheTextAndTwoValues(const char* text, int handle, int extra)
{
    Storm_501(g_cachedText2, text, 0x7FFFFFFF);
    g_cachedHandle = handle;
    g_cachedExtra = extra;
}

//----------------------------------------------------------------------------
//  0x6F4C1A60 is CDataAllocator::ReleaseAllBlocks (Net/dataallocator.cpp,
//  gamemain-batch-3's own real reconstruction of this address - see its own
//  header comment).  This file used to carry its own naked redirect to the
//  same address, routed through a minimal opaque `CDataAllocatorReleaser`
//  receiver rather than `CDataAllocator` itself (this file predates that
//  reconstruction landing) - a duplicate; the two static-dtor stubs below
//  now call the real method directly on their own already-typed
//  `CDataAllocator` globals.
//----------------------------------------------------------------------------
//  0x6F4CCEC0 - TextBlock.cpp's own static-destructor stub: release every
//  block CDataAllocator instance unk_6FAB7D30 still owns.
//----------------------------------------------------------------------------
//  g_textBlockPool - Render/text.h's own CDataAllocator (unk_6FAB7D30),
//  already declared there.
extern CDataAllocator g_textBlockPool;
static const char aTextblockCpp[] = ".\\TextBlock.cpp";

void TextBlockAllocatorStaticDtor()
{
    g_textBlockPool.ReleaseAllBlocks(aTextblockCpp, 0x1DF, 1);
}

//----------------------------------------------------------------------------
//  0x6F4C4000 - AUStatusEntry's own static-destructor stub: run its own two
//  (below this batch's own 43) module teardown calls first, then release
//  every block its CDataAllocator instance still owns.
//----------------------------------------------------------------------------
//  g_statusEntryPool - Render/cstatus.h's own CDataAllocator (unk_6FAB7D00).
extern CDataAllocator g_statusEntryPool;
static const char aAustatusentryC[] = ".?AVAUStatusEntry@@";   // type descriptor name

__declspec(naked) void StatusEntryModuleTeardownA()   // 0x6F4BEA40
{
    __asm { mov eax, 06F4BEA40h }
    __asm { jmp eax }
}
__declspec(naked) void StatusEntryModuleTeardownB()   // 0x6F4C3490
{
    __asm { mov eax, 06F4C3490h }
    __asm { jmp eax }
}
__declspec(naked) void StatusEntryModuleTeardownC()   // 0x6F4C4500
{
    __asm { mov eax, 06F4C4500h }
    __asm { jmp eax }
}

void StatusEntryStaticDtor()
{
    StatusEntryModuleTeardownA();
    StatusEntryModuleTeardownB();
    StatusEntryModuleTeardownC();
    g_statusEntryPool.ReleaseAllBlocks(aAustatusentryC, -2, 1);
}
