//============================================================================
//  TRefCnt - see refcnt.h for the class-level notes.
//============================================================================
#include "refcnt.h"

//  Storm.dll ordinal 465 - same declaration shape as src/tls.cpp's, and for
//  the same reason (the pipeline keys off the readable-name comment MASM
//  puts on a mangled EXTRN; a plain `extern "C"` symbol has none and the
//  call would canonicalise to an anonymous SYM on this side only).
void __stdcall SErrSetLastError(unsigned int code);

//  0x6F4C4030 - refcnt_self.cpp, its own translation unit; see that file's
//  header comment for why.

//----------------------------------------------------------------------------
//  0x6F4C40A0
//----------------------------------------------------------------------------
void TRefCnt::Release()
{
    //  Slot 1 by offset, with no argument - see refcnt.h on why this one
    //  call site cannot go through DeleteSelf's declared signature.
    typedef void (__thiscall *Slot1NoArgFn)(TRefCnt* self);

    TRefCnt* self = Self();
    if (--self->m_refcount == 0)
        ((Slot1NoArgFn)(*(void***)self)[1])(self);
}

//----------------------------------------------------------------------------
//  0x6F4C40C0
//----------------------------------------------------------------------------
TRefCnt* __fastcall TRefCnt::AddRef(const char* /*tag*/)
{
    if (this == 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }
    ++m_refcount;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C40E0
//----------------------------------------------------------------------------
TRefCnt* TRefCnt::AddRefOrNull()
{
    if (this == 0)
        return 0;
    TRefCnt* self = Self();
    ++self->m_refcount;
    return self;
}

//  0x6F4C8870 / 0x6F4C8880 - refcnt_release_forward.cpp, their own
//  translation unit; see that file's header comment for why.
