//============================================================================
//  Net::NetRouter::Game - vtable slot 0, 0x6F671290.  Same "scalar deleting
//  destructor" shape as PreGame's own (netrouter_pregame_dtor.cpp) and
//  Net::NetProvider's (netprovider_deleteself.cpp).
//============================================================================

//  0x6F670540 - the base ~Game() body.  A much larger teardown than
//  PreGame's own (17 callees in its own call tree - session/record/timer
//  cleanup this session did not further trace) and it carries the same
//  real SEH frame this build's fixed /GS- /EHs-c- cannot reproduce - a
//  thunk to the real, unhooked body rather than a reconstruction. `retn`
//  with no operand (thiscall, no stack args) confirmed off its own listing.
class GameBaseDtor
{
public:
    void Destroy();
};
__declspec(naked) void GameBaseDtor::Destroy()
{
    __asm { mov eax, 0x6F670540 }
    __asm { jmp eax }
}

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net { namespace NetRouter {

class Game
{
public:
    //  0x6F671290 - vtable slot 0.
    void* DeleteSelf(unsigned int flags);
};

void* Game::DeleteSelf(unsigned int flags)
{
    ((GameBaseDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}}  // namespace Net::NetRouter
