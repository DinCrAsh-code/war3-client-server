//============================================================================
//  0x6F4DDE20 - report a texture load failure through the CStatus the caller
//  handed down: "Error loading %s file \"%s\": %s\n", the kind word, the
//  file name and Storm's own last-error text, and then clear the error.
//
//  **The shipped build takes all three arguments in registers** - the status
//  in esi, the name in ebx and the kind word in edi.  Nothing writes any of
//  them before the first read and its two call sites load exactly those, so
//  it is the whole-program convention docs/msvc-vc8-idioms.md records under
//  "Arguments in eax and esi"; no MSVC convention reaches ebx or edi, so the
//  status rides in ecx here and each caller pays two moves.
//
//  The report itself goes down CStatus's vtable slot 3 as a `__cdecl`
//  variadic - the shipped call pushes six dwords and the caller does the
//  `add esp, 18h` - which is why it is spelled as a slot dispatch rather
//  than as a member: CStatus declares one virtual here (Destroy, slot 0) and
//  giving it four would change the vtable this build stamps.
//============================================================================
#include "texture.h"
#include "cstatus.h"

//  Storm.dll by ordinal: 463 fills a buffer with the text of the current
//  error and 462 is handed what it answers.
char* __stdcall Storm_463(char* buffer, unsigned int size);
void  __stdcall Storm_462(char* text);
void  __stdcall SErrSetLastError(unsigned int code);

void __fastcall ReportTextureLoadError(CStatus* status, const char* name,
                                       const char* kind)
{
    char text[0x100];
    Storm_462(Storm_463(text, sizeof(text)));

    typedef void (__cdecl *StatusReportFn)(CStatus*, int, const char*, ...);
    ((StatusReportFn)(*(void***)status)[0x0C / 4])(
        status, 3, "Error loading %s file \"%s\": %s\n", kind, name, text);

    SErrSetLastError(0);
}
