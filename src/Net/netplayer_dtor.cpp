//============================================================================
//  Net::NetPlayer - vtable slot 0, 0x6F66F630.  Same "scalar deleting
//  destructor" shape as Net::NetProvider's own (netprovider_deleteself.cpp).
//============================================================================

//  0x6F66F540 - the base ~NetPlayer() body: stamps this object's own vtable
//  pointer, tears down an embedded CDataStore-shaped sub-object at +0x24
//  (releasing it through the shared `off_6F932C0C` teardown vector every
//  CDataStore-derived class in this codebase already uses), unlinks two
//  intrusive TInstanceId-style list nodes (+0x14 and the trailing
//  TInstanceId<NetPlayer> base at +0x4). Carries the same real SEH frame
//  this build's fixed /GS- /EHs-c- cannot reproduce - a thunk to the real,
//  unhooked body rather than a reconstruction. `retn` with no operand
//  (thiscall, no stack args) confirmed off its own listing.
class NetPlayerBaseDtor
{
public:
    void Destroy();
};
__declspec(naked) void NetPlayerBaseDtor::Destroy()
{
    __asm { mov eax, 0x6F66F540 }
    __asm { jmp eax }
}

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net {

class NetPlayer
{
public:
    //  0x6F66F630 - vtable slot 0.
    void* DeleteSelf(unsigned int flags);
};

void* NetPlayer::DeleteSelf(unsigned int flags)
{
    ((NetPlayerBaseDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}  // namespace Net
