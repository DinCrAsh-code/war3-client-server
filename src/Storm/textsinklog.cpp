//============================================================================
//  0x6F4B73F0 - LogFormatted: the variadic debug text sink.
//
//  Was a `mov eax,<address> / jmp eax` redirect called "string formatting,
//  not widget/item logic".  It is eight instructions and it does no
//  formatting at all: it hands its first two arguments and a pointer to its
//  own variadic tail straight to Storm's ordinal 553, which is the one
//  doing the work.
//
//  `lea eax,[esp+arg_8]` is `va_start` spelled out - the address one dword
//  past the last named parameter - and `&format + 1` is the shortest C++
//  that produces exactly that with no `<cstdarg>` (which /Zl leaves out
//  anyway).
//
//  __cdecl on the way in and nothing on the way out: this function pushes
//  three dwords and does not adjust esp afterwards, so Storm_553 cleans
//  them itself.
//
//  Own translation unit: every DumpTo in textsink.h reaches this with a
//  real call.
//============================================================================
#include "textsink.h"

//  Storm ordinal 553 - an import, not shipped code this repo can reach.
//  __stdcall: three dwords pushed, none cleaned here.
//
//  C++ linkage, *not* `extern "C"`, and that is load-bearing in two places
//  neither `verify.py` nor `link_check.py` can see.  `build_mix.py` builds
//  gen_storm.def by harvesting each `EXTRN` out of the compiler's own /FA
//  listing and matching the readable name MASM writes beside it against
//  `funcmap.FUNCTIONS`; MASM writes that comment only for a *decorated*
//  C++ symbol.  Declared `extern "C"` this emitted a bare
//  `EXTRN _Storm_553@12:PROC` with no comment, so the ordinal never
//  reached the .def and the operator's link died on
//  `unresolved external symbol _Storm_553@12` - with `link_check.py`
//  clean, because that tool links *without* gen_storm.lib and counts every
//  Storm import as expected-unresolved.  The same comment is what
//  `canonlib` keys off to print `call Storm_553` instead of an anonymous
//  `call SYM`, which is the eighth instruction of this function.
//  Every other Storm ordinal in `src/` (574, 590, ...) is declared this
//  way already.
void __stdcall Storm_553(void* sink, const char* format, const void* args);

void __cdecl LogFormatted(void* sink, const char* format, ...)
{
    Storm_553(sink, format, (const void*)(&format + 1));
}
