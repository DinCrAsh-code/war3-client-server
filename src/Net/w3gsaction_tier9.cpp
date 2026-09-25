//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F66DFE0, sharing gameaction0x26_handler.cpp's
//  own ActionQueueTable/ConditionVariable infrastructure. Reuses its own
//  `self`-pointer stack slot as scratch storage for the parsed field (the
//  same "self is already cached in esi at entry, so arg_0's own slot is
//  free to reuse" idiom every ParseXAction-family caller in this batch
//  shares - see w3gsaction_6f667c10.cpp's own header for the address
//  version of the same trick), then clamps that parsed value into
//  [1, 0x10] and again against the record's own +0xC4 bound, stores the
//  clamped result into the record's own +0xF4, and - only if that store
//  actually changed the value - dispatches through an indirect vtable
//  call on a per-record sub-object (+0x10) whose real class this call
//  tree does not establish.
//============================================================================

struct ConditionVariable
{
    void SignalOne(unsigned int value);   // sub_6F6D8950
};
extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20

struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

int __fastcall ParseDwordAction_6F6661F0(void* src, unsigned int declaredLength, unsigned int* out);

//  The queue record, this time carrying the fields this handler reads/
//  writes: +0xC4 (an upper bound), +0xD4 (the usual masked session-key
//  compare field), +0xD8/+0xE0 (two more values forwarded verbatim into
//  the trigger dispatch), +0xE4 (m_state), +0xF4 (the clamped value this
//  handler itself owns) and +0x10 (a sub-object pointer whose own vtable
//  slot 0x48/4=18 is called).
struct ActionQueueRecord5
{
    char m_reserved00[0x10];
    void* m_field10;            // +0x10 - a sub-object with its own vtable
    char m_reserved14[0xC4 - 0x14];
    unsigned int m_field0xC4;   // +0xC4
    char m_reservedC8[0xD4 - 0xC8];
    unsigned int m_field0xD4;   // +0xD4
    unsigned int m_field0xD8;   // +0xD8
    char m_reservedDC[0xE0 - 0xDC];
    unsigned int m_field0xE0;   // +0xE0
    int m_state;                 // +0xE4
    char m_reservedE8[0xF4 - 0xE8];
    unsigned int m_field0xF4;   // +0xF4
};

//  Vtable slot 0x48/4 = index 18 on record->m_field10's own sub-object -
//  a guess written from this call site's own disassembly, the same
//  typedef-from-call-site shape CLAUDE.md's own vtable_dispatch_audit.py
//  section documents. No committed docs/targets/vtables/*.txt entry names
//  this sub-object's real class, so vtable_dispatch_audit.py cannot judge
//  this site either way - it is unconfirmed, not clean.
typedef void (__thiscall *ClampChangedFn)(void* self, unsigned int fieldE0,
                                           void* key, unsigned int fieldD8,
                                           unsigned int clampedValue);

//  0x6F66DFE0 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66DFE0(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned int parsed;
    if (!ParseDwordAction_6F6661F0((void*)arg4, arg8, &parsed))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecord5* record = (ActionQueueRecord5*)
        g_actionQueueTable.FindOrCreateRecord(key, 0, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (record->m_state < 3 && (void*)record->m_field0xD4 == key)
        {
            unsigned int oldValue = record->m_field0xF4;

            unsigned int clamped = parsed;
            if (clamped >= 0x10)
                clamped = 0x10;
            record->m_field0xF4 = clamped;

            if (clamped <= 1)
                clamped = 1;
            record->m_field0xF4 = clamped;

            //  unsigned min(clamped, record->m_field0xC4).
            unsigned int bound = record->m_field0xC4;
            if (clamped < bound)
                bound = clamped;
            record->m_field0xF4 = bound;

            if (bound != oldValue)
            {
                ClampChangedFn fn = (ClampChangedFn)(*(void***)record->m_field10)[0x48 / 4];
                fn(record->m_field10, record->m_field0xE0, key, record->m_field0xD8, bound);
            }

            status = 2;
        }

        if (wasCreated != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
    }
    return status;
}
