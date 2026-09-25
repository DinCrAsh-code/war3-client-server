//============================================================================
//  0x6F680F40 - Probably_W3GS_ActionHandler_MaybeServerVersion's case 14
//  (raw opcode 0x0E, w3gsactionhandler2.cpp). FOUR stack arguments, not the
//  three every sibling case in this switch takes - w3gsaction_thunks.cpp's
//  own header already documented this from the shipped `retn 10h` and the
//  dispatcher's own four-push call site for this one case; that thunk is
//  now removed in favour of this real reconstruction, keeping the same
//  four-wide declaration.
//
//  DESYNC-RELEVANT SUMMARY (see the user's own note that a live capture
//  showed this opcode host-to-client, echoing back another client's last-
//  reported Tock checksum bytes):
//
//    1. Parse the payload (ParseTockEchoRecord_6F680F40, out of this
//       session's own scope - see its own comment below): a dword
//       (`recordId` - which peer/session this echo is *about*, best guess),
//       a length-prefixed name/tag field (`name`, unused by this function
//       past being forwarded verbatim into the "not enough data" report
//       call below), and a length-prefixed byte array - `checksumBytes`,
//       `checksumCount` bytes long - which per the user's capture is the
//       echoed peer's own recent history of per-Tock checksum bytes.
//    2. Resolve `self`'s own session record the same way every sibling in
//       this dispatcher's batch does (g_netClientRecordTableGuard/
//       g_netClientRecordListHead, keyed by `(*(void**)self)->+0x40` -
//       note the extra dereference relative to a handler like
//       W3GSAction2_6F67FD30: this handler's own `self` is a pointer to the
//       real per-connection object, not that object itself).
//    3. Once found and past a minimum state (`rec->+0x130 >= 7`): compare
//       the echoed array's own first byte against *this* connection's own
//       current checksum byte (`(*(void**)self)->+0xB4`) to get
//       `firstDiffers`.
//    4. If there is enough already-accumulated history
//       (`rec->+0x12C >> 1 <= firstDiffers + checksumCount`, i.e. this
//       isn't the first handful of Tocks), walk `rec`'s own array of peer
//       records (`rec->+0x114/+0x118/+0x11C`, the same offset-indexed
//       intrusive-list shape Validate_6F679880 already reproduces
//       literally rather than naming - w3gsaction_tier17.cpp) and, for
//       every OTHER connected peer whose own current checksum byte
//       (`peer->+0xB4`) never appears anywhere in the just-echoed
//       `checksumBytes` history, call NotifyChecksumMismatch_6F6794B0 -
//       i.e. flag that specific peer as not agreeing with anything this
//       echo just reported.
//    5. If, after all that, there genuinely is a divergence
//       (`firstDiffers` still set and there was more than one byte of
//       history to go on), build a 32-bit bitmask of every checksum byte
//       value seen (this connection's own plus the whole echoed array,
//       via `1 << (byte - 1)`, hashed mod 32 the same way an inlined
//       Kernighan popcount collides two different byte values that share
//       a bit position - not a bug this reconstruction can avoid, the
//       shipped code does the same thing), then walk the peer array AGAIN
//       to pick the single BEST-matching peer (highest overlap between
//       that peer's own `+0x48` mask and the bitmask above, capped at a
//       "rank" derived from the bitmask's own popcount, ties broken by the
//       smaller checksum byte) and hand it to Dispatch_6F6806E0
//       (already reconstructed, w3gsaction_tier22.cpp) together with
//       `self` and the trailing argument - this is the actual
//       "who is the outlier" decision the real desync-kick logic upstream
//       of Dispatch_6F6806E0 presumably acts on.
//    6. If there was NOT enough history to run that search at all
//       (`firstDiffers + checksumCount <= 1`), instead build a short
//       diagnostic report (FormatDivergenceReport_6F680F40, out of
//       scope) tagged 0x23, carrying `recordId`, `name` and the echoed
//       array's own leading length field - likely a log line for "too
//       little history to judge yet" rather than a real divergence
//       verdict.
//    7. Either way, release the lock and free the heap-allocated
//       `checksumBytes` array (SMemFree, tag "e:\..." same truncated
//       IDA string every other SMemFree call in this codebase carries).
//
//  A genuine latent shipped quirk this reconstruction reproduces rather
//  than "fixes": in step 4's search, when `checksumCount` is exactly 0 for
//  the very first peer examined, the shipped code's own register
//  allocation skips loading that peer's own checksum byte at all (the
//  `jz` before the byte load) and still falls into the "not found, notify"
//  call carrying whatever garbage was left in that register - an
//  uninitialised read, not a defect in this source. See
//  `firstPeerScanNotify` below.
//
//  Fields not otherwise named (`+0x58`, `+0x5C` bit 2, `+0x114`/`+0x118`/
//  `+0x11C`'s own indexing scheme, `+0x12C`) are kept as raw offset casts
//  per CLAUDE.md's own rule - Validate_6F679880's own header already
//  documents the same +0x114/+0x11C walk as "a further, un-investigated
//  subsystem, reproduced literally rather than renamed into invented
//  semantics", and that applies here identically.
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

