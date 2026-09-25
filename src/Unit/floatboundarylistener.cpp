//============================================================================
//  0x6F477550 - SUnitTrackedRef::MakeBoundaryListener: make a
//  FloatListener that fires when this tracked value crosses one boundary,
//  bind it, and hand it back as a counted reference.
//
//  Its two call sites are both in
//  CAgentRegistrySingleton::RebuildDayNightListeners (0x6F429460,
//  agentdaynight.cpp) - one listener for dawn, one for dusk - which is
//  what the `above` flag selects between.
//
//  `this` is the tracked ref itself: the caller reaches it through
//  IndexedElementOwner_6F427F40::At(2) and calls
//  SUnitTrackedRef::GetValue on the same pointer, so the two are the same
//  object.
//
//  Return by value of a counted reference: the shipped `retn 18h` covers
//  six stack dwords for five source parameters, and the sixth is MSVC's
//  hidden return buffer - which is exactly the `mov eax,[esp+arg_0]` /
//  `mov [eax],esi` / conditional addref at the tail, and the reason eax
//  comes back holding that buffer.
//
//  The FloatListener is sixteen bytes off Storm's allocator with the
//  shipped __FILE__/__LINE__ (".\\FloatProp.cpp", 105), and a failed
//  allocation is carried through as a null rather than checked: the
//  `Init` call below runs with `this` null on that path in the shipped
//  code too (`mov ecx,esi` with esi zeroed), which is why the null test
//  produces two joined paths and not an early return.
//
//  Own translation unit: it calls into three other modules and is called
//  from a fourth.
//============================================================================
#include "floatlistener.h"
#include "storm.h"

static const char kFloatPropCpp[] = ".\\FloatProp.cpp";

//----------------------------------------------------------------------------
//  Slot 5 (+0x14) on the tracked ref's own vtable: the subject the new
//  listener is bound onto.  Reached as a hand-written slot dispatch
//  because SUnitTrackedRef's vtable is TRefCnt's and this repo has no
//  committed dump of it - `retn 0`, so the typedef pushes nothing, which
//  is what tools/vtable_dispatch_audit.py would check if it had that
//  dump (it does not, so this site lands in its skipped bucket - unknown,
//  not clean).  The argument count is read off the call site itself:
//  `mov eax,[edi]` / `mov edx,[eax+14h]` / `call edx` with ecx untouched
//  and nothing pushed.
//
//  What it hands back is the modifier *subject*: FloatListener::Init
//  (floatlistenerinit.cpp) resolves its {handle, typeTag} pair at
//  +0x0C/+0x10 exactly the way MovementModifier::Bind resolves its own,
//  which is what fixes the type.
//----------------------------------------------------------------------------
typedef SModifierSubject* (__thiscall *GetListenSubjectFn)(void*);

SFloatListenerRef SUnitTrackedRef::MakeBoundaryListener(
        int above, const CFloat* boundary, unsigned int msgId,
        void* subject, int mode)
{
    SModifierSubject* target =
        ((GetListenSubjectFn)(*(void***)this)[0x14 / 4])(this);

    FloatListener* listener =
        new (SMemAlloc(sizeof(FloatListener), kFloatPropCpp, 105, 0)) FloatListener();

    //  Not guarded: the shipped code calls Init on the null too.
    listener->Init(target, this, above, boundary, msgId, subject, mode);

    SFloatListenerRef ref;
    ref.m_ptr = listener;
    if (listener)
        listener->m_refcount++;

    return ref;
}
