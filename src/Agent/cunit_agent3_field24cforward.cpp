//============================================================================
//  0x6F2F4930 - `mov ecx,[ecx+24Ch]` / `jmp sub_6F37D490`: load a pointer
//  member at +0x24C and tail-jump one argument straight into whatever it
//  points at, the same adjustor-to-a-pointer-member shape
//  CAgent::PostEventToSelfObserver (0x6F471A40, agent_selfobserverpost.cpp)
//  is for an *embedded* sub-object - the difference being a load here
//  instead of an `add`, because +0x24C holds a pointer rather than an
//  embedded object.
//
//  sub_6F37D490 (`retn 4`, confirmed off its own tail bytes) is well
//  outside this agent's own worklist slice - not in any of the eight
//  parallel agents' worklists at all in this run - so its class is
//  declared here only far enough to name the one method this forwards to,
//  not reconstructed.
//============================================================================

//  Declared, not defined: 0x6F37D490 is out of scope for this batch (see
//  above).  `retn 4` is confirmed off the shipped bytes; the parameter's
//  own type could not be - int is the least committed 4-byte reading that
//  reproduces the same push/call shape.
class SUnreconstructedTarget_6F37D490
{
public:
    void Method(int arg);
};

struct SFieldAt0x24COwner
{
    char m_reserved000[0x24C];
    SUnreconstructedTarget_6F37D490* m_field24C;   // +0x24C

    void Forward(int arg);
};

void __thiscall SFieldAt0x24COwner::Forward(int arg)
{
    m_field24C->Method(arg);
}

//----------------------------------------------------------------------------
//  Link-completion thunk - 0x6F37D490 is out of scope for every one of the
//  eight parallel agents this run (see the header comment), so nothing
//  else in the tree defines it either. A naked redirect thunk - the same
//  idiom this repo uses for any other out-of-scope callee - makes this TU
//  link without claiming the address as a real reconstruction (no
//  funcmap.py row points at it).
//----------------------------------------------------------------------------
__declspec(naked) void SUnreconstructedTarget_6F37D490::Method(int)
{
    __asm { mov eax, 06F37D490h }
    __asm { jmp eax }
}
