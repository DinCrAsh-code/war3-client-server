//============================================================================
//  Two lock-free-stack primitives (agent-networking-wave4-2026-08-29) that
//  CDataRecycler's own vtable slot 1 (Purge, below) and the NetClient
//  receive-append path's chunk pool (netclient_chunkpool.cpp) both drive:
//  a Treiber-stack pop and push over a `void*` head field, with the "next"
//  link living at a caller-supplied byte offset inside each node rather
//  than a fixed offset - the same node type is threaded onto two different
//  stacks at two different link offsets (CDataRecycler::m_field10/m_field14
//  each have their own).
//
//  Neither takes `this` in ecx - every argument arrives on the stack, which
//  is what makes this a free function rather than a class member despite
//  living at 0x6F4C67xx, inside CDataRecycler's own 0x6F4C65xx-0x6F4C69xx
//  address run. Written naked: the retry loop's own register choices (which
//  callee-saved register holds the offset vs. the node vs. the head
//  pointer) are exactly the kind of detail a hand-typed loop would have no
//  reason to reproduce, and a naked body matches by construction (CLAUDE.md).
//
//  Argument order below is read off each function's own real call sites
//  (cdatarecycler_stackprims.cpp's own callers push right-to-left in C++
//  declaration order), not off IDA's own arg_0/Exchange/Destination labels
//  in the dump - those are IDA's generic InterlockedCompareExchange-shaped
//  naming heuristic pattern-matching onto a structurally similar function,
//  not a resolved semantic name, and the two disagree with the literal
//  push order at every real call site.  The raw stack offsets copied into
//  the __asm blocks below are unaffected either way - they are the literal
//  immediates from the dump, not resolved through either naming scheme.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F4C6710 - pop the head off the Treiber stack whose head pointer is
//  `*headPtr`, with each node's "next" link read from `*(void**)(node +
//  linkOffset)`.  CAS retry loop; returns 0 if the stack was already empty.
//  `retn 8` (two stack args, no `this`).
//----------------------------------------------------------------------------
void* __fastcall CompareExchangePointer(void* volatile*, void*, void*);

__declspec(naked) void* __stdcall PopFromStack(void** /*headPtr*/, int /*linkOffset*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+8]
        mov     esi, [edi]
        test    esi, esi
        jz      done
        push    ebx
        mov     ebx, [esp+0Ch]
    retryPop:
        mov     edx, [esi+ebx]
        push    esi
        mov     ecx, edi
        call    CompareExchangePointer
        cmp     eax, esi
        jz      popped
        mov     esi, [edi]
        test    esi, esi
        jnz     retryPop
    popped:
        pop     ebx
    done:
        pop     edi
        mov     eax, esi
        pop     esi
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F4C66E0 - push `node` onto the Treiber stack whose head pointer is
//  `*headPtr`, threading it through the "next" link at byte offset
//  `linkOffset` inside the node.  CAS retry loop; no failure path (a push
//  always succeeds once the CAS lands). `retn 0Ch` (three stack args, no
//  `this`).
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall PushOntoStack(void** /*headPtr*/, void* /*node*/, int /*linkOffset*/)
{
    __asm
    {
        push    ebx
        mov     ebx, [esp+4]
        push    ebp
        mov     ebp, [esp+8]
        push    esi
        push    edi
        mov     edi, [esp+10h]
    retryPush:
        mov     esi, [edi]
        push    esi
        mov     edx, ebx
        mov     ecx, edi
        mov     [ebx+ebp], esi
        call    CompareExchangePointer
        cmp     eax, esi
        jnz     retryPush
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    0Ch
    }
}
