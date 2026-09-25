//============================================================================
//  0x6F4E9A80 - allocate, construct and register a CTokenizer.
//
//  Its own translation unit: the three lazy getters in modeltokens.cpp call
//  it for real.
//
//  Storm's allocator is asked for 0x34 bytes tagged "HTOKENIZER" with line
//  -2 - the handle-type tag standing in for __FILE__, the same convention
//  storm.h already documents - and the delimiter string is duplicated
//  through Storm ordinal 507 with this class's real source file and line
//  (".\\Tokenizer.cpp", 63) instead.
//
//  The function ends in a tail `jmp` to TRefCnt::AddRef, which is why the
//  refcount bump is spelled as this function's return expression.  The
//  shipped tail also loads edx with the "HTOKENIZER" tag, and that is
//  reproduced now: AddRef is one __fastcall member taking the tag as its
//  single argument, so `this` stays in ecx and the tag lands in edx with no
//  second declaration and nothing for the linker to disagree about.  This
//  comment used to say the instruction was unreachable, and it was - but
//  only because refcnt.h declared AddRef with no parameter, not because the
//  ABI forbade it.
//============================================================================
#include "tokenizer.h"
#include "storm.h"

//  0x6F4E9A50 - the constructor proper.  Reconstructed for real now, in its
//  own translation unit (Storm/tokenizerctor.cpp - see that file for why it
//  is not here): a same-TU callee this small is inlined on sight at /Ob2,
//  which would erase the shipped code's real out-of-line `call` this
//  function makes to it.
CTokenizer* __fastcall ConstructTokenizer(void* raw);

//  Storm ordinal 507 - duplicate a string into Storm-owned memory, with the
//  caller's own file and line for the allocation log.  Declared as ordinary
//  C++, not extern "C": that is what makes MASM write the `; Storm_507`
//  readable-name comment build_mix.py binds the import through.
char* __stdcall Storm_507(const char* text, const char* logfilename,
                          int logline);

CTokenizer* __fastcall CreateTokenizer(const char* delimiters)
{
    //  One combined test, not two early returns: the shipped code has a
    //  single failure exit that both checks branch to, and two separate
    //  `return 0;` statements give the first one its own shorter epilogue
    //  before esi is even live.
    void* raw = SMemAlloc(0x34, "HTOKENIZER", -2, 0);
    CTokenizer* self;
    if (raw == 0 || (self = ConstructTokenizer(raw)) == 0)
        return 0;

    self->m_pDelimiters = Storm_507(delimiters, ".\\Tokenizer.cpp", 63);
    return (CTokenizer*)self->AddRef("HTOKENIZER");
}
