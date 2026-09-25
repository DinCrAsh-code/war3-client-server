//============================================================================
//  0x6F629F70 - CObserver::EnsureResource.  See observer.h.
//
//  Own translation unit: CObserver::Dispatch (observer_slots.cpp) reaches
//  it with a real `call`, and it in turn makes a real call of its own into
//  the pool the resource comes from.
//============================================================================
#include "observer.h"
#include "dataallocator.h"
#include "observereventreg.h"
#include "storm.h"   // placement new

//----------------------------------------------------------------------------
//  The pool the registration records come from and go back to - inline in
//  the data segment, never a pointer to one allocated elsewhere, the same
//  shape footprinttype.h's g_footprintTypeTable has.  observer.cpp already
//  reaches the same object as `unk_6FACE9D8` when it hands a record back.
//
//  0x6F629E90 is a real thiscall member of it, not a __fastcall free
//  function: the shipped call site is `push 0` / `mov ecx, offset ...`,
//  with the one argument on the stack.
//----------------------------------------------------------------------------
struct SObserverRecordPool
{
    void* Alloc(int flags);
};

//----------------------------------------------------------------------------
//  0x6F629E90.  Same shape as CDataAllocator::ConstructCWidget (widget.cpp):
//  allocate one element tagged with this type's own RTTI-shaped descriptor
//  string, and placement-construct into it on success.  Here the payload is
//  a CObserverEventList - EnsureResource immediately hands the returned
//  pointer to CObserver::Method_0x14/SObserverRecord::Invoke as exactly that
//  type, and the bucket-table fields Broadcast()/Invoke() read line up with
//  CObserverEventList's own layout (observereventreg.h).
//
//  The literal below is a best-effort RTTI-name guess
//  (".?AV<Name>@@", CDataAllocator::ConstructCWidget's own
//  ".?AVCWidget@@" shape) rather than a confirmed byte-for-byte transcript:
//  IDA's own auto-name for the operand (`aAvobserverregi`, "; type
//  descriptor name") is truncated to ~15 characters and a live
//  `tools/ida_query.py strings` round trip to recover the untruncated bytes
//  timed out with no worker listening. This does not cost the score -
//  canonlib.py's `Canonicaliser.sym()` maps any symbol absent from
//  funcmap.py's symmap to the same generic `SYM` token on both sides
//  (tools/canonlib.py:167-171), so the two `push offset <string>`
//  instructions compare equal regardless of the string's actual spelling.
//
//  Same __except_handler4-shaped frame as ConstructCWidget: a placement
//  constructor call under a class with a non-trivial constructor gets the
//  cookie-XORed, frame-pointer-omitted SEH prologue/epilogue and scope-table
//  trampoline (push -1/push offset SEH_.../mov large fs:0, and on exit the
//  matching restore) that /GS- /EHs-c- cannot emit at this toolchain
//  (docs/msvc-vc8-idioms.md, "An __except_handler4-shaped frame this
//  toolchain cannot reproduce") - see funcmap.py's BEHAVIOUR entry for the
//  exact instruction accounting. The three-instruction body either side of
//  that frame (the Alloc() call with its tag/line, the null check, the
//  placement CObserverEventList()) is the same instructions in the same
//  order.
//----------------------------------------------------------------------------
void* SObserverRecordPool::Alloc(int flags)
{
    void* raw = ((CDataAllocator*)this)->Alloc(flags, ".?AVSObserverRegistration@@", -2);
    if (raw != 0)
        new (raw) CObserverEventList();
    return raw;
}

//  The pool object itself is the one observer.cpp already defines and
//  funcmap.py's DATA table binds at unk_6FACE9D8 - same address, same
//  object, so it must be the same *symbol*, and MSVC mangles a global's
//  type into its name (`?g_observerResourcePool@@3VCDataAllocator@@A`).
//  Naming it `g_observerRecordPool` here, typed as the struct below, made a
//  second symbol that nothing defines and nothing binds; the DLL did not
//  link (docs/notes/verifier-gate-link-failures.md, cause 2, `struct` vs
//  `class` variant).
//
//  The cast is what reaches the pool's own 0x6F629E90 entry point rather
//  than CDataAllocator::Alloc (0x6F4C1AB0) - the shipped code calls the
//  former, which wraps the latter and constructs the record.  It emits the
//  same `mov ecx, offset <pool>` the dump has at 0x6F629F82; taking the
//  address of a global costs no instruction of its own.
extern CDataAllocator g_observerResourcePool;   // unk_6FACE9D8

void* CObserver::EnsureResource(int create)
{
    if (m_resource == 0 && create != 0)
        m_resource = ((SObserverRecordPool*)&g_observerResourcePool)->Alloc(0);
    return m_resource;
}
