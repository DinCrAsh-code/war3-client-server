//============================================================================
//  0x6F457C50 - `TSHashTable<Symbol,HASHKEY_CONSTSTR>::Construct` (`retn 0`).
//  Kept a redirect for the exact reason jasshashtablectors.h's own file
//  header gives for its own three tables: this shipped class stamps a
//  four-slot `??_7?$TSHashTable@USymbol@@VHASHKEY_CONSTSTR@@@@6B@` vtable,
//  where this repo's own TSHashTable<T,K> template (Storm/storm.h)
//  declares only the two virtuals it can actually justify (the destructor
//  and AllocNode). Constructing one for real would stamp a wrong-length
//  vtable over a four-slot shipped class - invisible to the instruction
//  score, and exactly what tools/check_vtables.py exists to catch. A
//  redirect leaves the shipped constructor, and therefore the shipped
//  vtable, in place. Spelled as a `Construct` member rather than a real
//  C++ constructor because MSVC will not take `__declspec(naked)` on one
//  (jasshashtablectors.cpp's own precedent).
//============================================================================
struct Symbol;

struct SJassSymbolHashTable
{
    //  0x6F457C50 - `retn 0`.  Redirect; see file header.
    SJassSymbolHashTable* Construct();
    char m_storage[0x28];
};

#define JASS_SYMBOL_HASHTABLE_CTOR_THUNK(addr) \
    {                                          \
        __asm { mov eax, addr }               \
        __asm { jmp eax }                      \
    }

//  0x6F457C50
__declspec(naked) SJassSymbolHashTable* SJassSymbolHashTable::Construct()
    JASS_SYMBOL_HASHTABLE_CTOR_THUNK(06F457C50h)
