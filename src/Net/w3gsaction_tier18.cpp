//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67FD30. Same shape as
//  w3gsaction_tier17.cpp's own 0x6F67FC60 sibling right up through the
//  found-candidate gate (+0x240/+0x244), reusing that file's own
//  FindByKey_6F67EA80 and Validate_6F679880 declarations by forward
//  declaration (both already naked redirects there - a second naked body
//  under the identical decorated name would collide with tier17's own at
//  link time) - but this one's success path resets the candidate's own
//  +0x240/+0x244/+0x248 fields directly instead of forwarding the
//  validator's own return value onward, then fires a different
//  not-in-scope trigger (Notify_6F679630) carrying the same
//  hidden-ecx-argument shape tier17.cpp's own Commit_6F679680 documents.
//============================================================================

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

struct GameRecord11
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
};

//  0x6F666C40 - already reconstructed for real (not a redirect) in
//  w3gsaction_parsers.cpp: ParseByteByteDwordAction_6F666C40 parses a
//  byte, a byte and a dword into its own out-buffer.
int __fastcall ParseByteByteDwordAction_6F666C40(void*, unsigned int, unsigned char*);

//  0x6F67EA80 - same callee w3gsaction_tier17.cpp already declares;
//  reused by forward declaration rather than re-defined here.
void* __fastcall FindByKey_6F67EA80(void*, unsigned int);

//  0x6F679880 - same callee w3gsaction_tier17.cpp already declares;
//  reused by forward declaration rather than re-defined here.
int __fastcall Validate_6F679880(void*, void*, unsigned char);

//  0x6F679630 - batch-J-w3gsaction-tiers found this address had TWO C++
//  names, both naked pass-through thunks: this file's own `Notify_6F679630`
//  and playertable_installhandler.cpp's separate `sub_6F679630` (reached
//  from PlayerTable_Method67E9E0's own per-node reset loop) - the exact
//  "one shipped address, two C++ names" defect thunk_inventory's own notes
//  warn about. Reconstructed for real here as the single canonical
//  definition; playertable_installhandler.cpp now forward-declares and
//  calls this same name instead of keeping a second naked body.
//
//  fastcall(this=record/ecx, candidate/edx), plain `retn`. Only when
//  candidate->+0x240 is nonzero, record->+0x148 is non-null, its own
//  +0x3C ("target") is non-null, and bit 0x100000 of record->+0x148->+0x80
//  is clear: builds a 12-byte {candidate->+8 (dword), candidate->+0x240
//  (byte), candidate->+0x23C (dword)} local packet and forwards it to the
//  not-in-scope sub_6F678A70 with `target` (not `record`) still sitting in
//  ecx from the gate check above - the shipped call site never reloads it,
//  the same hidden-register-argument shape this file's own header already
//  flags for Trigger_6F67BC20/Commit_6F679680. sub_6F678A70 carries a real
//  `__except_handler4`-shaped SEH frame this
//  build's fixed /GS- /EHs-c- cannot reproduce (confirmed off its own
//  dump: `push -1` / `push offset SEH_6F678A70` / `mov eax, fs:[0]`
//  prologue, docs/msvc-vc8-idioms.md's own documented shape) - kept a
//  thunk. `__fastcall(ecx, edx)`, `retn 0`, off the call site's own
//  register-only argument passing.
extern "C" __declspec(naked) void __fastcall sub_6F678A70(void*, void*)
{
    __asm { mov eax, 0x6F678A70 }
    __asm { jmp eax }
}

//  `extern "C"` - every other naked thunk in this file (sub_6F678A70
//  above) and w3gsaction_tier19.cpp/w3gsaction_tier21.cpp's own forward
//  declarations all agree on this; a plain-C++-linkage re-declaration or
//  definition mangles to a different symbol and leaves the .mix with an
//  unresolved external (link_check.py's own UNEXPECTED bucket catches
//  exactly this - see batch-J's own commit 20d29ab, which fixed this
//  question the first time).
extern "C" __declspec(naked) void __fastcall Notify_6F679630(void* record, void* candidate)
{
    __asm
    {
        sub     esp, 0Ch
        cmp     dword ptr [edx+240h], 0
        jz      short skip
        mov     eax, [ecx+148h]
        test    eax, eax
        jz      short skip
        mov     ecx, [eax+3Ch]
        test    ecx, ecx
        jz      short skip
        test    dword ptr [eax+80h], 100000h
        jnz     short skip
        mov     eax, [edx+8]
        mov     [esp], eax
        mov     al, [edx+240h]
        mov     edx, [edx+23Ch]
        mov     [esp+5], edx
        lea     edx, [esp]
        mov     [esp+4], al
        call    sub_6F678A70
    skip:
        add     esp, 0Ch
        retn
    }
}

//  0x6F67FD30 - case (see w3gsactionhandler2.cpp). Same
//  found-implies-2 return-value shape as w3gsaction_tier17.cpp's own
//  0x6F67FC60: the shipped body sets its own return value to 2 the
//  instant a record is found - BEFORE the second search even runs -
//  and only resets it to 0 when Validate_6F679880 itself returns 0, so
//  a found-but-mismatched candidate (or one FindByKey_6F67EA80 never
//  finds at all) still returns 2. On the one path where
//  Validate_6F679880 returns nonzero, additionally resets the
//  candidate's own +0x240 to 1, +0x244 to 0, +0x248 to 0 and fires
//  Notify_6F679630(record, candidate).
int __stdcall W3GSAction2_6F67FD30(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[8];

    if (!ParseByteByteDwordAction_6F666C40((void*)arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord11* rec = (GameRecord11*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord11*)rec->m_nextRecord;

        if (rec)
        {
            unsigned int searchKey = *(unsigned int*)(buf + 2);
            status = 2;

            void* candidate = FindByKey_6F67EA80(rec, searchKey);
            if (candidate &&
                *(unsigned int*)((char*)candidate + 0x240) == 2 &&
                *(unsigned char*)((char*)candidate + 0x244) == (unsigned char)arg8)
            {
                if (Validate_6F679880(rec, self, (unsigned char)arg8))
                {
                    *(unsigned int*)((char*)candidate + 0x240) = 1;
                    *(unsigned char*)((char*)candidate + 0x244) = 0;
                    *(unsigned int*)((char*)candidate + 0x248) = 0;
                    Notify_6F679630(rec, candidate);
                }
                else
                {
                    status = 0;
                }
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}
