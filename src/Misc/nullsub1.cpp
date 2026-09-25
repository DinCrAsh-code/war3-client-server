//============================================================================
//  0x6F20BC60 (`nullsub_1`) - a plain `retn`, no stack cleanup, called from
//  six sites in this call tree with a printf-style format string plus a
//  varying number of value arguments (a debug-log hook that never actually
//  logs anything in this build). Its own TU for the same reason as
//  nullstub.cpp's 0x6F001700: nothing else in this call tree gives it a
//  name, and it must not be inlined into any one caller.
//============================================================================
void __cdecl nullsub_1(const char*, ...) {}
