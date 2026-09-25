//============================================================================
//  Net::NetProvider - vtable slot 31, 0x6F65D650, and its direct callee
//  sub_6F65D260 (0x6F65D260): find a free-pool node by its own +8 key
//  under the object's +0x62C lock and, if one matches, release and unlink
//  it exactly the way netprovider_slot9.cpp's ShrinkFreePool does (the
//  same ReleasePayload/UnlinkAndFreeNode pair, reached here by
//  re-declaring their already-defined mangled names rather than copying
//  their bodies - the mangled symbol carries no layout information, the
//  same technique netprovider_slot2.cpp uses for
//  NetProviderMethod0x90Self::Cleanup).
//============================================================================

//  Real bodies in netprovider_slot9.cpp.
class NetProviderSlot9Self
{
public:
    void ReleasePayload(int value);
    void UnlinkAndFreeNode(void* node);
};

void __fastcall EnterCritSec(struct _RTL_CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(struct _RTL_CRITICAL_SECTION* cs);

class NetProviderSlot31Self
{
public:
    //  0x6F65D260 - returns 1 if a node was found (and removed), 0
    //  otherwise.
    int FindAndRemove(int key);
};

int NetProviderSlot31Self::FindAndRemove(int key)
{
    char* self = (char*)this;
    EnterCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x62C));

    int found = 0;
    int raw = *(int*)(self + 0x64C);
    int node = (raw > 0) ? raw : 0;
    if (node > 0)
    {
        for (;;)
        {
            if (*(int*)(node + 8) == key)
            {
                int payload = *(int*)(node + 8);
                found = 1;
                ((NetProviderSlot9Self*)this)->ReleasePayload(payload);
                ((NetProviderSlot9Self*)this)->UnlinkAndFreeNode((void*)node);
                break;
            }
            node = *(int*)(node + 4);
            if (node <= 0)
                break;
        }
    }

    LeaveCritSec((struct _RTL_CRITICAL_SECTION*)(self + 0x62C));
    return found;
}

namespace Net
{

class NetProvider
{
public:
    //  0x6F65D650 - vtable slot 31.  FindAndRemove's own 0/1 result,
    //  remapped to 7 (not found) / 1 (found).
    int RemoveByKey(int key);
};

int NetProvider::RemoveByKey(int key)
{
    int found = ((NetProviderSlot31Self*)this)->FindAndRemove(key);
    return found ? 1 : 7;
}

}  // namespace Net
