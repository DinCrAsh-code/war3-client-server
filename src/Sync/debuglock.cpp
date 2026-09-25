//============================================================================
//  A small "CDebugLock" diagnostic-logging family (the debug tag string
//  "%s(%u) : CDebugLock:%08x: ..." both halves below share names) built on
//  top of two duplicated fixed-size ring buffers (dword_6FAD7DC8/unk_6FAD7DB8
//  and their 0x6FAD91xx twins) and two duplicated singly-linked lists
//  (dword_6FADA600/dword_6FADA604) - reached from FlushDeadlockLog_6F6D9500/
//  FlushDeadlockLog_6F6D9510 (deadlocklog_flush.cpp), each driving one half
//  of the pair. Reproduced with __declspec(naked) bodies: several of these
//  read `ecx`/`edi`/`esi` conventions the compiler would not choose from a
//  plain C++ signature (LazyInitAndEnterDebugLock is read from a *global*
//  address, not a real per-instance `this`, at both its call sites), the
//  same class of hand-written convention as threadwaitpool.cpp's own family.
//============================================================================
#include <windows.h>

extern "C" void nullsub_1();
extern "C" void* __cdecl memset(void* dst, int value, unsigned int count);
#pragma function(memset)

extern "C" long dword_6FAD7DC8[];
extern "C" long dword_6FAD7DC0[];
extern "C" char unk_6FAD7DB8[];
extern "C" long dword_6FADA5B8;
extern "C" long dword_6FADA5BC;

extern "C" long dword_6FAD91C8[];
extern "C" long dword_6FAD91C0[];
extern "C" char unk_6FAD91B8[];
extern "C" long dword_6FADA5DC;
extern "C" long dword_6FADA5E0;

extern "C" void* dword_6FADA600;
extern "C" void* dword_6FADA604;

static const char aSUCdebuglock08[] = "%s(%u) : CDebugLock:%08x: tid:%03x %c %s\n";
static const char aSUCdebuglock08_0[] = "%s(%u) : CDebugLock:%08x: entry has bad state %08x\n";
static const char aW32OsLockCpp[] = ".\\W32\\OsLock.cpp";

void DebugLock_LogBadEntry0();
void DebugLock_LogBadEntry1();
void DebugLock_LogEntries0();
void DebugLock_LogEntries1();

//----------------------------------------------------------------------------
//  0x6F6D8650 - lazily InitializeCriticalSection this "self" record (a
//  fixed global, not a real object - both call sites pass the address of a
//  static struct directly) under a spin-locked flag at self+0, then always
//  EnterCriticalSection it before returning whether this call was the one
//  that performed the lazy init.
//----------------------------------------------------------------------------
//  Declared __fastcall with the record pointer as its one argument, because
//  that is what the two call sites in this file and the two in OsTcp.cpp
//  (OsNetInitialize/OsNetShutdown, which pass &g_osNetLock) actually do: the
//  body's first act is `mov esi, ecx`.  It was a no-argument __cdecl
//  declaration while its only callers were the two __asm ones below, which
//  set ecx themselves; a C++ caller cannot.  The body is unchanged and so is
//  its score.
__declspec(naked) void __fastcall LazyInitAndEnterDebugLock(void*)
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, ecx
        xor     ebx, ebx
        cmp     dword ptr [esi+4], ebx
        jnz     short alreadyInit
        push    edi
        mov     edi, InterlockedExchange
    spin:
        push    1
        push    esi
        call    edi
        test    eax, eax
        jnz     short spin
        cmp     dword ptr [esi+4], ebx
        pop     edi
        jnz     short skipInit
        lea     eax, [esi+8]
        test    eax, eax
        mov     [esi+4], eax
        jz      short skipInitCS
        push    eax
        //  `call DWORD PTR [X]`, not `call X`.  In inline asm the bare form
        //  assembles to `E8 rel32` **at the import table slot itself**
        //  (REL32 relocation), so it executes the four bytes of the
        //  function pointer as code.  The bracketed form is `FF 15`
        //  (DIR32) - the indirect call through the slot that the dump's
        //  own `call ds:__imp_...` does.  Both canonicalise to `call SYM`,
        //  so the instruction score cannot tell them apart.  See
        //  docs/notes/import-call-indirection.md.
        call    DWORD PTR [InitializeCriticalSection]
    skipInitCS:
        mov     ebx, 1
    skipInit:
        mov     dword ptr [esi], 0
    alreadyInit:
        mov     eax, [esi+4]
        push    eax
        call    DWORD PTR [EnterCriticalSection]
        pop     esi
        mov     eax, ebx
        pop     ebx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8A70 - one-time init of the first ring buffer: lazily lock the