//  Same struct shape as GameRecord11/GameRecord15 (w3gsaction_tier18.cpp,
//  w3gsaction_tier22.cpp) when walked off g_netClientRecordListHead as
//  `rec`, and the same shape again (a different instance) when walked as
//  a `peer` entry off `rec`'s own +0x114/+0x118/+0x11C array - the two
//  roles share a layout the way Validate_6F679880's own `record`/
//  `candidate` pair do.
struct NetChecksumRecord
{
    char m_reserved000[0x08];
    void* m_nextRecord;          // +0x08 - only meaningful walked off g_netClientRecordListHead
    unsigned int m_recordKey;    // +0x0C - only meaningful walked off g_netClientRecordListHead
    char m_reserved010[0x40 - 0x10];
    void* m_sessionKey;          // +0x40 - only meaningful in the `peer` role
    char m_reserved044[0x48 - 0x44];
    unsigned int m_checksumMask; // +0x48 - only meaningful in the `peer` role
    char m_reserved04C[0x58 - 0x4C];
    void* m_field58;             // +0x58 - only meaningful in the `rec` role: forwarded to
                                   //  FormatDivergenceReport_6F680F40 and Dispatch_6F6806E0
    unsigned char m_flags05C;    // +0x5C, bit 2 tested - only meaningful in the `peer` role
    char m_reserved05D[0xB4 - 0x5D];
    unsigned char m_checksum;    // +0xB4 - only meaningful in the `peer` role
    char m_reserved0B5[0x114 - 0xB5];
    char* m_peerArrayBase;       // +0x114 - only meaningful in the `rec` role
    char m_reserved118[0x11C - 0x118];
    int m_peerListHead;          // +0x11C - only meaningful in the `rec` role
    char m_reserved120[0x12C - 0x120];
    int m_checksumTotal;         // +0x12C - only meaningful in the `rec` role
    int m_state;                 // +0x130 - only meaningful in the `rec` role
};

//  The offset-indexed walk Validate_6F679880 (w3gsaction_tier17.cpp) also
//  reproduces literally: `node` is not a raw pointer but an index added to
//  `rec`'s own +0x114 base, and `node == 0` denotes the embedded sentinel
//  "head" living at rec+0x118 (whose own "+4 next" field is rec's own
//  +0x11C - the exact same storage the initial masked-positive read pulls
//  from).
static void* NextPeerNode(NetChecksumRecord* rec, void* node)
{
    if (node != 0)
        return *(void**)(rec->m_peerArrayBase + (int)node + 4);
    return *(void**)((char*)rec + 0x118 + 4);
}

//  0x6F675710 - fastcall(data, declaredLength) + one stack out-pointer,
//  retn 4. Constructs its own local CDataStore over (data, declaredLength)
//  and reads a dword, a length-prefixed field (TurnsSyncFieldWrite,
//  already reconstructed elsewhere - sub_6F651CF0, netevent_dispatch.cpp)
//  and a length-prefixed byte array (through a further, not-in-scope
//  growable-buffer allocator, sub_6F6853A0/sub_6F46C440) into consecutive
//  offsets of the caller's own buffer, then returns whether every declared
//  byte was actually consumed. A real /GS-cookie, out-of-line-SEH-funclet
//  frame this build's fixed /GS- /EHs-c- cannot reproduce regardless, and
//  its own callees are a whole further growable-array subsystem out of
//  this session's scope - kept a thunk rather than pulled in whole.
extern "C" __declspec(naked) int __fastcall ParseTockEchoRecord_6F680F40(void*, unsigned int, void*)
{
    __asm { mov eax, 0x6F675710 }
    __asm { jmp eax }
}

