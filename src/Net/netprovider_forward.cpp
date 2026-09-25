//============================================================================
//  Net::NetProvider - vtable slots 34 and 35, 0x6F657210 / 0x6F657220.
//  Each is a bare tail-jump into the object's own +0x2B4 inner interface's
//  vtable+0x14 / vtable+0x18 slot: `this` reinterpreted, no stack traffic
//  of its own, so whatever the caller pushed for this slot passes straight
//  through unmodified to the real target's own `retn`.
//
//  A `__declspec(naked)` body is not eligible for the usual register-
//  allocation excuses (CLAUDE.md) - MSVC does no scheduling inside one, so
//  this should be an exact instruction-for-instruction match by
//  construction, and a plain forwarding tail-jmp through an unknown-arity
//  inner vtable slot has no C++ spelling that guarantees the same shape,
//  so it is written as the literal asm instead.
//============================================================================

namespace Net
{

class NetProvider
{
public:
    void ForwardToInner_0x14();
    void ForwardToInner_0x18();
};

//  0x6F657210 - vtable slot 34.
__declspec(naked) void NetProvider::ForwardToInner_0x14()
{
    __asm
    {
        mov     ecx, [ecx + 0x2B4]
        mov     eax, [ecx]
        mov     eax, [eax + 0x14]
        jmp     eax
    }
}

//  0x6F657220 - vtable slot 35.
__declspec(naked) void NetProvider::ForwardToInner_0x18()
{
    __asm
    {
        mov     ecx, [ecx + 0x2B4]
        mov     eax, [ecx]
        mov     edx, [eax + 0x18]
        jmp     edx
    }
}

}  // namespace Net
