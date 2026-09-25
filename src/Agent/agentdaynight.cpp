//============================================================================
//  0x6F429460 - CAgentRegistrySingleton::RebuildDayNightListeners.
//
//  Where the game clock is now decides which boundary it next has to be
//  told about, so this function throws both existing boundary listeners
//  away and builds exactly the ones the current time needs:
//
//    now < dawn            - one listener, above dawn+span   (night)
//    dawn <= now < dusk    - two: below dawn-span, above dusk+span (day)
//    dusk <= now           - one listener, below dusk-span   (night)
//
//  and then publishes the resulting "is it daytime" flag, but only when it
//  actually changed.  The published write goes through the same
//  HandleRefFieldOwner_6F473170 pair CAgentRegistrySingleton::FinishSingleton
//  uses (misc_handle_lookups.cpp), which is what makes the flag observable
//  at all.
//
//  The two message ids are one apart and picked branchlessly in the
//  shipped code (`neg`/`sbb`/`add 80265h`): 0x80264 for "it is day now",
//  0x80265 for "it is night now".  Writing them as a conditional
//  expression is what produces that fold; an if/else over two `push`es
//  does not.
//
//  Own translation unit: it calls into four other modules and is itself a
//  real call from CAgentRegistrySingleton::FinishSingleton.
//
//  Three shapes here are load-bearing:
//
//   * every listener release re-reads the field after the detach call.
//     The shipped code does `call [vtable+8]` and then loads `[esi+78h]`
//     again before touching the refcount, because the detach can clear
//     the field - fold the re-read away and one load disappears.
//
//   * the boundary is bound as a reference to the CFloat sum, not copied
//     into a named local: the shipped code pushes the pointer operator+
//     returned straight into MakeBoundaryListener's argument list, the
//     same idiom pathtraceramp.cpp records for this pair.
//
//   * the event's message id is assigned twice - once before the virtual
//     dispatch and once after it, immediately before the registration
//     call.  Both stores are in the shipped stream, to the same slot;
//     the dispatch is free to have overwritten it.
//============================================================================
#include "agentregistrysingleton.h"
#include "unittrackedref.h"
#include "CFloat.h"

//  Reading a CFloat as the IEEE single it is - see agentregistrysingleton.cpp.
//  By const reference, not by value: the shipped code compares `now`
//  against both bounds with one `fld` of it kept live on the x87 stack
//  across the branch (`fcomp st(1)` pops only the bound).  Taking a CFloat
//  by value copies it into a fresh slot for every comparison, which forces
//  a spill-and-reload pair per test and loses that.
static inline float AsHardwareFloat(const CFloat& value) { return *(const float*)&value; }

//  0x6F009EE0 - configfloat.cpp's.
CFloat __fastcall GetConfigFloat(const char* section, const char* key, int index);

//: dword_6FAAE480 - how far either side of a boundary the listener is
//: armed.  A CFloat constant in the same block as g_CFloatZero
//: (0x6FAAE470); nothing in this call tree gives it a value, only that
//: both listeners are placed at boundary +/- it.
extern const CFloat g_dayNightTransitionSpan;   // 0x6FAAE480

//: ??_7CEvent@@6B@ at 0x6F877928 - the vtable stamped into the record
//: below.  A data symbol rather than a C++ class, for the same reason
//: g_vftFloatListener is one (floatboundarylistener.cpp): the class's own
//: slots are all outside this call tree, and declaring them `virtual`
//: would make this build emit a vtable it cannot fill.
extern void* const g_vftCEvent;

//: 0x6F4269B0 - the visitor the registration below is handed.  Six
//: instructions, `retn 0`: `obj->vtable[3](arg); return 1;` with the
//: object in ecx and the argument in edx.  Its own TU (agenteventvisit.cpp).
int __fastcall DispatchEventToObserver(void* observer, void* event);

//: 0x6F2A3270 - register the record as an event binding.  __cdecl and
//: genuinely variadic: after the callback and its context comes a stream
//: of tag words this call closes with 0x1F.  See agenteventbinding.cpp.
int __cdecl BuildAndRegisterEventBinding(void* callback, void* context, ...);

//  The two message ids, one apart.
const unsigned int kEventBecameDay   = 0x80264;
const unsigned int kEventBecameNight = 0x80265;

//  The id every boundary listener this function makes is armed with.
const unsigned int kEventDayNightBoundary = 0x80263;

//----------------------------------------------------------------------------
//  What a FloatListener looks like from here.  Same minimal shape
//  floatboundarylistener.cpp declares - a plain vtable word and a
//  refcount - because the class's own eight slots are outside this tree
//  and a `virtual` declaration would emit a vtable this build cannot
//  fill.  The two slots that *are* reached are reached as hand-written
//  dispatches, with their argument counts read off the call sites: both
//  are `call edx` with nothing pushed and ecx untouched, so both are
//  `retn 0`.
//----------------------------------------------------------------------------
struct FloatListener
{
    const void* m_vtable;   // +0x00
    int         m_refcount; // +0x04
};

typedef void (__thiscall *ListenerSlotFn)(FloatListener*);

static inline void DetachListener(FloatListener* p)
{
    ((ListenerSlotFn)(*(void***)p)[0x08 / 4])(p);   // slot 2
}

static inline void ReleaseListenerSelf(FloatListener* p)
{
    ((ListenerSlotFn)(*(void***)p)[0x00 / 4])(p);   // slot 0
}

//  The counted reference SUnitTrackedRef::MakeBoundaryListener returns.
//  Declared here with its destructor: the shipped code carries one of
//  these across each branch and releases it at scope exit, which is the
//  five-instruction inline block after every assignment and the SEH state
//  word that guards it.
struct SFloatListenerRef
{
    ~SFloatListenerRef()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                ReleaseListenerSelf(m_ptr);
        }
    }

    FloatListener* m_ptr;
};

