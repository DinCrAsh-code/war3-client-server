//============================================================================
//  CItem-batch-3: TSHashTable<OrderTypeReg,HASHKEY_4CC> teardown, the
//  command layer's own four-character-code registry that
//  CCommandBar::CCommandBar (GameUI/commandbar.cpp) builds through the
//  still-redirected SOrderTypeRegTable::Construct
//  (Containers/hashtablectors.h).  The exact mirror of
//  Containers/commandregtable_dtor.cpp - see that file for the full
//  reasoning behind each address and each redirect.
//
//    0x6F35D080  TSExplicitList<OrderTypeReg>::UnlinkAll()
//    0x6F3675B0  TSHashTable<OrderTypeReg,HASHKEY_4CC>::UnlinkAll(bool)
//    0x6F370380  TSGrowableArray<TSExplicitList<OrderTypeReg>>::~TSGrowableArray()
//    0x6F3727A0  `jmp sub_6F370380`, one instruction, IDA's own second
//                heading for the bucket array destructor - redirect, not a
//                second definition of it.
//    0x6F375D50  TSHashTable<OrderTypeReg,HASHKEY_4CC>::~TSHashTable()
//                itself - self-redirect, same wrong-vtable reason
//                SOrderTypeRegTable::Construct is one (hashtablectors.cpp).
//============================================================================
#include "storm.h"
#include "tslist.inl"

struct OrderTypeReg;
class HASHKEY_4CC;

//  0x6F35D080
template void TSExplicitList<OrderTypeReg>::UnlinkAll();

#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UOrderTypeReg@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

//  0x6F370380
template TSGrowableArray<TSExplicitList<OrderTypeReg> >::~TSGrowableArray();

#include "tshash.inl"

//  0x6F3675B0
template void TSHashTable<OrderTypeReg, HASHKEY_4CC>::UnlinkAll(bool);

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F3727A0
struct SOrderTypeRegBucketArrayDtorAlias
{
    void Dtor();   // 0x6F3727A0, `retn 0`
};
ADDR_THUNK(void SOrderTypeRegBucketArrayDtorAlias::Dtor(), 0x6F370380)

//  0x6F375D50
struct SOrderTypeRegTableDtor
{
    void Dtor();   // 0x6F375D50, `retn 0`
};
ADDR_THUNK(void SOrderTypeRegTableDtor::Dtor(), 0x6F375D50)

#undef ADDR_THUNK
