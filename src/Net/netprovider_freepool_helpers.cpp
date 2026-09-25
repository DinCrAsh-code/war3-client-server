//============================================================================
//  Net::NetProvider's free-pool node helpers, sub_6F65AC20
//  (NetProviderSlot9Self::ReleasePayload) and sub_6F65D000
//  (NetProviderSlot9Self::UnlinkAndFreeNode) - shared by vtable slot 9
//  (netprovider_slot9.cpp, ShrinkFreePool) and vtable slot 31
//  (netprovider_slot31.cpp, FindAndRemove).
//
//  Deliberately its own translation unit: the shipped code reaches both
//  through a real `call`, never inlined, and with the definitions visible
//  in the same TU as either caller this toolchain's /Ob2 inlines them on
//  the spot - measured directly (ShrinkFreePool's own generated listing
//  showed PostRecycled's body folded straight into it once ReleasePayload
//  lived in the same file).  Same rule as agent_deleteself.cpp /
//  observer_deleteself.cpp (docs/msvc-vc8-idioms.md, "A deleting
//  destructor's `this != 0` test tracks whether the base destructor
//  inlined").
//============================================================================
#include "storm.h"

//  0x6F658B70 - a CDataStore-backed status post (constructs a CDataStore
//  local, forwards through sub_6F657010/sub_6F6514B0/sub_6F6576F0 and the
//  two "recycled event" callback slots at ds:off_6F932C14/ds:off_6F932C0C),
//  under a real __except_handler4-shaped SEH frame - the same
//  unreproducible-under-/GS- shape netprovider_deleteself.cpp's own base
//  destructor thunk documents.  Not this session's target; thunked to the
//  real, unhooked body.
class NetProviderSlot9Self
{
public:
    void PostRecycled(int code, int* payload);
    //  0x6F65AC20 - forwards straight into PostRecycled above with code
    //  0xD, keeping whatever `this` its own caller already left in ecx
    //  (its body never reloads ecx itself).
    void ReleasePayload(int value);
    //  0x6F65D000 - see below.
    void UnlinkAndFreeNode(void* node);
};
__declspec(naked) void NetProviderSlot9Self::PostRecycled(int, int*)
{
    __asm { mov eax, 0x6F658B70 }
    __asm { jmp eax }
}

void NetProviderSlot9Self::ReleasePayload(int value)
{
    int local = value;
    PostRecycled(0xD, &local);
}

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  A poisoned-link-offset tag: this list's element type is unrecoverable
//  from this call tree (TSLink<T>::Unlink() never touches T, only pointer
//  arithmetic - the same reasoning netprovider_pendinglist.cpp's own
//  DirectLink already gives), so it is tagged by this instantiation's own
//  target address, the same convention netprovider_waitqueuering.cpp uses.
struct UnknownFreeNode_6F65D000;
typedef TSLink<UnknownFreeNode_6F65D000> FreeNodeLink;

//  0x6F65D000 - free one pool node: release its type descriptor if it has
//  one, unlink it from whatever list it is on (a node's own TSLink lives at
//  the node's own address, offset 0 - the same DirectLink shape
//  netprovider_pendinglist.cpp already establishes), free the node itself,
//  and decrement `this`'s own +0x650 live-count.
void NetProviderSlot9Self::UnlinkAndFreeNode(void* node)
{
    char* self = (char*)this;
    if (node)
    {
        int typeDesc = *(int*)((char*)node + 0x2C);
        if (typeDesc)
            SMemFree((void*)typeDesc, "AuFixedStringData", -2, 0);

        ((FreeNodeLink*)node)->Unlink();

        SMemFree(node, "delete", -1, 0);
    }
    *(int*)(self + 0x650) -= 1;
}
