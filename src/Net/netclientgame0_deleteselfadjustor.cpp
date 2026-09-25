//============================================================================
//  0x6F67DB20 - `??_7Game@NetClient@Net@@6B@_0` (the 13-slot table this repo
//  calls Game_0) vtable slot 7 (networking wave 5).
//
//  A pure adjustor thunk: `sub ecx, 10h` then a bare tail `jmp` into the
//  already-reconstructed `Net::NetClient::Game::DeleteSelf`
//  (0x6F6801D0, netclientgame_dtor.cpp). This is the MSVC-generated
//  "vector deleting destructor" thunk for a secondary base sub-object -
//  0x6F67DB30 (NetClientGameBaseDtor::Destroy, still a naked thunk into the
//  real shipped body, see netclientgame_dtor.cpp's own header) stamps
//  *this* 13-slot table's vtable pointer at `this+0x10` inside the very
//  same `Net::NetClient::Game` object whose primary vtable is the other,
//  1-slot table at 0x6F971820. So Game_0 is not a separate class at all:
//  it is `Net::NetClient::Game`'s own second base, reached through a
//  `this` pointer 0x10 bytes further in, and this slot's whole job is to
//  walk that pointer back to the primary `this` before reusing the
//  primary vtable's own DeleteSelf - retn 4 (one stack arg, `flags`),
//  inherited unmodified from the real body it tail-jumps to.
//
//  Naked for the same reason every other bare-adjustor forward in this
//  repo is (asyncfilealiases.cpp, netclientgame0_thunk.cpp): a real call
//  would add a prologue/epilogue and disturb the stack `flags` argument
//  the caller already pushed, which this instruction stream must leave
//  untouched.
//============================================================================
//  Redeclared (not defined) exactly as netclientgame_dtor.cpp declares it,
//  so this TU names the identical mangled symbol for the linker to bind to -
//  the same cross-TU forward-reference shape netprovider_deleteself.cpp's
//  own family already establishes.
namespace Net { namespace NetClient {
class Game
{
public:
    void* DeleteSelf(unsigned int flags);
};
}}

__declspec(naked) void __fastcall NetClientGame0_Slot7_DeleteSelfAdjustor(void*)
{
    __asm { sub ecx, 10h }
    __asm { jmp Net::NetClient::Game::DeleteSelf }
}
