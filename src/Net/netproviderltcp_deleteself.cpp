//============================================================================
//  Net::NetProviderLTCP - vtable slot 0, 0x6F663680.  The classic MSVC
//  "scalar deleting destructor" - see netprovider_deleteself.cpp's own
//  header for the full shape and netproviderloop_deleteself.cpp's own note
//  on why the base destructor is a thunk rather than a reconstruction here
//  too.
//============================================================================
#include "netproviderltcp.h"

//  0x6F663200 - ~NetProviderLTCP().  Carries the same __except_handler4-
//  shaped SEH frame (`push -1` / `push offset SEH_6F663200` / `large fs:0`
//  prologue) docs/msvc-vc8-idioms.md documents as unreproducible at this
//  build's fixed /GS- /EHs-c- - left as a plain thiscall member in its own
//  translation unit, never defined here, for the same reason
//  netproviderloop_deleteself.cpp gives: with a real, un-inlined call the
//  shipped `test esi,esi`/`jz` this-null guard below has to survive, and
//  the dump shows it does.
class NetProviderLtcpDtor
{
public:
    void Destroy();
};
__declspec(naked) void NetProviderLtcpDtor::Destroy()
{
    __asm { mov eax, 0x6F663200 }
    __asm { jmp eax }
}

//  Storm.dll ordinal 403 - matches the declaration already used across this
//  directory (netprovider_deleteself.cpp, netproviderloop_deleteself.cpp, ...).
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net
{

void* NetProviderLTCP::DeleteSelf(unsigned int flags)
{
    ((NetProviderLtcpDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}  // namespace Net