//  0x6F6794B0 - fastcall(rec, checksumByte), plain `retn`. Out of this
//  session's scope: a small "notify once" gate on rec->+0x14C/+0x3C/+0x80
//  bit 0x200000, forwarding to a further not-in-scope callee
//  (sub_6F678370) when it fires.
extern "C" __declspec(naked) void __fastcall NotifyChecksumMismatch_6F6794B0(void*, unsigned char)
{
    __asm { mov eax, 0x6F6794B0 }
    __asm { jmp eax }
}

//  0x6F65C080 - thiscall(this=rec->+0x58 sub-object) + 4 stack args, retn
//  10h. Builds a short local report, memcpy's the name field into it,
//  tags it 0x23 and forwards to a further not-in-scope formatter
//  (sub_6F659D40) - a diagnostic/log record, not anything this function's
//  own return value depends on (its own return value is never read at the
//  call site). Out of this session's scope.
struct RecordSubObject58
{
    void FormatDivergenceReport_6F680F40(int recordId, const void* name, unsigned int nameLenField, void* out);
};
__declspec(naked) void RecordSubObject58::FormatDivergenceReport_6F680F40(int, const void*, unsigned int, void*)
{
    __asm { mov eax, 0x6F65C080 }
    __asm { jmp eax }
}

//  Already reconstructed:
int __fastcall PopCount_6F664B90(unsigned int value);                                        // src/Misc/trivialpredicates.cpp
int __fastcall Dispatch_6F6806E0(void* record, void* subobj58, void* selfPtr,
                                  unsigned int argC, unsigned int parsedField);               // w3gsaction_tier22.cpp

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
static const char aE_1[] = "e:\\...";   // aE_1 - IDA's own truncated display; full text not reached by this call tree

