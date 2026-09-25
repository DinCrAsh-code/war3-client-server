//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F66E0E0. Carries a plain `/GS` stack-
//  cookie frame this repo's fixed `/GS-` build can never emit (see
//  w3gsaction_tier8.cpp's own header for the class), and is the most
//  involved member of the ActionQueueTable-backed family this batch
//  reaches: on a field0xD4-masked-matches-self gate, copies a
//  length-prefixed block twice (record+0x100, then a local payload copied
//  FROM record+0x100) and re-reads the SAME +0xD4 field a second time -
//  this time treated not as a single session key but as the head of a
//  masked, array-indexed linked list of session nodes (the shipped body's
//  own `sub_6F668130` iterator adds each node's own pointer VALUE as a
//  raw byte offset into a base array read off record+0xCC, not a
//  conventional `node->next` walk) - notifying every node except self
//  with the freshly-copied payload via the not-in-scope `sub_6F66A040`.
//
//  Both local buffers this handler builds (the field-parse destination and
//  the broadcast payload) are sized generously from the frame's own total
//  reservation rather than from a byte-exact source this call tree does
//  not establish a concrete type for - correct in shape (a raw,
//  length-prefixed byte block each), not proven byte-exact to whatever the
//  original local's own declared size was.
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

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  0x6F6662A0 - retn 4 (fastcall ecx/edx + one stack out-pointer). Not one
//  of this batch's own worklist addresses - out of scope.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F6662A0(unsigned int, unsigned int, void*)
{
    __asm { mov eax, 0x6F6662A0 }
    __asm { jmp eax }
}

//  0x6F3A1830 - fastcall(dest, stack src): a length-prefixed block copy.
//  Reads a leading `unsigned short` count off `src`, writes it into
//  `dest`, then `memcpy`s that many bytes from `src+2` to `dest+2`.
//  Reconstructed for real (small, self-contained) rather than redirected.
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

void* __fastcall CopyLengthPrefixedBlock(void* dest, const void* src)
{
    unsigned short len = *(const unsigned short*)src;
    *(unsigned short*)dest = len;
    memcpy((char*)dest + 2, (const char*)src + 2, len);
    return dest;
}

//  0x6F668130 - thiscall(this) + one stack argument (a previous node's own
//  pointer value, or null for "start"), retn 4. `this` is the sub-object
//  at record+0xCC in this call tree. Its own body (null input returns
//  `this+8`; non-null input adds the input's own pointer VALUE as a raw
//  byte offset onto `this+0` and returns the dword 4 bytes past that - an
//  array-indexed walk, not an ordinary `node->next` chase) is small enough
//  to inline outright, and an earlier draft did exactly that - which
//  costs the shipped body's own explicit `call` instruction the diff
//  shows it keeping (not inlined there, presumably a separate linkage
//  boundary this call tree does not otherwise establish). batch-J-
//  w3gsaction-tiers: reconstructed for real as a naked transcription of
//  its own nine-instruction body instead - this keeps the caller's own
//  explicit `call` (an ordinary function call into a real, scored
//  reconstruction) without gambling on the compiler choosing not to
//  inline a from-scratch C++ body this small.
struct ListWalker
{
    void* Step(void* prev);
};
__declspec(naked) void* ListWalker::Step(void*)
{
    __asm
    {
        mov     edx, [esp+4]
        test    edx, edx
        jz      short from_start
        mov     eax, [ecx]
        add     eax, edx
        mov     eax, [eax+4]
        retn    4
    from_start:
        mov     eax, [ecx+8]
        retn    4
    }
}

//  The queue record's own fields this handler reads are addressed via raw
//  `record + offset` casts rather than a named struct - unlike this
//  batch's other ActionQueueTable-family entries, nothing here needs more
//  than one field read through any single typed pointer, so a struct
//  would add a name without adding a check.

//  0x6F66A040 - out of this batch's own scope (80 instructions). fastcall
//  shape confirmed off this call site's own disassembly: `this` in ecx,
//  the payload pointer in edx, plain `retn`.
extern "C" __declspec(naked) void __fastcall NotifyNode(void*, void*)
{
    __asm { mov eax, 0x6F66A040 }
    __asm { jmp eax }
}

//  0x6F66E0E0 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction_6F66E0E0(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned char parseBuf[0x84];
    *(unsigned int*)parseBuf = 0;
    if (!FieldValidator_6F6662A0(arg4, arg8, parseBuf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    char* record = (char*)g_actionQueueTable.FindOrCreateRecord(key, 0, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        unsigned int* field0xD4 = (unsigned int*)(record + 0xD4);
        int* state = (int*)(record + 0xE4);

        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (*state < 3 && MaskedPositive((int)*field0xD4) == key)
        {
            void* dest0x100 = record + 0x100;
            CopyLengthPrefixedBlock(dest0x100, parseBuf);

            unsigned char payload[0x84];
            *(unsigned int*)payload = 0;
            CopyLengthPrefixedBlock(payload, dest0x100);

            void* node = MaskedPositive((int)*field0xD4);
            if ((int)node > 0)
            {
                ListWalker* walker = (ListWalker*)(record + 0xCC);
                do
                {
                    if (node != key)
                    {
                        void* target = *(void**)((char*)node + 0x3C);
                        NotifyNode(target, payload);
                    }
                    node = walker->Step(node);
                } while ((int)node > 0);
            }

            status = 2;
        }

        if (wasCreated != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(wasCreated != 0));
    }
    return status;
}
