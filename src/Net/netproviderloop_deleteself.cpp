//============================================================================
//  Net::NetProviderLOOP - vtable slot 0, 0x6F661700.  The classic MSVC
//  "scalar deleting destructor" - see netprovider_deleteself.cpp's own
//  header for the full shape and why the base destructor is a thunk rather
//  than a reconstruction here too.
//============================================================================
#include "netproviderloop.h"

//  0x6F661500 - ~NetProviderLOOP().  Carries the same __except_handler4-
//  shaped SEH frame (`push -1` / `push offset SEH_6F661500` / `large
//  fs:0` prologue, matching epilogue) docs/msvc-vc8-idioms.md documents as
//  unreproducible at this build's fixed /GS- /EHs-c- - drops the object's
//  own +0x2B4 interface (stamping and deleting it as a Net::NetClient),
//  unlinks the +0x69C port list and the +0x684 CRITICAL_SECTION, and tail-
//  calls the base Net::NetProvider destructor (0x6F65D9E0, itself already a
//  thunk in netprovider_deleteself.cpp for the identical reason).  Left as
//  a plain thiscall member in its own translation unit, never defined here,
//  for the same reason netprovider_deleteself.cpp gives: with a real,
//  un-inlined call the shipped `test esi,esi`/`jz` this-null guard below
//  has to survive, and the dump shows it does.
class NetProviderLoopDtor
{
public:
    void Destroy();
};
__declspec(naked) void NetProviderLoopDtor::Destroy()
{
    __asm { mov eax, 0x6F661500 }
    __asm { jmp eax }
}

//  Storm.dll ordinal 403 - matches the declaration already used across this
//  directory (netprovider_deleteself.cpp, actionqueuerecord_notify.cpp, ...).
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net
{

void* NetProviderLOOP::DeleteSelf(unsigned int flags)
{
    ((NetProviderLoopDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}  // namespace Net
