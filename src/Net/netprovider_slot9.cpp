//============================================================================
//  Net::NetProvider - vtable slot 9, 0x6F65D510: shrink a free-node pool
//  down to a new limit, releasing nodes off its head one at a time under
//  the object's own +0x62C CRITICAL_SECTION.  Its two callees
//  (ReleasePayload/UnlinkAndFreeNode, sub_6F65AC20/sub_6F65D000) live in
//  netprovider_freepool_helpers.cpp - only declared here, deliberately, so
//  this build's own /Ob2 does not inline them the way a same-TU definition
//  measurably does (see that file's own header comment).
//============================================================================

class NetProviderSlot9Self
{
public:
    void ReleasePayload(int value);
    void UnlinkAndFreeNode(void* node);
};

void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);

namespace Net
{

class NetProvider
{
public:
    //  0x6F65D510 - vtable slot 9.
    void ShrinkFreePool(unsigned int newLimit);
};

void NetProvider::ShrinkFreePool(unsigned int newLimit)
{
    char* self = (char*)this;
    EnterCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x62C));

    unsigned int liveCount = *(unsigned int*)(self + 0x650);
    *(unsigned int*)(self + 0x654) = newLimit;
    if (liveCount > newLimit)
    {
        do
        {
            int raw = *(int*)(self + 0x64C);
            int node = (raw > 0) ? raw : 0;
            void* payload = *(void**)((char*)node + 8);

            ((NetProviderSlot9Self*)this)->ReleasePayload((int)payload);
            ((NetProviderSlot9Self*)this)->UnlinkAndFreeNode((void*)node);
        } while (*(unsigned int*)(self + 0x650) > *(unsigned int*)(self + 0x654));
    }

    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x62C));
}

}  // namespace Net
