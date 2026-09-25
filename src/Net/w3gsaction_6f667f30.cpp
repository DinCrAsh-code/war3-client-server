//============================================================================
//  0x6F667F30 - Probably_W3GS_ActionHandler's case 59: guard `self+0x40`,
//  parse one word field (ParseWordAction_6F666980, w3gsaction_parsers.cpp),
//  then rate-limit a write into `self+0x48` through the same
//  CheckAndAdvanceInterval (netclient_pendingflush.cpp) gate the network
//  retry logic elsewhere already reconstructs - a fixed 500ms interval, no
//  escalation. `self+0x50`/`self+0x48` are otherwise unidentified per-session
//  fields (a GetTickCount-domain deadline and the word this handler stores).
//
//  The returned status local (`status`) is stored in the exact same stack
//  slot CheckAndAdvanceInterval's own `outLongSilence` out-pointer targets -
//  the shipped body reuses that address rather than allocating a second
//  local, which is harmless here because `allowEscalate` is always 0 for
//  this call (CheckAndAdvanceInterval never writes through that pointer on
//  that path), so `status` is always 2 by the time it is returned.
//============================================================================

int __fastcall ParseWordAction_6F666980(void* src, unsigned int declaredLength, unsigned short* out);
int __fastcall CheckAndAdvanceInterval(int* state, unsigned int intervalMs, int allowEscalate, int* outLongSilence);

int __stdcall W3GSAction_6F667F30(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned short parsed;
    if (!ParseWordAction_6F666980((void*)arg4, arg8, &parsed))
        return 1;

    int status = 2;
    if (CheckAndAdvanceInterval((int*)((char*)self + 0x50), 0x1F4, 0, &status))
        *(unsigned short*)((char*)self + 0x48) = parsed;

    return status;
}