//----------------------------------------------------------------------------
//  The 0x10-byte record the change is announced with.  Field order is the
//  layout the four stores fix, not the order they are issued in.
//----------------------------------------------------------------------------
struct SDayNightEvent
{
    const void*  m_vtable;      // +0x00
    int          m_reserved04;  // +0x04
    unsigned int m_msgId;       // +0x08
    int          m_reserved0C;  // +0x0C
};

//  Slot 4 (+0x10) on the singleton's own vtable: announce a record.  The
//  call site pushes exactly one argument and cleans nothing itself, so
//  the record is that argument and the slot is `retn 4`.
typedef void (__thiscall *AnnounceEventFn)(void*, SDayNightEvent*);

//  0x6F473140 / 0x6F473170 - misc_handle_lookups.cpp's, re-declared here
//  identically so the two mangled names are one symbol.
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;
    int          m_typeTag;

    int QueryField78();
    void SetField78(int value);
};

//----------------------------------------------------------------------------
//  Throw one listener away.  Inline on purpose: the shipped code has this
//  block twice, written out, with no call between them.
//----------------------------------------------------------------------------
static inline void DropListener(FloatListener** slot)
{
    if (*slot)
    {
        DetachListener(*slot);

        //  Re-read: the detach above can have cleared the field.
        FloatListener* p = *slot;
        if (p)
        {
            if (--p->m_refcount == 0)
                ReleaseListenerSelf(p);
        }

        *slot = 0;
    }
}

//----------------------------------------------------------------------------
//  Adopt one.  Addref the new value first, then release the old, then
//  store - the shipped order, and the one that is safe when the two are
//  the same object.
//----------------------------------------------------------------------------
static inline void AdoptListener(FloatListener** slot, const SFloatListenerRef& ref)
{
    if (ref.m_ptr)
        ref.m_ptr->m_refcount++;

    FloatListener* old = *slot;
    if (old)
    {
        if (--old->m_refcount == 0)
            ReleaseListenerSelf(old);
    }

    *slot = ref.m_ptr;
}

void CAgentRegistrySingleton::RebuildDayNightListeners()
{
    DropListener((FloatListener**)&m_pDawnListener);
    DropListener((FloatListener**)&m_pDuskListener);

    SUnitTrackedRef* clock =
        (SUnitTrackedRef*)((IndexedElementOwner_6F427F40*)this)->At(2);

    CFloat now(kCFloatNoInit);
    clock->GetValue(&now);

    //  The load-and-store-back the shipped code really has: `now` is read
    //  out of the buffer GetValue filled and written into a local of its
    //  own.  Fold it away and the pair goes with it.
    CFloat when = now;

    CFloat dawn = GetConfigFloat("Misc", "Dawn", 0);
    CFloat dusk = GetConfigFloat("Misc", "Dusk", 0);
    CFloat span = g_dayNightTransitionSpan;

    int isDaytime;

    //  The three-way split, in clock order.  Spelling the two tests as
    //  nested `<=` instead was measured (0.467 against 0.470) and moves
    //  nothing: this toolchain lowers the x87 predicate differently from
    //  the shipped one either way - it emits `fcom st(1)` plus a separate
    //  `fstp st(1)` and a `test ah,5` / `jp` parity test where the shipped
    //  build has one `fcomp st(1)` and `test ah,41h` / `jne`.  Same
    //  comparison, same branch, one extra instruction per test.
    if (AsHardwareFloat(dawn) > AsHardwareFloat(when))
    {
        //  Before dawn: wait for the clock to come up past dawn.
        const SFloatListenerRef& ref = clock->MakeBoundaryListener(
            1, &(dawn + span), kEventDayNightBoundary, this, 0);
        AdoptListener((FloatListener**)&m_pDawnListener, ref);

        isDaytime = 0;
    }
    else if (AsHardwareFloat(dusk) > AsHardwareFloat(when))
    {
        //  Daytime: both ends are live.
        const SFloatListenerRef& low = clock->MakeBoundaryListener(
            0, &(dawn - span), kEventDayNightBoundary, this, 0);
        AdoptListener((FloatListener**)&m_pDawnListener, low);

        const SFloatListenerRef& high = clock->MakeBoundaryListener(
            1, &(dusk + span), kEventDayNightBoundary, this, 0);
        AdoptListener((FloatListener**)&m_pDuskListener, high);

        isDaytime = 1;
    }
    else
    {
        //  Past dusk: wait for the clock to come back down past it.
        const SFloatListenerRef& ref = clock->MakeBoundaryListener(
            0, &(dusk - span), kEventDayNightBoundary, this, 0);
        AdoptListener((FloatListener**)&m_pDuskListener, ref);

        isDaytime = 0;
    }

    HandleRefFieldOwner_6F473170* flag =
        (HandleRefFieldOwner_6F473170*)m_isDaytime;

    if (isDaytime != flag->QueryField78())
    {
        flag->SetField78(isDaytime);

        unsigned int msgId = isDaytime ? kEventBecameDay : kEventBecameNight;

        SDayNightEvent event;
        event.m_vtable = &g_vftCEvent;
        event.m_reserved04 = 0;
        event.m_msgId = msgId;
        event.m_reserved0C = 0;

        ((AnnounceEventFn)(*(void***)this)[0x10 / 4])(this, &event);

        //  Assigned again: the dispatch above is free to have overwritten
        //  it, and the shipped code stores it a second time right here.
        event.m_msgId = msgId;

        BuildAndRegisterEventBinding((void*)DispatchEventToObserver, &event,
                                     4, 6, 0x1F);
    }
}
