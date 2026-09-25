//============================================================================
//  CItem-batch-3: TSHashTable<CommandReg,HASHKEY_8CC> teardown, the command
//  layer's own eight-character-code registry that CCommandBar::CCommandBar
//  (GameUI/commandbar.cpp) builds through the still-redirected
//  SCommandRegTable::Construct (Containers/hashtablectors.h).
//
//  Three real addresses, plus one one-instruction alias and one destructor
//  both left as redirects:
//
//    0x6F35D010  TSExplicitList<CommandReg>::UnlinkAll() - the all-nodes
//                list's own drain, reused unchanged by both the table's
//                UnlinkAll(bool) below and by the bucket array's own
//                destructor two lines down.
//    0x6F367470  TSHashTable<CommandReg,HASHKEY_8CC>::UnlinkAll(bool) -
//                Storm/storm.h's declaration, Containers/tshash.inl's body.
//    0x6F3702B0  TSGrowableArray<TSExplicitList<CommandReg>>::~TSGrowableArray()
//                - the bucket array's own release (Containers/tsarray.inl).
//    0x6F372790  a one-instruction `jmp sub_6F3702B0` IDA gives its own
//                heading - left a redirect rather than a second definition
//                of the destructor above (one C++ class has one destructor;
//                MSVC's own second copy of the body is what this address
//                is, not something a second declaration can reproduce).
//    0x6F375A50  TSHashTable<CommandReg,HASHKEY_8CC>::~TSHashTable() itself
//                stays a self-redirect for the same reason
//                SCommandRegTable::Construct does (hashtablectors.cpp): it
//                stamps `??_7?$TSHashTable@...@@6B@` at +0x00, and the
//                vtable it would stamp is this repo's own two-slot one, not
//                the shipped four-slot table `ida_query vtable` reports. A
//                destructor that installs the wrong-length vtable is
//                exactly what tools/check_vtables.py exists to catch; the
//                redirect leaves the shipped vtable in place instead.
//============================================================================
#include "storm.h"
#include "tslist.inl"

//  CommandReg itself is never dereferenced by any of these three walks -
//  only its embedded TSLink and the bucket array's own bookkeeping are -
//  so it stays an incomplete placeholder, same convention as every other
//  TSExplicitList/TSHashTable instantiation this repo has reconstructed
//  without knowing the node's real layout.
struct CommandReg;
class HASHKEY_8CC;

//  0x6F35D010
template void TSExplicitList<CommandReg>::UnlinkAll();

#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UCommandReg@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

//  0x6F3702B0
template TSGrowableArray<TSExplicitList<CommandReg> >::~TSGrowableArray();

#include "tshash.inl"

//  0x6F367470
template void TSHashTable<CommandReg, HASHKEY_8CC>::UnlinkAll(bool);

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F372790 - `jmp sub_6F3702B0` in the shipped image: one instruction,
//  IDA's own second heading for the bucket array destructor above.  Left a
//  redirect (to the same real address, not a self-redirect) rather than a
//  second C++ definition of that destructor - a class has one destructor,
//  and this address is the compiler's own second copy of its body.
struct SCommandRegBucketArrayDtorAlias
{
    void Dtor();   // 0x6F372790, `retn 0`
};
ADDR_THUNK(void SCommandRegBucketArrayDtorAlias::Dtor(), 0x6F3702B0)

//  0x6F375A50 - see the file header: same reasoning as
//  SCommandRegTable::Construct, kept a self-redirect rather than stamping
//  this repo's own (wrong-length) TSHashTable vtable.
struct SCommandRegTableDtor
{
    void Dtor();   // 0x6F375A50, `retn 0`
};
ADDR_THUNK(void SCommandRegTableDtor::Dtor(), 0x6F375A50)

#undef ADDR_THUNK
