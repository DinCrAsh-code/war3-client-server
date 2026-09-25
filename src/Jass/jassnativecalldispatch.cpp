//============================================================================
//  JASS VM/compiler core internals (jassvm-core-A) - the native-vs-user-
//  defined-function call dispatch for a JASS function-reference symbol,
//  and the thin wrapper that spawns a child instance's own interpreter
//  entry.  batch-3's own released-unfinished closure (see
//  jassvm_callee_worklist.json / user_knowledge.json for the addresses
//  below); this session finished it.
//============================================================================
#include "jassinstance.h"
#include "jassscopetable.h"
#include "jassarray.h"
#include "cdatastore.h"
#include "handleobject.h"
#include "jassenginehandlerefs.h"
#include "cstringrep.h"
#include "rcstring.h"
#include "jassnatives.h"

//  0x6F42A3C0 - jassreadpointerarray.cpp.
CDataStore* __fastcall JassReadPointerArray(CDataStore* store, TSGrowableArray<void*>* arr);

//----------------------------------------------------------------------------
//  0x6F45AF10 / 0x6F4597A0 - a hash-table find and a two-node TSLink
//  bucket unlink over JassInstance's own +0x287C table (no dump for
//  either - see jassinstance.h's own note on that field).  Left naked
//  redirects rather than reconstructed from scratch: neither is the JASS
//  VM's own logic, both are the same kind of generic hash-table
//  bookkeeping jasslinklistrelink.cpp already keeps thunked, and nothing
//  in this closure needs their bodies to model ReleaseCompiledFunc's own
//  control flow.
//----------------------------------------------------------------------------
class JassNativeSlotTableOps
{
public:
    void* Find(unsigned int key);
    void  Unlink(void* entry);
};

__declspec(naked) void* JassNativeSlotTableOps::Find(unsigned int)
{
    __asm
    {
        mov eax, 06F45AF10h
        jmp eax
    }
}

