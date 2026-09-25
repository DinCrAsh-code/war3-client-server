//============================================================================
//  0x6F7E1B62 - __report_gsfailure, the CRT's /GS cookie-mismatch handler.
//  This is not game source: it is MSVC8's own gs_report.c
//  (vs8/VC/crt/src/gs_report.c), reproduced from the compiler's own CRT
//  sources rather than reverse-engineered - the x86 branch of that file,
//  trimmed of the AMD64/IA64 branches that can never compile for this
//  target, is instruction-for-instruction what 0x6F7E1B62 disassembles to.
//  frame and blocks stack packing regardless of this repo's /O2, exactly
//  as it must here since the shipped function has one.
//============================================================================
#include "crtsec.h"
//  IsDebuggerPresent (below) is gated on _WIN32_WINNT >= 0x0400 in this
//  SDK's WinBase.h; the shipped binary plainly calls it (see the dump), so
//  raise the target Windows version for this translation unit only.
#define _WIN32_WINNT 0x0400
#include <windows.h>

//: The CRT's own security-cookie complement, stored immediately after
//: g_securityCookie (crtsec.h) the same way MSVC8's __security_cookie and
//: __security_cookie_complement are a pair - not otherwise reachable from
//: this call tree, so it exists only for this copy to read.
extern unsigned int g_securityCookieComplement;
unsigned int g_securityCookieComplement;

typedef LONG NTSTATUS;
static const NTSTATUS kStatusStackBufferOverrun = (NTSTATUS)0xC0000409L;

//  dbgint.h's _CRT_DEBUGGER_HOOK on x86 - see vs8/VC/crt/src/dbgint.h.  Not
//  pulled in from the CRT-internal header (this build's INCLUDE path does
//  not carry vs8/VC/crt/src), just the one declaration it resolves to.
//  Declared without its own leading underscore: __cdecl decoration adds
//  exactly one, and the shipped import thunk names the decorated symbol
//  `_crt_debugger_hook` (see asm/.../_crt_debugger_hook, `jmp
//  ds:__imp__crt_debugger_hook`) - one underscore, not the two `__cdecl
//  _crt_debugger_hook` would decorate to.
extern "C" void __cdecl crt_debugger_hook(int);

static const int kCrtDebuggerGsFailure = 1;   // _CRT_DEBUGGER_GSFAILURE

//  Global memory for the fake exception, exactly as gs_report.c keeps it:
//  the routine is about to terminate the process regardless, so there is no
//  reason to risk more stack after a buffer overrun already corrupted some
//  of it.
static EXCEPTION_RECORD         GS_ExceptionRecord;
static CONTEXT                  GS_ContextRecord;
static const EXCEPTION_POINTERS GS_ExceptionPointers =
{
    &GS_ExceptionRecord,
    &GS_ContextRecord
};

static BOOL DebuggerWasPresent;

#pragma optimize("", off)   /* force an EBP frame on x86, no stack packing */

void __report_gsfailure()
{
    volatile unsigned int cookie[2];

    //  On x86, reserve extra, unused stack so as much of the caller's frame
    //  as possible survives when the function that overran its buffer
    //  entered __security_check_cookie with a JMP (SecurityCheckCookie's
    //  own tail jmp) rather than a CALL.
    volatile unsigned long dw[(sizeof(CONTEXT) + sizeof(EXCEPTION_RECORD)) / sizeof(unsigned long)];

    __asm
    {
        mov dword ptr [GS_ContextRecord.Eax], eax
        mov dword ptr [GS_ContextRecord.Ecx], ecx
        mov dword ptr [GS_ContextRecord.Edx], edx
        mov dword ptr [GS_ContextRecord.Ebx], ebx
        mov dword ptr [GS_ContextRecord.Esi], esi
        mov dword ptr [GS_ContextRecord.Edi], edi
        mov word ptr [GS_ContextRecord.SegSs], ss
        mov word ptr [GS_ContextRecord.SegCs], cs
        mov word ptr [GS_ContextRecord.SegDs], ds
        mov word ptr [GS_ContextRecord.SegEs], es
        mov word ptr [GS_ContextRecord.SegFs], fs
        mov word ptr [GS_ContextRecord.SegGs], gs
        pushfd
        pop [GS_ContextRecord.EFlags]

        //  Set the context EBP/EIP/ESP to the values found in the caller to
        //  __security_check_cookie - i.e. the function whose buffer
        //  actually overran.
        mov eax, [ebp]
        mov dword ptr [GS_ContextRecord.Ebp], eax
        mov eax, [ebp+4]
        mov dword ptr [GS_ContextRecord.Eip], eax
        //  One dword past the return-address slot: what the caller's ESP
        //  will read as once that return actually happens.  This is the
        //  published gs_report.c unchanged.  (An earlier session read the
        //  disassembly's `lea eax, [ebp+arg_0]` as `[ebp+4]` and changed
        //  the source to match; that was tools/canonlib.py resolving IDA's
        //  `arg_0` as four bytes past the return address, which is right
        //  for a frame-pointer-less function and four short for this one.
        //  The canonicaliser knows the difference now and the source is
        //  back the way Microsoft shipped it.)
        lea eax, [ebp+8]
        mov dword ptr [GS_ContextRecord.Esp], eax

        //  Make sure the dummy stack space looks referenced.
        mov eax, dword ptr dw
    }

    GS_ContextRecord.ContextFlags = CONTEXT_CONTROL;
    GS_ExceptionRecord.ExceptionAddress = (PVOID)(ULONG_PTR)GS_ContextRecord.Eip;

    GS_ExceptionRecord.ExceptionCode  = kStatusStackBufferOverrun;
    GS_ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

    //  Save the global cookie and its complement locally - an array, to
    //  defeat any stack packing.
    cookie[0] = g_securityCookie;
    cookie[1] = g_securityCookieComplement;

    DebuggerWasPresent = IsDebuggerPresent();
    crt_debugger_hook(kCrtDebuggerGsFailure);

    //  Delete any filter already in place, then hand the fake exception to
    //  Watson.
    SetUnhandledExceptionFilter(0);
    UnhandledExceptionFilter((EXCEPTION_POINTERS*)&GS_ExceptionPointers);

    //  If Watson returns and the debugger was not already attached, give a
    //  newly-attached one a chance to break in before the process dies.
    if (!DebuggerWasPresent)
    {
        crt_debugger_hook(kCrtDebuggerGsFailure);
    }

    TerminateProcess(GetCurrentProcess(), kStatusStackBufferOverrun);
}

#pragma optimize("", on)