//  first global record, zero the 0x1400-byte log area, chain each of its
//  0x14-byte slots' own leading dword to `slotIndex+1` (a free-list-style
//  self-index, one-based), and mark the ring "ready".
//----------------------------------------------------------------------------
__declspec(naked) void DeadlockLog0_Init()
{
    __asm
    {
        mov     ecx, offset dword_6FADA5BC
        call    LazyInitAndEnterDebugLock
        test    eax, eax
        jz      short done
        push    1400h
        push    0
        push    offset unk_6FAD7DB8
        call    memset
        add     esp, 0Ch
        mov     ecx, 1
        mov     eax, 14h
    fill:
        add     ecx, 1
        mov     dword_6FAD7DC0[eax], ecx
        add     eax, 14h
        cmp     eax, 13ECh
        jb      short fill
        mov     dword_6FADA5B8, 1
    done:
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8AD0 - the second ring buffer's own twin of DeadlockLog0_Init.
//----------------------------------------------------------------------------
__declspec(naked) void DeadlockLog1_Init()
{
    __asm
    {
        mov     ecx, offset dword_6FADA5E0
        call    LazyInitAndEnterDebugLock
        test    eax, eax
        jz      short done
        push    1400h
        push    0
        push    offset unk_6FAD91B8
        call    memset
        add     esp, 0Ch
        mov     ecx, 1
        mov     eax, 14h
    fill:
        add     ecx, 1
        mov     dword_6FAD91C0[eax], ecx
        add     eax, 14h
        cmp     eax, 13ECh
        jb      short fill
        mov     dword_6FADA5DC, 1
    done:
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D94C0 - walk the first singly-linked "active lock" list
//  (dword_6FADA600, each node's own +8 the link) logging every node through
//  DebugLock_LogEntries0.
//----------------------------------------------------------------------------
__declspec(naked) void DeadlockLog0_WalkActive()
{
    __asm
    {
        push    esi
        mov     esi, dword_6FADA600
        test    esi, esi
        jz      short done
    walk:
        mov     ecx, esi
        call    DebugLock_LogEntries0
        mov     esi, [esi+8]
        test    esi, esi
        jnz     short walk
    done:
        pop     esi
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D94E0 - the second list's own twin (dword_6FADA604 /
//  DebugLock_LogEntries1).
//----------------------------------------------------------------------------
__declspec(naked) void DeadlockLog1_WalkActive()
{
    __asm
    {
        push    esi
        mov     esi, dword_6FADA604
        test    esi, esi
        jz      short done
    walk:
        mov     ecx, esi
        call    DebugLock_LogEntries1
        mov     esi, [esi+8]
        test    esi, esi
        jnz     short walk
    done:
        pop     esi
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8CC0 - one node's own log entries: reads a slot count at node+0,
//  and for each of up to 0x100 slots in the ring buffer (unk_6FAD7DB8, each
//  slot 0x14 bytes wide, indexed via slot*5), formats one diagnostic line
//  through nullsub_1 describing the slot's own recorded tag byte, elapsed
//  time since GetTickCount, and a free/held flag pair derived from the same
//  slot dword's own top two bits, then advances to the slot's own +8 "next"
//  link. A count of 0x100 or more instead calls DebugLock_LogBadEntry0
//  once, treating the node itself as corrupt.
//----------------------------------------------------------------------------
__declspec(naked) void DebugLock_LogEntries0()
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, ecx
        xor     esi, esi
        call    DWORD PTR [GetTickCount]
        mov     edx, [edi]
        test    edx, edx
        mov     ebx, eax
        jz      short done
        cmp     edx, 100h
        jnb     short badEntry
    loop_:
        lea     esi, [edx+edx*4]
        mov     eax, dword_6FAD7DC8[esi*4]
        lea     esi, unk_6FAD7DB8[esi*4]
        mov     edx, eax
        and     edx, 80000000h
        mov     ecx, ebx
        sub     ecx, [esi]
        neg     edx
        push    ecx
        sbb     edx, edx
        mov     ecx, eax
        and     edx, 0Eh
        and     ecx, 40000000h
        add     edx, 46h
        neg     ecx
        push    edx
        mov     edx, [esi+4]
        sbb     ecx, ecx
        and     ecx, 5
        add     ecx, 52h
        push    ecx
        push    edx
        push    edi
        and     eax, 3FFFFFFFh
        push    eax
        mov     eax, [esi+0Ch]
        push    eax
        push    offset aSUCdebuglock08
        call    nullsub_1
        mov     edx, [esi+8]
        add     esp, 20h
        test    edx, edx
        jnz     short loop_
    done:
        pop     edi
        pop     esi
        pop     ebx
        retn
    badEntry:
        push    31Bh
        push    offset aW32OsLockCpp
        push    esi
        mov     ecx, edi
        call    DebugLock_LogBadEntry0
        jmp     short done
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8F60 - the second ring buffer's own twin of DebugLock_LogEntries0.
//----------------------------------------------------------------------------
__declspec(naked) void DebugLock_LogEntries1()
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, ecx
        xor     esi, esi
        call    DWORD PTR [GetTickCount]
        mov     edx, [edi]
        test    edx, edx
        mov     ebx, eax
        jz      short done
        cmp     edx, 100h
        jnb     short badEntry
    loop_:
        lea     esi, [edx+edx*4]
        mov     eax, dword_6FAD91C8[esi*4]
        lea     esi, unk_6FAD91B8[esi*4]
        mov     edx, eax
        and     edx, 80000000h
        mov     ecx, ebx
        sub     ecx, [esi]
        neg     edx
        push    ecx
        sbb     edx, edx
        mov     ecx, eax
        and     edx, 0Eh
        and     ecx, 40000000h
        add     edx, 46h
        neg     ecx
        push    edx
        mov     edx, [esi+4]
        sbb     ecx, ecx
        and     ecx, 5
        add     ecx, 52h
        push    ecx
        push    edx
        push    edi
        and     eax, 3FFFFFFFh
        push    eax
        mov     eax, [esi+0Ch]
        push    eax
        push    offset aSUCdebuglock08
        call    nullsub_1
        mov     edx, [esi+8]
        add     esp, 20h
        test    edx, edx
        jnz     short loop_
    done:
        pop     edi
        pop     esi
        pop     ebx
        retn
    badEntry:
        push    31Bh
        push    offset aW32OsLockCpp
        push    esi
        mov     ecx, edi
        call    DebugLock_LogBadEntry1
        jmp     short done
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8B30 - logs a "bad state" line via nullsub_1 for a corrupt entry;
//  if the reported state (the third stack argument) is 0, clears the node's
//  own +0 slot count instead, otherwise clears its +8 "next" link.
//----------------------------------------------------------------------------
void __declspec(naked) DebugLock_LogBadEntry0()
{
    __asm
    {
        mov     eax, [esp+0Ch]
        push    esi
        push    edx
        mov     esi, ecx
        mov     ecx, [esp+8+8]
        push    esi
        push    eax
        push    ecx
        push    offset aSUCdebuglock08_0
        call    nullsub_1
        mov     eax, [esp+18h+4]
        add     esp, 14h
        test    eax, eax
        jnz     short badLink
        mov     [esi], eax
        pop     esi
        retn    0Ch
    badLink:
        mov     dword ptr [eax+8], 0
        pop     esi
        retn    0Ch
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8B70 - the second ring buffer's own twin of DebugLock_LogBadEntry0.
//----------------------------------------------------------------------------
void __declspec(naked) DebugLock_LogBadEntry1()
{
    __asm
    {
        mov     eax, [esp+0Ch]
        push    esi
        push    edx
        mov     esi, ecx
        mov     ecx, [esp+8+8]
        push    esi
        push    eax
        push    ecx
        push    offset aSUCdebuglock08_0
        call    nullsub_1
        mov     eax, [esp+18h+4]
        add     esp, 14h
        test    eax, eax
        jnz     short badLink
        mov     [esi], eax
        pop     esi
        retn    0Ch
    badLink:
        mov     dword ptr [eax+8], 0
        pop     esi
        retn    0Ch
    }
}
