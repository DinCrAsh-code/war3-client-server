//============================================================================
//  Net::NetProvider - vtable slot 6, 0x6F65CE60, and its own direct callee
//  sub_6F65C7D0 (0x6F65C7D0, reached from slot 6's own body).  Both walk
//  the same two lock-protected structures this class embeds at +0x5F0 (a
//  24-byte CRITICAL_SECTION) and +0x60C/+0x610/+0x628 (a small node list
//  with a running "in use" counter at +0x628 and a byte-flag high-bit
//  gate at each node's own +0x10).  Nothing in this call tree names the
//  node type, so it is read through raw offsets exactly as the disassembly
//  does, the same way netprovider_waitqueuering.cpp's own WaitQueueRing
//  reads its untyped item slots.
//
//  sub_6F65C900 (0x6F65C900, 12 stack args, `retn 30h`) posts a status
//  update through several more layers and is genuinely deep, out of scope
//  for this batch - a thunk to the real, unhooked body rather than a
//  reconstruction.
//
//  sub_6F65C840 (FinishReset below) is reconstructed for real
//  (batch-G-netevent-and-tail): a second-pass sweep over the same node
//  list under the same +0x5F0 lock, testing each live node's own +0x10
//  high-bit flags (0x20000000/0x40000000) and posting through two further,
//  un-investigated status-post callees (sub_6F65B750/sub_6F65AB90, each
//  its own deeper subsystem, left as naked thunks - ABI confirmed off each
//  one's own `retn`) before clearing the tested bits; nodes whose own
//  +0x10 top bit (0x80000000, tested via the `js` on a signed read) is
//  clear instead get unlinked and freed through sub_6F65ABC0/sub_6F65C570
//  (same further-subsystem treatment).
//============================================================================

void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
extern "C" void* memcpy(void*, const void*, unsigned int);

#define NAKED_THUNK(sig, addr)             \
    __declspec(naked) sig                  \
    {                                       \
        __asm { mov eax, addr }             \
        __asm { jmp eax }                   \
    }

//  0x6F65B750 - retn 0x14 (5 stack args): the "posted" status-post callee
//  FinishReset's own 0x20000000-flagged nodes reach. Own further,
//  un-investigated subsystem.
NAKED_THUNK(void __fastcall PostNodeStatusA_6F65B750(void* self, void* /*unused_edx*/,
                                                       void* a0, void* a1, void* a2,
                                                       void* a3, void* a4),
            0x6F65B750)
//  0x6F65AB90 - retn 0xC (3 stack args): the "posted" status-post callee
//  FinishReset's own 0x40000000-flagged nodes reach. Own further,
//  un-investigated subsystem.
NAKED_THUNK(void __fastcall PostNodeStatusB_6F65AB90(void* self, void* /*unused_edx*/,
                                                       void* a0, void* a1, void* a2),
            0x6F65AB90)
//  0x6F65ABC0 - retn 4 (1 stack arg): the neither-flag-set callee, called
//  ahead of unlinking the node. Own further, un-investigated subsystem.
NAKED_THUNK(void __fastcall PostNodeStatusC_6F65ABC0(void* self, void* /*unused_edx*/,
                                                       void* a0),
            0x6F65ABC0)
//  0x6F65C570 - retn 4 (1 stack arg), `this` = &self->m_nodeListHead
//  (self+0x608): unlink/free `node` and return the list's new head. Own
//  further, un-investigated subsystem.
NAKED_THUNK(void* __fastcall UnlinkAndFreeNode_6F65C570(void* nodeListHead, void* /*unused_edx*/,
                                                          void* node),
            0x6F65C570)

#undef NAKED_THUNK

//  0x6F65C900 - node-notify post, 12 stack arguments plus `this` in ecx.
class NetProviderSlot6Self
{
public:
    void NotifyNode(void* a, void* b, int c, int d, int e, int f, void* g,
                     int h, int i, int j, void* node, int flag);
    //  0x6F65C840 - the second-pass sweep over the same node list.
    void FinishReset();
    //  0x6F65C7D0 - clear each node's own "posted" high bit under lock.
    void ClearPostedBits();
};

__declspec(naked) void NetProviderSlot6Self::NotifyNode(void*, void*, int, int,
                                                         int, int, void*, int,
                                                         int, int, void*, int)
{
    __asm { mov eax, 0x6F65C900 }
    __asm { jmp eax }
}

