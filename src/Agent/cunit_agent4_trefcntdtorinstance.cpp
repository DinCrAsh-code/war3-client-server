//============================================================================
//  0x6F2C8EA0 - another COMDAT instantiation of the trivial "reset the
//  vtable pointer and nothing else" destructor body TRefCnt::~TRefCnt()
//  already is (Agent/refcnt.h, at its own address 0x6F00C070) - this image
//  was linked without /OPT:ICF, so identical bodies at distinct addresses
//  genuinely stay distinct entries (agenttypedslots.h's own note about
//  this build's link options).
//
//  Given its own name and its own tiny empty-destructor class rather than
//  a second definition of TRefCnt::~TRefCnt() itself: the mangled
//  destructor name is already claimed by 0x6F00C070's own translation
//  unit, and a second real definition of that identical mangled symbol
//  from a different TU is exactly the ODR situation link_check.py's own
//  note calls out (a duplicate `LNK2005`, not the `LNK4006` a genuinely
//  COMDAT-foldable duplicate downgrades to at the real link). The vtable
//  word stamped is a plain unresolved `SYM` either way as far as the score
//  is concerned (CLAUDE.md's own note on `mov [reg], SYM` canonicalising
//  the same regardless of which class's vtable it names), so a distinct,
//  otherwise-identical empty class reproduces the same two instructions
//  without colliding with the existing symbol.
//============================================================================

class SAgent4TrivialDtorTag_6F2C8EA0
{
public:
    virtual ~SAgent4TrivialDtorTag_6F2C8EA0() {}
};

//  Forces this TU's own copy of the destructor into a real, addressable
//  COMDAT (an inline virtual destructor never used is otherwise dropped
//  entirely at /O2).
void __fastcall DestroyTrivialDtorTagInstance_6F2C8EA0(
        SAgent4TrivialDtorTag_6F2C8EA0* obj)
{
    //  Qualified so this calls the destructor directly (a plain
    //  `mov [reg], offset vftable` / `retn`) rather than dispatching
    //  through the vtable it exists to reset - the same
    //  `this->TRefCnt::~TRefCnt()` spelling Agent/refcnt_deleteself.cpp
    //  already uses for exactly this reason.
    obj->SAgent4TrivialDtorTag_6F2C8EA0::~SAgent4TrivialDtorTag_6F2C8EA0();
}
