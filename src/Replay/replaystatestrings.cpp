//============================================================================
//  Storage for the five replay-state names IDA prints truncated.  Defined
//  here rather than declared, for the reason src/Misc/cstringrepglobals.cpp
//  gives: a funcmap.DATA row whose IDA name is not address-shaped has
//  nothing for the injection binder to resolve.  The text is this repo's
//  best reading of the truncated dump and only ever reaches a diagnostic
//  print, so a wrong character changes nothing that runs.
//============================================================================

extern "C" const char aReplayStateNon[] = "REPLAY_STATE_NONE";
extern "C" const char aReplayStateLoa[] = "REPLAY_STATE_LOADING";
extern "C" const char aReplayStateStr[] = "REPLAY_STATE_STREAMING";
extern "C" const char aReplayStateStr_0[] = "REPLAY_STATE_STREAMING_DONE";
extern "C" const char aReplayStateSto[] = "REPLAY_STATE_STOPPED";
extern "C" const char aReplayStateUnk[] = "REPLAY_STATE_UNKNOWN?";