void NetProviderSlot6Self::FinishReset()
{
    char* self = (char*)this;
    EnterCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x5F0));

    if (--*(int*)(self + 0x628) == 0)
    {
        int count = *(int*)(self + 0x610);
        //  Same "clamp negative to zero, then treat as unsigned count"
        //  shape as the caller's own list-head derivation above.
        int node = (count > 0) ? count : 0;

        if (node > 0)
        {
            //  `and [esi+10h], ebx` with ebx=0x1FFFFFFF - keep the low 29
            //  bits, clear the two "posted" flags this loop tests.
            const unsigned int kClearPostedMask = 0x1FFFFFFF;
            char* n = (char*)node;
            do
            {
                int flags = *(int*)(n + 0x10);
                if (flags & 0x20000000)
                {
                    PostNodeStatusA_6F65B750((void*)self, 0,
                                              (void*)*(int*)(n + 8),
                                              (void*)*(int*)(n + 0x40),
                                              (void*)*(int*)(n + 0x44),
                                              (void*)*(int*)(n + 0x48),
                                              (void*)(n + 0x4C));
                    *(unsigned int*)(n + 0x10) &= kClearPostedMask;
                    n = *(char**)(n + 4);
                }
                else if (flags & 0x40000000)
                {
                    PostNodeStatusB_6F65AB90((void*)self, 0,
                                              (void*)*(int*)(n + 8),
                                              (void*)*(int*)(n + 0x40),
                                              (void*)*(int*)(n + 0x44));
                    *(unsigned int*)(n + 0x10) &= kClearPostedMask;
                    n = *(char**)(n + 4);
                }
                else if (flags >= 0)
                {
                    n = *(char**)(n + 4);
                }
                else
                {
                    PostNodeStatusC_6F65ABC0((void*)self, 0, (void*)*(int*)(n + 8));
                    n = (char*)UnlinkAndFreeNode_6F65C570(self + 0x608, 0, n);
                }
            } while ((int)n > 0);
        }
    }

    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x5F0));
}

//  0x6F65C7D0 - real body in netprovider_slot6_helper.cpp, deliberately in
//  its own TU so this build's /Ob2 does not inline it into ResetAndNotify
//  below (measured: it does, the instant the definition is visible here).

namespace Net
{

class NetProvider
{
public:
    //  0x6F65CE60 - vtable slot 6.  Two stack args: a source pointer to a
    //  12-dword block copied wholesale into a fresh SMemAlloc'd one (or
    //  freeing the previous copy and storing null if the source is null),
    //  a second value stashed straight into +0x618, and then a node-list
    //  walk under the same +0x60C list ClearPostedBits above reads,
    //  posting each live node through vtable+0x90's own NotifyNode.
    void ResetAndNotify(void* replacement, void* sourceBlock);
};

void NetProvider::ResetAndNotify(void* sourceBlock, void* replacement)
{
    char* self = (char*)this;
    EnterCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x5F0));

    void* oldBlock = *(void**)(self + 0x61C);
    if (oldBlock)
    {
        SMemFree(oldBlock, "delete", -1, 0);
        *(void**)(self + 0x61C) = 0;
    }

    void* newBlock = SMemAlloc(0x30, ".\\NetProvider.cpp", 0x218, 0);
    if (newBlock)
        memcpy(newBlock, sourceBlock, 0x30);
    *(void**)(self + 0x61C) = newBlock;

    *(void**)(self + 0x618) = replacement;
    ((NetProviderSlot6Self*)this)->ClearPostedBits();

    int raw = *(int*)(*(int*)(self + 0x60C)) ;
    raw = *(int*)(raw + 4);
    int node = (raw > 0) ? raw : 0;
    if (node)
    {
        do
        {
            char* n = (char*)node;
            ((NetProviderSlot6Self*)this)->NotifyNode(
                (void*)(n + 0x28), (void*)(n + 0x18),
                *(int*)(n + 0x38), *(int*)(n + 0x3C),
                *(int*)(self + 0x24), *(int*)(self + 0x28),
                (void*)(n + 0x4C),
                *(int*)(n + 0x40), *(int*)(n + 0x44), *(int*)(n + 0x48),
                (void*)n, 1);

            int list = *(int*)(*(int*)n);
            node = *(int*)(list + 4);
        } while (node > 0);
    }

    ((NetProviderSlot6Self*)this)->FinishReset();
    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x5F0));
}

}  // namespace Net
