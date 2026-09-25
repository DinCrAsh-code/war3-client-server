//============================================================================
//  The JASS native-registration primitive - `.\Jass.cpp`'s own
//  RegisterNative(name, entry, "(sig)R") calls, one per native in the
//  1167-entry table at 0x6F3D4020 (docs/targets/jass-natives-registration-
//  table.md already has the whole table).
//============================================================================
#ifndef JASSREGISTERNATIVE_H
#define JASSREGISTERNATIVE_H

//  The record RegisterNative resolves-or-creates per native, out of
//  JassThreadLocal's own NativeFunc hash table (jasshashtablectors.h).
//  Only the two fields this function writes are named; everything else -
//  including the CJassFunc-shaped 30-slot vtable CLAUDE.md's own session
//  note puts out of scope - is untouched here.
struct NativeFunc
{
    char         m_reserved00[0x20];
    unsigned int m_argCount;    // +0x20 - stack words the native's JASS call site pushes
    char         m_reserved24[0x38 - 0x24];
    unsigned int m_returnType;  // +0x38 - 0=V,3=C,4=I,5=R,6=S,7=H,8=B (see jassregisternative.cpp)
};

//  0x6F4550C0 - GetThreadLocalSlot(kThreadLocalJass) plus JASSVARSCOPE::Declare
//  (0x6F454E30, src/Jass/jassdeclare.cpp) against the running thread's own
//  JassThreadLocal - see jassregisternative.cpp's own note.
void __fastcall ResolveNativeFuncRecord(const char* name, void** outHandle,
                                         void* funcAddr, const char* sig);

//  0x6F455110 - `retn 4`.  ecx = entry point, edx = JASS name, one stack
//  argument = the JASS signature string ("(sig)R").
void __fastcall RegisterNative(void* funcAddr, const char* name, const char* sig);

#endif
