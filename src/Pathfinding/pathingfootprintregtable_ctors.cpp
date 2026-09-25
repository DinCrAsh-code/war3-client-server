//============================================================================
//  0x6F3FC9A0 / 0x6F3FCBC0 - TSHashTable<CPathingFootprintReg,HASHKEY_STRI>'s
//  own constructor and destructor.  Redirects, not reconstructions, for the
//  exact reason Containers/hashtablectors.cpp's own header note gives for
//  every other TSHashTable instantiation in this repo: constructing (or
//  properly tearing down) one needs MSVC to stamp/consult
//  `??_7?$TSHashTable@UCPathingFootprintReg@@VHASHKEY_STRI@@@@6B@`, and the
//  vtable this repo's own `TSHashTable<T,K>` template (storm.h) would stamp
//  is a two-slot one (`~TSHashTable`, `AllocNode`) where the shipped table
//  - confirmed this session, agent_worktrees/classes/0x6F94CCF0.json - is a
//  real **four**-slot table (`DeleteNode`, `AllocNode`, and two more this
//  session did not reach). A real constructor here would install a
//  wrong-length vtable that `check_vtables.py` exists to catch; a redirect
//  leaves the shipped table's own vtable untouched.  `DeleteNode`
//  (0x6F3FCB80) and `AllocNode` (0x6F3FCAA0) themselves ARE reconstructed
//  this session - see pathingfootprintreg.cpp - as free functions, not as
//  this repo's own template's virtuals, for the same reason.
//============================================================================

//  Member functions on an opaque storage struct, the same technique
//  Containers/hashtablectors.h/.cpp uses for its own four redirects -
//  MSVC will not take `__thiscall` on a free function (and will not take
//  `__declspec(naked)` on a real constructor/destructor either, the same
//  file's own reasoning).
struct SPathingFootprintRegTable
{
    //  0x6F3FC9A0 - `retn 14h`.
    SPathingFootprintRegTable* Construct(void* a1, void* a2, void* a3, void* a4, void* a5);
    //  0x6F3FCBC0 - `retn 0`.
    void Destruct();
};

__declspec(naked) SPathingFootprintRegTable* SPathingFootprintRegTable::Construct(
    void*, void*, void*, void*, void*)
{
    __asm { mov eax, 06F3FC9A0h }
    __asm { jmp eax }
}

__declspec(naked) void SPathingFootprintRegTable::Destruct()
{
    __asm { mov eax, 06F3FCBC0h }
    __asm { jmp eax }
}
