//============================================================================
//  Two one-instruction `jmp` aliases IDA gives their own headings, into two
//  destructors this batch does not otherwise reach:
//
//    0x6F3364E0  `jmp sub_6F603800` - CSimpleButton::~CSimpleButton, not
//                yet reconstructed (no heading in this batch's own dump
//                reaches its own body, only this alias and
//                GameUI/commandbuttondtor.cpp's own real call into it).
//    0x6F338950  `jmp sub_6F609DD0` - CSimpleFrame::~CSimpleFrame, same
//                situation.
//
//  Left as raw-address redirects (not a `jmp RealSymbol` the way
//  Agent/handleobject.cpp's own alias is) because neither target has a
//  C++ declaration in this tree yet for the assembler to resolve - the
//  same "callee outside this batch's own dump" situation
//  GameUI/selectionhostdtor.cpp's own forward declaration is for a called
//  (not jumped-to) function.  `retn 0` both, thiscall.
//============================================================================
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F3364E0
struct SCSimpleButtonDtorAlias
{
    void Dtor();
};
ADDR_THUNK(void SCSimpleButtonDtorAlias::Dtor(), 0x6F603800)

//  0x6F338950
struct SCSimpleFrameDtorAlias
{
    void Dtor();
};
ADDR_THUNK(void SCSimpleFrameDtorAlias::Dtor(), 0x6F609DD0)

#undef ADDR_THUNK