__declspec(naked) void JassNativeSlotTableOps::Unlink(void*)
{
    __asm
    {
        mov eax, 06F4597A0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45B1A0 - `retn 4`.  On the last live reference to a compiled
//  user-defined function (index `index` into m_field2874's handle table),
//  releases its handle, unlinks and destroys its record in m_field287C's
//  own table (calling that table's own vtable slot 0 - not reconstructed,
//  see jassarray.h's "vtable you cannot declare" idiom for why this is a
//  raw function-pointer dispatch rather than a `virtual`), clears the
//  record's own string field, and appends the freed `index` to
//  m_field2878's free-index list.  Returns the address of that freshly
//  grown slot (whatever eax already holds on every early-exit path, which
//  this reconstruction settles to null rather than leaving genuinely
//  undefined).
//----------------------------------------------------------------------------
void** JassInstance::ReleaseCompiledFunc(unsigned int index)
{
    if (index == 0)
        return 0;

    char* record = (char*)m_field2874->m_entries + (index << 4);
    if (--*(int*)(record + 0xC) != 0)
        return 0;

    unsigned int handle = GetHandleOrZeroAlias((const SHandleHolder*)record);

    JassNativeSlotTableOps* table = (JassNativeSlotTableOps*)m_field287C;
    void* bucket = table->Find(handle);
    table->Unlink(bucket);

    typedef void (__thiscall *TableSlot0Fn)(void*, void*);
    void** vtable = *(void***)m_field287C;
    ((TableSlot0Fn)vtable[0])(m_field287C, bucket);

    ((STStringField*)record)->Assign(0);

    void** slot = m_field2878->Grow();
    *slot = (void*)index;
    return slot;
}

//----------------------------------------------------------------------------
//  0x6F45B2E0 - `retn 4`.  A symbol's own native-vs-user-function
//  dispatch, the "resolve/invoke with the symbol's own decl as the
//  argument" half: kind 7 (native) tail-calls the instance's resolved
//  native function pointer with the symbol's own +0x20 payload as its
//  first argument; kind 6 (compiled user function) tail-calls
//  ReleaseCompiledFunc with that same payload as the index.  Anything
//  else, or a null `instance`, returns null.
//----------------------------------------------------------------------------
void* JassSymbolNode::DispatchWithOwnDecl(JassInstance* instance)
{
    if (m_declKind == 7)
    {
        void* fn = instance ? instance->m_nativeFuncPtr : 0;
        if (!fn)
            return 0;

        void* arg = instance->m_nativeFuncArg;
        typedef void* (__fastcall *NativeThunkFn)(void*, int, void*);
        return ((NativeThunkFn)fn)(m_declData, 1, arg);
    }

    if (m_callKind == 6)
    {
        if (!instance)
            return 0;

        return instance->ReleaseCompiledFunc((unsigned int)m_declData);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F459750 - `retn 8`.  The same dispatch's "call with a caller-
//  supplied argument" half: kind 7 calls the instance's resolved native
//  function pointer with the caller's own `arg` (not the symbol's own
//  decl) and an explicit zero second argument, returning its result; kind
//  6 tail-calls JassEngineHandleOwner::AddHandleRef(instance, arg)
//  instead of releasing anything.
//----------------------------------------------------------------------------
void* JassSymbolNode::DispatchWithArg(JassInstance* instance, void* arg)
{
    if (m_declKind == 7)
    {
        void* fn = instance ? instance->m_nativeFuncPtr : 0;
        if (!fn)
            return 0;

        void* nativeArg = instance->m_nativeFuncArg;
        typedef void* (__fastcall *NativeThunkFn)(void*, int, void*);
        return ((NativeThunkFn)fn)(arg, 0, nativeArg);
    }

    if (m_callKind == 6 && instance != 0)
    {
        ((JassEngineHandleOwner*)instance)->AddHandleRef((int)arg);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F45C170 - `retn 8`.  Deserializes a symbol's own resolved binding off
//  `store`: a discarded dword, a call-kind, a decl-kind, an "is array"
//  flag and a count (clamped to 0x1FFF).  When the array flag is set,
//  this is the same object as JassArrayRefSite (jassarray.h - identical
//  +0x18/+0x1C/+0x20 layout): acquires/resizes its own cached JassArray to
//  `count` elements and, unless `count` is zero, reads that many raw
//  pointers straight into the array's own backing TSGrowableArray via
//  JassReadPointerArray.  Otherwise (a scalar symbol) reads one more dword
//  as the resolved value and calls UpdateBinding with it - through a fresh
//  global-slot pointer when the decl-kind read earlier was 3, or the raw
//  value itself otherwise.
//----------------------------------------------------------------------------
void* JassSymbolNode::DispatchOrEvaluate(JassInstance* instance, CDataStore* store)
{
    unsigned int discard = 0;
    store->ReadDword(&discard);

    unsigned int callKind = 0;
    store->ReadDword(&callKind);

    unsigned int declKind = 0;
    store->ReadDword(&declKind);

    m_callKind = callKind;
    m_declKind = declKind;

    unsigned int isArray = 0;
    store->ReadDword(&isArray);

    unsigned int count = 0;
    store->ReadDword(&count);
    if (count >= 0x2000)
        count = 0x1FFF;

    if (isArray != 0)
    {
        ((JassArrayRefSite*)this)->AcquireArray(count, 1);

        if (count == 0)
        {
            unsigned int discard2 = 0;
            store->ReadDword(&discard2);
            return 0;
        }

        JassArray* array = ((JassArrayRefSite*)this)->m_array;
        if (array != 0)
            return JassReadPointerArray(store, (TSGrowableArray<void*>*)&array->m_elementAlloc);

        return JassReadPointerArray(store, 0);
    }

    unsigned int value = 0;
    store->ReadDword(&value);

    if (m_callKind == 3)
    {
        void** slot = instance->GetGlobalSlot(value);
        m_declKind = declKind;
        return (void*)UpdateBinding(declKind, slot, instance);
    }

    m_declKind = declKind;
    return (void*)UpdateBinding(declKind, (void*)value, instance);
}

//----------------------------------------------------------------------------
//  0x6F45B780 - `retn 0xC`.  Updates a symbol's own resolved binding to
//  (kind, data) for `instance`, releasing the old binding through
//  DispatchWithOwnDecl and re-dispatching through DispatchWithArg only
//  when `data` actually changed - unless the symbol's own decl-kind
//  disagrees with `kind` and the instance has not yet reached its own
//  "committed" field-48 threshold, in which case nothing is touched and
//  this returns false outright.
//----------------------------------------------------------------------------
int JassSymbolNode::UpdateBinding(int kind, void* data, JassInstance* instance)
{
    if (m_declKind != kind)
    {
        if (instance->GetField48() >= 1)
            return 0;
    }

    int changed = (m_declData != data);
    if (changed)
        DispatchWithOwnDecl(instance);

    m_callKind = m_declKind;
    m_declData = data;

    if (changed)
        DispatchWithArg(instance, data);

    return changed;
}

//----------------------------------------------------------------------------
//  0x6F4484F0 - `__fastcall(ecx=arg, edx=instance)`, `retn 0`.  Tail-calls
//  JassInstance::Run(arg) for its side effect and hands back `arg`
//  itself, discarding Run's own return value - reached right after
//  JassThreadRegisterInstance in the "spawn a child instance" closure
//  (0x6F455250).
//----------------------------------------------------------------------------
void* __fastcall JassInvokeInstanceRun(void* arg, JassInstance* instance)
{
    instance->Run(arg);
    return arg;
}
