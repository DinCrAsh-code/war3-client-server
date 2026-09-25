//============================================================================
//  0x6F72D750 - reached from the same CItem vtable BFS as
//  citemclosure_sound_gates.cpp/citemclosure_terrain_gates3.cpp (batch 8 of
//  8, docs/targets/CItem-callee-batch8.md), landing in yet another unrelated
//  engine subsystem: a circular array of paired floats, resized in place
//  with wraparound (the `div`-by-current-count / "shift everything past the
//  new write point" shape, then two `fst`s per relocated slot into an
//  interleaved 8-byte-stride buffer at `this+8`).  Both helpers it calls
//  (sub_6F50F500 - picks the new element count when the caller passes none,
//  sub_6F34C150 - performs the wraparound shift itself) are out of this
//  batch's scope and this call tree never establishes the element type
//  (float pair, colour, 2-vector - all shapes read the same at this level),
//  so - per the same "guessing a layout is worse than not looking" reasoning
//  citemclosure_terrain_gates3.cpp's STerrainHashTableThunk documents -
//  it is a direct redirect rather than an invented reconstruction.  0%
//  instruction match by construction.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  `retn 4`, __thiscall(ecx=buffer, stack=newcount).
struct SCircularPairBufferThunk
{
    void __thiscall Resize(int newcount);
};
ADDR_THUNK(void __thiscall SCircularPairBufferThunk::Resize(int),
           0x6F72D750)
