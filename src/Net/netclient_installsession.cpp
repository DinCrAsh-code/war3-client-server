//============================================================================
//  0x6F67A170 - Net::NetClient vtable slot 4 (networking wave 8,
//  agent-networking-wave3-2026-08-29's claim, continued).
//
//  Shares the record-table-walk shape every other NetClient slot in this
//  directory already establishes (g_netClientRecordTableGuard/
//  g_netClientRecordListHead, MaskedPositive on the list head only, a
//  plain `>0` continue test on each node's own `m_nextRecord` - see
//  netclient_getxy_0x25f0.cpp's own header for why the per-node step is
//  NOT re-masked), but is not a member of the seven-slot notify family:
//  three stack arguments (a count, a source array, an optional string),
//  a real provider-selection fallback (+0x14C first, +0x148 with a
//  0x100000 flag test second), a one-shot "already installed" guard bit
//  (+0x80 bit 0x8 on whichever provider was chosen), and a real local
//  array object (FixedElementArray20Self, netclient_slot1_helpers.cpp)
//  built, filled, and released around the framing call - not just a
//  fixed-header {tag,tag} pair.
//
//  Field offsets on the record (`+0x130` state, `+0x148`/`+0x14C`
//  provider pair) and on the chosen provider (`+0x3C` target, `+0x80`
//  flag dword) match the notify family's own record layout and
//  netclient_recvappend.cpp's own provider-flag reads exactly - reused
//  as raw offset casts, the same convention every sibling file in this
//  directory already uses, rather than merged into one shared struct
//  four independent partial views have not yet been reconciled into.
//
//  Two structural details the disassembly makes explicit and this source
//  reproduces on purpose:
//   - the immediate-return path (record key itself is null) never takes
//     the global guard at all, and never reaches the shared SignalOne
//     exit either - it jumps straight past both, matching the shipped
//     code's own very first branch;
//   - the state check is two separate comparisons (`state < 4` then
//     `state < 7`), not one folded `state < 7` - the shipped code makes
//     both tests even though the second alone is sufficient, so both are
//     reproduced here rather than "simplified" to one.
//============================================================================

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head every
//  other NetClient record-table walk in this directory uses.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

//  netclient_slot1_helpers.cpp
struct FixedElementArray20Self
{
    void ResizeAndCopyFrom(unsigned int newCount, const void* srcData);
    void ReleaseData();
};
struct ByteLengthPrefixedFieldCopier
{
    void* Copy(const unsigned char* src);
};

//  netclient_buildarelay.cpp
struct NetClientBuildARelaySelf
{
    void BuildAndSend(void* passthrough);
};

//  A masked field is only ever a positive object pointer or a poisoned
//  non-positive encoding - the same idiom every sibling file in this
//  directory redeclares locally.
static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  Only the fields this function itself reads; see the header comment
//  above for why these stay raw offset casts rather than a shared struct.
struct NetClientRecordFieldsSlot1
{
    char m_reserved000[0xC];
    unsigned int m_recordKey;  // +0x0C (unused directly here - the walk
                                //  compares against `key` by address, kept
                                //  only so the offset lines up with the
                                //  sibling files' own struct)
    char m_reserved010[0x130 - 0x10];
    int  m_state;               // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;           // +0x148
    void* m_altProvider;        // +0x14C
};

struct NetClientProviderFieldsSlot1
{
    char m_reserved00[0x3C];
    void* m_target;      // +0x3C
    char m_reserved40[0x80 - 0x40];
    unsigned int m_flags80; // +0x80
};

//  A local, stack-resident array-with-string frame: the 12-byte
//  FixedElementArray20 header (+0x00) immediately followed by one byte
//  (+0x0C) the optional-string copy writes through
//  ByteLengthPrefixedFieldCopier - the exact layout the disassembly's own
//  `var_A0`/`var_9C`/`var_98`/`var_94` group (a contiguous 13-byte region)
//  lays out on the stack.
struct Slot1LocalFrame
{
    unsigned int m_requestedCount; // +0x00
    unsigned int m_count;          // +0x04
    void*        m_data;           // +0x08
    unsigned char m_stringBuf;     // +0x0C (first byte of a small inline buffer)
};

struct NetClientInstallSessionSelf
{
    void InstallSession(unsigned int count, const void* srcArray, const unsigned char* optionalString);
};

void NetClientInstallSessionSelf::InstallSession(unsigned int count, const void* srcArray,
                                                   const unsigned char* optionalString)
{
    void* self = this;
    void* key = *(void**)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    //  Same do-while walk shape netclient_getxy_0x25f0.cpp's own header
    //  documents: only the list head is masked; each node's own `next`
    //  step is a plain `> 0` continuation test. The target folds every
    //  failure path - the walk finding nothing, and both state checks -
    //  down to one single `ConditionVariable::SignalOne` call reached by
    //  real `jl`/`jmp` branches. This build's compiler does not: no matter
    //  which of several equivalent C shapes this session tried (a `goto`
    //  to a shared label, a literal repeated exit at every failure point,
    //  this `foundRecord`-gated single `if`), the record-not-found loop
    //  exit keeps its own separate, physical `SignalOne` call rather than
    //  folding into the one below - see the BEHAVIOUR entry in
    //  tools/funcmap.py for the one confirmed real difference this leaves.
    NetClientRecordFieldsSlot1* rec = (NetClientRecordFieldsSlot1*)MaskedPositive(g_netClientRecordListHead);
    bool foundRecord = false;
    if (rec)
    {
        do
        {
            if (*(void**)((char*)rec + 0xC) == key)
            {
                foundRecord = true;
                break;
            }
            rec = (NetClientRecordFieldsSlot1*)*(void**)((char*)rec + 0x8);
        } while ((int)rec > 0);
    }

    //  Two separate comparisons, not one folded `state < 7` - see the
    //  header comment above. All of this - the walk failing, and both
    //  state checks - converges on the one shared exit below, matching
    //  the shipped code's own single `jmp`/`jl`-to-one-place shape rather
    //  than each exit repeating its own SignalOne call.
    if (foundRecord && rec->m_state >= 4 && rec->m_state >= 7)
    {
        NetClientProviderFieldsSlot1* provider = (NetClientProviderFieldsSlot1*)rec->m_altProvider;
        if (!provider)
        {
            NetClientProviderFieldsSlot1* fallback = (NetClientProviderFieldsSlot1*)rec->m_provider;
            if (fallback && (fallback->m_flags80 & 0x100000))
                provider = fallback;
        }
        if (provider && provider->m_target && !(provider->m_flags80 & 8))
        {
            provider->m_flags80 |= 8;

            Slot1LocalFrame frame;
            FixedElementArray20Self* array = (FixedElementArray20Self*)&frame;
            array->ResizeAndCopyFrom(count, srcArray);

            if (optionalString)
                ((ByteLengthPrefixedFieldCopier*)&frame.m_stringBuf)->Copy(optionalString);

            ((NetClientBuildARelaySelf*)provider->m_target)->BuildAndSend(&frame);

            array->ReleaseData();
        }
    }

    g_netClientRecordTableGuard.SignalOne(0);
}
