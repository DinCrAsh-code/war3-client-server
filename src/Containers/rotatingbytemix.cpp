//============================================================================
//  0x6F280B90 - `retn 0`, `__fastcall(value in ecx, &accumulator in edx)`.
//  Folds one dword's four bytes into a running hash by the same
//  `(b*0x7A2D) ^ b` byte mix CNetData_DispatchOneQueuedRecord's own inline
//  type-0x1E/0x1F "turn" hashing (Net/pausecallchainthunks.cpp) duplicates
//  literally rather than calling out to: `*acc += mix(topByte)`, then fold
//  in the other three bytes (low, then bits8-15, then bits16-23) on top of
//  that running total. Reached from CNetData_DispatchOneQueuedRecord's own
//  type-0x17/0x21/0x2F special cases, once per wire field read through
//  RecordCtor_6F6515B0/NetMsgReadBytePair_6F6516F0/NetMsgReadDwordPair_
//  6F651790 (netevent_replaystream_records.cpp / netmsgio_651.cpp) - see
//  that file's own header comment for the whole shape.
//============================================================================

static inline unsigned int MixByte_7A2D(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

void __fastcall RotatingByteMixAccumulate_6F280B90(unsigned int value, unsigned int* acc)
{
    unsigned int topByte = (value >> 16) >> 8;          // bits 24-31
    *acc += MixByte_7A2D(topByte);

    unsigned int running = *acc;
    unsigned int loByte = (unsigned char)(value >> 16);  // bits 16-23
    running = MixByte_7A2D(loByte) + running;

    unsigned int b1 = (unsigned char)(value >> 8);        // bits 8-15
    unsigned int b0 = (unsigned char)value;               // bits 0-7
    unsigned int mixed = MixByte_7A2D(b1) + (MixByte_7A2D(b0) + running);

    *acc = mixed;
}
