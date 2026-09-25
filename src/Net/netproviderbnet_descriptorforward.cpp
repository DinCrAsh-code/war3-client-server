//============================================================================
//  NetProviderBnetDescriptorForward::Forward, 0x6F65B870.  Copies `code`
//  and 0xB8 bytes from `dataPtr` onto its own stack and forwards the blob
//  through NetProviderBnetForwardTarget::Notify with a fixed inner code of
//  0x11.  Own TU, separate from Method_19 (netproviderbnet_session.cpp),
//  so the real call into it survives rather than being inlined away.
//============================================================================
#include <windows.h>

//  0x6F658F30 - deep CDataStore-building forward (Storm event post through
//  ds:off_6F932C14/0x6F932C0C, sub_6F657010/sub_6F6525A0/sub_6F6576F0) this
//  session did not trace; carries a real __except_handler4-shaped SEH frame
//  this build's fixed /GS- /EHs-c- cannot reproduce regardless (see
//  docs/msvc-vc8-idioms.md's own section on that frame shape) - a thunk to
//  the real, unhooked body.  `this` in ecx, two stack args, `retn 8`: the
//  first (arg_0) is a plain int (the fixed code Forward always passes as
//  0x11), the second (arg_4) the address of the 0xB8-byte blob Forward
//  copies onto its own stack.
class NetProviderBnetForwardTarget
{
public:
    void Notify(int code, void* blobPtr);
};
__declspec(naked) void NetProviderBnetForwardTarget::Notify(int, void*)
{
    __asm { mov eax, 0x6F658F30 }
    __asm { jmp eax }
}

class NetProviderBnetDescriptorForward
{
public:
    void Forward(int code, void* dataPtr);
};

struct NetProviderBnetDataBlock
{
    char m_raw[0xB8];
};
struct NetProviderBnetDescriptorBlob
{
    int                       m_code;
    NetProviderBnetDataBlock  m_data;
};

void NetProviderBnetDescriptorForward::Forward(int code, void* dataPtr)
{
    NetProviderBnetDescriptorBlob blob;
    blob.m_code = code;
    blob.m_data = *(NetProviderBnetDataBlock*)dataPtr;
    ((NetProviderBnetForwardTarget*)this)->Notify(0x11, &blob);
}
