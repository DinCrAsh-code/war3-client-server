//============================================================================
//  Net::NetProvider - vtable slot 2, 0x6F65BE40.  Tell vtable+0x90
//  (NetProvider__Method_0x90, netprovider_method0x90.cpp) "code 4", then
//  run the same lazily-allocated-event teardown Method_0x90's own SEH
//  cleanup calls, and report success.
//============================================================================

//  NetProviderMethod0x90Self::Cleanup - the real body lives in
//  netprovider_method0x90.cpp.  Only the qualified name and signature have
//  to match for the link to resolve (the mangled symbol carries no layout
//  information), so this is a minimal re-declaration rather than a second
//  copy of that struct - the same technique src/Misc/unreconstructed_
//  thunks.cpp's own MEMBER_THUNK block uses for a call into another TU's
//  class.
class NetProviderMethod0x90Self
{
public:
    void Cleanup();
};

namespace Net
{

class NetProvider
{
public:
    //  0x6F65BE40 - vtable slot 2.
    int Method_0x2();
};

int NetProvider::Method_0x2()
{
    //  vtable+0x90, `this` in ecx, 3 stack ints (retn 0Ch confirms it) -
    //  the same slot netprovider_slot9/slot25 reach with different codes.
    typedef void (__thiscall *Method0x90Fn)(void*, int, int, int);
    ((Method0x90Fn)(*(void***)this)[0x90 / 4])(this, 4, 0, 0);

    ((NetProviderMethod0x90Self*)this)->Cleanup();
    return 1;
}

}  // namespace Net
