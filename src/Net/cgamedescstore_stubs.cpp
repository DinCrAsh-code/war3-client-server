//============================================================================
//  Net::CGameDescStore - two of its own plain constant-answer slots (0x6F97
//  0E80's slots 2 and 3): each ignores every argument, `this` included, and
//  returns 0 - the same "constant-answer" shape
//  Net::NetProvider::ConstAnswer_SlotN already documents (netprovider_stubs
//  .cpp), just with a `xor eax,eax` answer instead of a nonzero one. The
//  argument count is read straight off each one's own `retn`.
//============================================================================

namespace Net
{

class CGameDescStore
{
public:
    //  0x6F4C1BA0 - vtable slot 2.  `retn 14h` - 5 stack dwords, all unused.
    int ZeroAnswer_Slot2(int, int, int, int, int);

    //  0x6F654250 - vtable slot 3.  `retn 1Ch` - 7 stack dwords, all unused.
    int ZeroAnswer_Slot3(int, int, int, int, int, int, int);
};

int CGameDescStore::ZeroAnswer_Slot2(int, int, int, int, int) { return 0; }
int CGameDescStore::ZeroAnswer_Slot3(int, int, int, int, int, int, int) { return 0; }

}  // namespace Net
