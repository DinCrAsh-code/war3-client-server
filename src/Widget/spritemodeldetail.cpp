//============================================================================
//  0x6F4D3550 - "does the sprite's model carry detail flag 0x10".
//
//  Was a `mov eax,<address> / jmp eax` redirect under the name GetModel; it
//  is six instructions, and what it hands back is not a model but the
//  answer of a flag test already reconstructed one level down
//  (SField98Owner::TestSubObjectFlag10, 0x6F501D80,
//  misc_trivial_getters.cpp).  The name is corrected here to say so; the
//  old one is dropped from tools/funcmap.py in the same change.
//
//  Null in +0x20 answers 0 without dispatching, which is why the tail jump
//  is guarded rather than unconditional.
//
//  Own translation unit: the flag test below is reached with a real tail
//  jump, so it cannot share a file with this.
//============================================================================

//  0x6F501D80 - already reconstructed in misc_trivial_getters.cpp.
//  Declared, not re-thunked.
struct SField98Owner
{
    int __thiscall TestSubObjectFlag10();
};

struct SSpriteModelQuery
{
    int QueryModelDetailFlag();

    char           m_reserved00[0x20];
    SField98Owner* m_pModel;    // +0x20
};

int SSpriteModelQuery::QueryModelDetailFlag()
{
    //  The guard is spelled the positive way round: the shipped code falls
    //  *through* into the tail jump and branches away to the zero, so an
    //  `if (model == 0) return 0;` early exit inverts the branch and puts
    //  the tail jump last.
    SField98Owner* model = m_pModel;
    if (model != 0)
        return model->TestSubObjectFlag10();
    return 0;
}
