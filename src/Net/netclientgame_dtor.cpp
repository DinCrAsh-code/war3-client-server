//============================================================================
//  Net::NetClient::Game - vtable slot 0, 0x6F6801D0 (`??_7Game@NetClient@
//  Net@@6B@`).  Same "scalar deleting destructor" shape as the other
//  one-slot classes in this family (netrouter_pregame_dtor.cpp,
//  netrouter_game_dtor.cpp, netplayer_dtor.cpp).
//
//  Confirmed genuinely a distinct 1-slot vtable from its RTTI-identical
//  sibling `??_7Game@NetClient@Net@@6B@_0` at 0x6F9717E8 - that second
//  table has 13 slots (see netclientgame0_thunk.cpp), so this is not the
//  multiple-inheritance secondary-base pair the vtable_classes.json claim
//  note speculated; the two tables just happen to share IDA's demangled
//  name because RTTI only carries the qualified class name, not which
//  base's sub-object a given vtable belongs to.
//============================================================================

//  0x6F67DB30 - the base ~Game() body (NetClient::Game, not
//  NetRouter::Game). 18 callees in its own call tree - out of scope for
//  this session, and it carries the same real SEH frame this build's fixed
//  /GS- /EHs-c- cannot reproduce - a thunk to the real, unhooked body
//  rather than a reconstruction. `retn` with no operand (thiscall, no
//  stack args) confirmed off its own listing (0x6F67DE04).
class NetClientGameBaseDtor
{
public:
    void Destroy();
};
__declspec(naked) void NetClientGameBaseDtor::Destroy()
{
    __asm { mov eax, 0x6F67DB30 }
    __asm { jmp eax }
}

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net { namespace NetClient {

class Game
{
public:
    //  0x6F6801D0 - vtable slot 0.
    void* DeleteSelf(unsigned int flags);
};

void* Game::DeleteSelf(unsigned int flags)
{
    ((NetClientGameBaseDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}}  // namespace Net::NetClient
