//============================================================================
//  0x6F2F75C0 (`retn 0Ch`) - the first stack argument is never read.
//  Fetches this object's own +0x210 field, tests it with
//  IsField1B4_6F2F4200 - a leaf Misc/sweep_predicates.cpp already
//  reconstructs for real, for a different receiver class (SIdentityOwner)
//  that shares this object's own +0x210/+0x1B4 layout at the offsets this
//  call site actually touches (the same "reinterpret through a
//  compatible-offset, differently-named struct" this repo already uses -
//  e.g. unit_movementflags.cpp's own SUnitMovement-as-Position calls) -
//  and, only if that guard passes, forwards this function's own second
//  and third arguments to the +0x210 object's own two-argument method
//  (0x6F35AAE0, still out of scope - its own body runs 60+ instructions
//  into a module this call tree does not reach, so it is declared and
//  self-redirect-thunked rather than reconstructed).
//
//  Own translation unit, separate from its one caller
//  (GameUI/cunit_agent1_notifygameui.cpp's NotifyGameUIIfPresent,
//  0x6F333230): the shipped binary calls this out-of-line as a real,
//  separate function, so keeping both in one TU let /O2 inline this
//  trivial-looking body straight into its caller and lose the byte-for-
//  byte match entirely - the same "one TU per original module" rule
//  CLAUDE.md's house rules call out.
//============================================================================
//  Declared locally to match GameUI/cunit_agent1_notifygameui.cpp's own
//  declaration of this struct's mangled name exactly - the two TUs never
//  share a header for it, the same pattern this repo already uses to keep
//  a callee's real definition out-of-line from its one caller.
struct SGameUINotifyLeaf { void __thiscall Notify(int a, int b, int c); };

//  Misc/sweep_predicates.cpp - already reconstructed, real body.  Declared
//  here to match that class's own mangled name and signature exactly
//  rather than redefined.
struct SIdentityOwner
{
    int __thiscall IsField1B4_6F2F4200(void* candidate);
};

//  0x6F35AAE0 - out of scope (see file header above).
struct SIdentityOwnerPairTarget
{
    void __thiscall NotifyPair_6F35AAE0(int b, int c);
};

__declspec(naked) void __thiscall SIdentityOwnerPairTarget::NotifyPair_6F35AAE0(int, int)
{
    __asm { mov eax, 06F35AAE0h }
    __asm { jmp eax }
}

void __thiscall SGameUINotifyLeaf::Notify(int, int b, int c)
{
    void* candidate = *(void**)((char*)this + 0x210);
    if (candidate == 0)
        return;

    if (!((SIdentityOwner*)this)->IsField1B4_6F2F4200(candidate))
        return;

    ((SIdentityOwnerPairTarget*)candidate)->NotifyPair_6F35AAE0(b, c);
}
