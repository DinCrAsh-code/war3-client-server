//============================================================================
//  0x6F4E9A50 - CTokenizer's constructor proper, `this` in ecx.  Not spelled
//  as a real C++ constructor: the shipped order is refcount, vtable, the
//  embedded hashtable, *then* m_pDelimiters - opposite of declaration order,
//  which a compiler-generated member-init sequence cannot produce regardless
//  of how the initializer list is written (real init order always follows
//  declaration order).  Sets the CTokenizer vtable, zeroes the refcount, and
//  constructs the embedded name->id table through the redirect
//  hashtablectors.h supplies (see there for why that member stays a
//  redirect) before m_pDelimiters.  `this` comes back in eax, the usual
//  MSVC constructor-return convention.
//
//  Its own translation unit, separate from CreateTokenizer
//  (tokenizercreate.cpp): the shipped code has a real out-of-line `call` from
//  one to the other, and at this size /Ob2 inlines a same-TU callee this
//  small on sight - which collapsed CreateTokenizer's own call and cost it
//  three matched instructions the one time this was tried in the same file.
//============================================================================
#include "tokenizer.h"

//: A placeholder single-entry vtable, the same idiom Misc/cstringmanager.cpp
//: uses: this build has no way to make a real C++ constructor stamp
//: ??_7CTokenizer@@6B@ itself here, so the pointer is written by hand
//: instead.  canonlib compares `mov [reg], SYM` regardless of which symbol is
//: really behind it, so this does not cost the instruction match; nothing
//: outside this reconstruction ever reads through it.
void* g_vftableCTokenizer[1] = { 0 };

CTokenizer* __fastcall ConstructTokenizer(void* raw)
{
    CTokenizer* self = (CTokenizer*)raw;
    self->m_refcount = 0;
    *(void**)self = g_vftableCTokenizer;
    self->m_tokens.Construct();
    self->m_pDelimiters = 0;
    return self;
}
