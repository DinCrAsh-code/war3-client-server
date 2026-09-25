//============================================================================
//  0x6F537990 and 0x6F5379E0 - the two diagnostic name tables the replay
//  layer prints its own state with.
//
//  The first is a five-case jump table with a default; the second is a
//  two-case chain, which is what `sub ecx,0` / `jz` / `sub ecx,1` / `jz` is
//  - MSVC's decision tree for two consecutive values, not a table.
//
//  The strings are referenced as symbols rather than retyped as literals:
//  IDA prints only the first sixteen characters of the first five
//  ("REPLAY_STATE_STR"...), so a literal here would not be byte-for-byte the
//  shipped one and would put a second copy in the image.  The two the dump
//  does print in full are spelled out.
//============================================================================

extern "C" const char aReplayStateNon[];
extern "C" const char aReplayStateLoa[];
extern "C" const char aReplayStateStr[];
extern "C" const char aReplayStateStr_0[];
extern "C" const char aReplayStateSto[];
extern "C" const char aReplayStateUnk[];

const char* __fastcall ReplayStateName(unsigned int state)
{
    switch (state)
    {
    case 0:  return aReplayStateNon;
    case 1:  return aReplayStateLoa;
    case 2:  return aReplayStateStr;
    case 3:  return aReplayStateStr_0;
    case 4:  return aReplayStateSto;
    default: return aReplayStateUnk;
    }
}

const char* __fastcall RecordStateName(unsigned int state)
{
    switch (state)
    {
    case 0:  return "RECORD_STATE_NONE";
    case 1:  return "RECORD_STATE_RECORDING";
    default: return "RECORD_STATE_UNKNOWN?";
    }
}
