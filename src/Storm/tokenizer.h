//============================================================================
//  CTokenizer - Storm's string->id map, registered under the handle-type tag
//  "HTOKENIZER" and built from ".\\Tokenizer.cpp".
//
//  The game keeps three of them as lazily-created singletons (modeltokens.h):
//  one for bone names, one for model attachment points and one for animation
//  names.  Each maps a name to the id the model format actually stores, so
//  everything downstream compares integers rather than strings.
//
//  Derives from TRefCnt: the creation path ends in a tail `jmp` to
//  TRefCnt::AddRef (0x6F4C40C0, refcnt.cpp), which only lines up if the
//  refcount is at +0x04 behind a vtable at +0x00 - and +0x08, the first
//  field this class owns, is where the duplicated delimiter string lands.
//============================================================================
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "refcnt.h"
#include "hashtablectors.h"

class CTokenizer : public TRefCnt
{
public:
    char* m_pDelimiters;         // +0x08
    SNativeTokenTable m_tokens;  // +0x0C, size 0x28 - see hashtablectors.h
};

//  0x6F4E9A80 - tokenizercreate.cpp.  The delimiter string arrives in ecx.
CTokenizer* __fastcall CreateTokenizer(const char* delimiters);

//  0x6F4E9CA0 - add one name->id mapping.  Tokenizer in ecx, name in edx,
//  id on the stack.  Below this dump's depth-3 cut; thunked - see
//  hashtablectors.h for why (its insert path needs an AllocNode virtual
//  body nothing here supplies).
void __fastcall AddToken(CTokenizer* self, const char* name, int id);

#endif