#pragma warning(push)
#pragma warning(disable: 4701)   //  nodeChecksum - see the comment at its declaration below
int __stdcall W3GSAction2_6F680F40(void* self, unsigned int Src, unsigned int opcodeDword, unsigned int trailing)
{
    //  `Src`/`opcodeDword` here double as the raw data pointer and the
    //  packet's own declared length - the dispatcher's own comment in
    //  w3gsactionhandler2.cpp already documents this reuse for every
    //  handler in this batch.
    unsigned char parseBuf[0x75];
    parseBuf[4] = 0;                                   //  name[0] = 0 (empty by default)
    *(unsigned int*)(parseBuf + 0x65) = 0;
    *(unsigned int*)(parseBuf + 0x69) = 0;              //  checksumCount = 0
    *(unsigned int*)(parseBuf + 0x6D) = 0;              //  checksumBytes = 0

    int lengthsMatched = ParseTockEchoRecord_6F680F40((void*)Src, opcodeDword, parseBuf);
    if (!lengthsMatched)
    {
        unsigned char* bytes = *(unsigned char**)(parseBuf + 0x6D);
        if (bytes)
            SMemFree(bytes, aE_1, -2, 0);
        return 1;
    }

    void* peer = *(void**)self;
    void* sessionKey = *(void**)((char*)peer + 0x40);
    int status = 0;

    if (sessionKey != 0)
    {
        g_netClientRecordTableGuard.Wait(1);

        NetChecksumRecord* rec = (NetChecksumRecord*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != sessionKey)
            rec = (NetChecksumRecord*)rec->m_nextRecord;

        if (!rec)
        {
            g_netClientRecordTableGuard.SignalOne(1);
        }
        else if (rec->m_state < 7)
        {
            status = 2;
        }
        else
        {
            unsigned int checksumCount = *(unsigned int*)(parseBuf + 0x69);
            unsigned char* checksumBytes = *(unsigned char**)(parseBuf + 0x6D);
            void* subObj58 = rec->m_field58;

            int firstDiffers;
            if (checksumCount == 0)
                firstDiffers = 1;
            else
                firstDiffers = (checksumBytes[0] != *((unsigned char*)peer + 0xB4)) ? 1 : 0;

            //  Enough accumulated history to bother searching for a
            //  divergent peer at all?
            if ((unsigned int)(firstDiffers + checksumCount) >= (unsigned int)(rec->m_checksumTotal) >> 1)
            {
                //  First peer-array walk: flag every OTHER connected peer
                //  whose own current checksum byte matches nothing in the
                //  just-echoed history.
                for (void* node = MaskedPositive(rec->m_peerListHead); node; node = NextPeerNode(rec, node))
                {
                    if (node == peer)
                        continue;

                    //  `nodeChecksum` is deliberately left uninitialised on the
                    //  checksumCount==0 path - the shipped code's own register
                    //  allocation skips the load entirely there (a `jz` ahead
                    //  of `mov cl,[esi+0B4h]`) and still falls into the
                    //  "not found, notify" call carrying whatever was already
                    //  in that register. Reproduced rather than "fixed" - see
                    //  this file's own header comment. (The /W4 C4701 this
                    //  provokes is suppressed for the whole function, below -
                    //  VC8's own flow analysis reports it at function scope,
                    //  not at this block, so a push/pop bracketing just this
                    //  block does not reach it.)
                    unsigned char nodeChecksum;
                    int idx = (int)checksumCount;
                    bool found;
                    if (idx != 0)
                    {
                        nodeChecksum = ((NetChecksumRecord*)node)->m_checksum;
                        found = false;
                        do
                        {
                            idx -= 1;
                            if (checksumBytes[idx] == nodeChecksum)
                            {
                                found = true;
                                break;
                            }
                        } while (idx != 0);
                    }
                    else
                    {
                        found = false;
                    }
                    if (!found)
                        NotifyChecksumMismatch_6F6794B0(rec, nodeChecksum);
                }
            }

            if ((unsigned int)(firstDiffers + checksumCount) <= 1)
            {
                RecordSubObject58* sink = (RecordSubObject58*)subObj58;
                unsigned int recordId = *(unsigned int*)(parseBuf + 0);
                const void* name = parseBuf + 5;
                unsigned int nameLenField = *(unsigned int*)(parseBuf + 4);
                void* reportOut = parseBuf + 0x65;
                sink->FormatDivergenceReport_6F680F40((int)recordId, name, nameLenField, reportOut);
                status = 2;
            }
            else if (!firstDiffers)
            {
                status = 2;
            }
            else
            {
                unsigned char localChecksum = *((unsigned char*)peer + 0xB4);
                unsigned int bitmask = 1u << (unsigned int)(localChecksum - 1);
                for (unsigned int k = checksumCount; k-- > 0; )
                    bitmask |= 1u << (unsigned int)(checksumBytes[k] - 1);

                int diversity = 0;
                for (unsigned int m = bitmask; m; m &= m - 1)
                    diversity += 1;
                int rank = diversity ? diversity - 1 : diversity;

                unsigned int intersection = ((NetChecksumRecord*)peer)->m_checksumMask & bitmask;
                int intCount = 0;
                for (unsigned int m = intersection; m; m &= m - 1)
                    intCount += 1;

                void* best = peer;
                int bestScore = (intCount < rank) ? intCount : rank;

                for (void* node = MaskedPositive(rec->m_peerListHead); node; node = NextPeerNode(rec, node))
                {
                    if (node == peer)
                        continue;
                    if (checksumCount == 0)
                        continue;

                    NetChecksumRecord* peerNode = (NetChecksumRecord*)node;
                    unsigned char nodeChecksum = peerNode->m_checksum;
                    int idx = (int)checksumCount;
                    bool found = false;
                    do
                    {
                        idx -= 1;
                        if (checksumBytes[idx] == nodeChecksum) { found = true; break; }
                    } while (idx != 0);
                    if (!found)
                        continue;
                    if (!(peerNode->m_flags05C & 2))
                        continue;

                    unsigned int intersect2 = peerNode->m_checksumMask & bitmask;
                    int cnt2 = PopCount_6F664B90(intersect2);
                    if (cnt2 > rank)
                        cnt2 = rank;

                    if (cnt2 > bestScore ||
                        (cnt2 == bestScore && nodeChecksum < *((unsigned char*)best + 0xB4)))
                    {
                        best = node;
                        bestScore = cnt2;
                    }
                }

                unsigned int parsedField = *((unsigned char*)best + 0xB4);
                status = Dispatch_6F6806E0(rec, subObj58, self, trailing, parsedField);
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }

    unsigned char* freeBytes = *(unsigned char**)(parseBuf + 0x6D);
    if (freeBytes)
        SMemFree(freeBytes, aE_1, -2, 0);
    return status;
}
#pragma warning(pop)
