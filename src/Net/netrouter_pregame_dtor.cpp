//============================================================================
//  Net::NetRouter::PreGame - vtable slot 0, 0x6F670430.  The classic MSVC
//  "scalar deleting destructor": run the base destructor, then free `this`
//  if bit 0 of the flags argument is set, and hand `this` back - same shape
//  netprovider_deleteself.cpp already documents for Net::NetProvider.
//============================================================================

//  0x6F66FAE0 - the base ~PreGame() body: stamps this object's own vtable
//  pointer and TInstanceId<PreGame>'s, releases the CRITICAL_SECTION at
//  +0x10 (sub_6F6C4070, an interlocked-decrement release already used
//  elsewhere in this directory) and unlinks the TInstanceId<PreGame> list
//  node at +0x4. Carries a real __except_handler4-shaped SEH frame this
//  build's fixed /GS- /EHs-c- cannot reproduce (see
//  netprovider_deleteself.cpp's own note on the same shape) - a thunk to
//  the real, unhooked body rather than a reconstruction. `retn` with no
//  operand (thiscall, no stack args) confirmed off its own listing.
class PreGameBaseDtor
{
public:
    void Destroy();
};
__declspec(naked) void PreGameBaseDtor::Destroy()
{
    __asm { mov eax, 0x6F66FAE0 }
    __asm { jmp eax }
}

//  Storm.dll ordinal 403 - matches the declaration already used across this
//  directory (netprovider_deleteself.cpp, actionqueuerecord_notify.cpp, ...).
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net { namespace NetRouter {

class PreGame
{
public:
    //  0x6F670430 - vtable slot 0.
    void* DeleteSelf(unsigned int flags);
};

void* PreGame::DeleteSelf(unsigned int flags)
{
    ((PreGameBaseDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}}  // namespace Net::NetRouter
