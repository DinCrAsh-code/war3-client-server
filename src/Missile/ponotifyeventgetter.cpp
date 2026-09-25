//============================================================================
//  0x6F4AABF0 - a trivial +0x10 getter on the small event record
//  `CBhPoProjectile::NotifyVelocityDelta` receives
//  (Missile/bhpoprojectile_notify.cpp).  `retn 0`, no arguments.
//
//  Own translation unit even though the body is two instructions: the
//  shipped code reaches it through a real `call`, and `/Ob2` would
//  otherwise fold something this small straight into its one caller in
//  this session's own tree (CLAUDE.md's own "watch for /Ob2
//  over-inlining" note) - kept as a real leaf the same way
//  Misc/misc_handle_resolvers.cpp's own batch of small handle-resolve
//  getters already are.
//============================================================================
struct SPoNotifyEventArg
{
    void* __thiscall GetTarget();
};

void* __thiscall SPoNotifyEventArg::GetTarget()
{
    return *(void**)((char*)this + 0x10);
}
