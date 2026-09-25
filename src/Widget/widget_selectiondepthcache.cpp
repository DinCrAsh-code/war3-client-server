//============================================================================
//  0x6F337990/0x6F372660/0x6F372C60/0x6F372F00/0x6F41F6A0/0x6F421E70/
//  0x6F4C8570/0x6F4C8790/0x6F4C9060 - CAbility slot 165's own
//  GameUI-rendering closure (asm/sub_6F02E160_0x6F02E160_calltree_asm.md),
//  cability_depth5_closure_worklist.json batch 3.
//
//  Several of these are members of an unrecovered owner class - a
//  minimal stand-in, the same reasoning ability_slots.cpp's own
//  CAbilityLevelRowRaw note gives - since a free function cannot be
//  declared `__thiscall` in this compiler (only a native member can).
//============================================================================
#include "widget_selectiondepthcache.h"
#include "unitmembership.h"
#include "tshash.inl"

//----------------------------------------------------------------------------
//  0x6F4C8570 - SDepthCacheKey::Hash().  The same rolling string-hash
//  AGILE_TYPE_ID::Hash (Agent/agileid.cpp) has, generalised from one
//  dword's four bytes to two dwords' eight - the `shrd`/`shr` pair
//  shifting the 64-bit (m_field4:m_field0) pair right 8 bits per byte
//  rather than a plain `shr` on one dword.  dword_6F954910 is a second,
//  distinct sixteen-entry table from g_agileHashTable (agiletype.h);
//  Storm keeps one such table per hash function it needs, not one
//  shared table for all of them, so this is not a re-declaration of
//  that symbol.
//----------------------------------------------------------------------------
extern const unsigned int g_depthCacheHashTable[16];     // dword_6F954910

unsigned int SDepthCacheKey::Hash() const
{
    unsigned int seed1 = 0x7FED7FED;
    unsigned int seed2 = 0xEEEEEEEE;

    unsigned int lo = m_field0;
    unsigned int hi = m_field4;
    while ((lo | hi) != 0)
    {
        unsigned int ch = lo & 0xFF;
        seed1 = (seed1 + seed2) ^ (g_depthCacheHashTable[ch >> 4] -
                                   g_depthCacheHashTable[ch & 0x0F]);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
        lo = (lo >> 8) | (hi << 24);
        hi >>= 8;
    }

    return seed1;
}

template CDepthCacheRecord* TSHashTable<CDepthCacheRecord, SDepthCacheKey>::Ptr(
    unsigned int, const SDepthCacheKey&);

void CDepthCacheOwner::UpdateState(unsigned int a0, unsigned int a1, unsigned int flag)
{
    char* field1C = (char*)QueryField1C(a0, a1);
    if (field1C == 0)
        return;

    int state = *(int*)(field1C + 0x5BC);
    if (state == 0)
    {
        *(int*)(field1C + 0x5BC) = (flag != 0) ? 2 : 1;
    }
    else if (state == 2 && flag == 0)
    {
        *(int*)(field1C + 0x5BC) = 1;
    }
}

SUnitMembershipList* CDepthCacheOwner::GetOwnerAt20C()
{
    return m_ownerAt20C;
}

int CDepthCacheOwner::QueryOwnerAt20CField(void* unit)
{
    SUnitMembershipList* owner = GetOwnerAt20C();
    if (owner != 0)
        return owner->Contains(unit);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F337990 - write this record's own two cache slots (index `slot`)
//  from `source`'s own +0xC/+0x10, or -1/-1 when `source` is null.
//  A second minimal stand-in: the receiver here is a plain array, not
//  CDepthCacheOwner - nothing ties the two together in this call tree.
//----------------------------------------------------------------------------
class CDepthCacheSlotArray
{
public:
    int WriteSlot(void* source, unsigned int slot);
    int m_data[1];
};

int CDepthCacheSlotArray::WriteSlot(void* source, unsigned int slot)
{
    int a = (source != 0) ? *(int*)((char*)source + 0xC) : -1;
    m_data[slot + 6] = a;   // +0x18 / 4

    if (source == 0)
    {
        int b = -1;
        m_data[slot + 8] = b;   // +0x20 / 4
        return b;
    }

    int b = *(int*)((char*)source + 0x10);
    m_data[slot + 8] = b;
    return b;
}

//----------------------------------------------------------------------------
//  0x6F4C8790 - resolve edx's own record through ecx's own vtable slot 5
//  (QueryField, `[obj_vtbl+0x14]`), then answer it as an int: 'I' tag
//  reads the answer's own +4 dword directly, 'F' tag truncates the
//  answer's own +4 float toward zero, anything else answers 0.
//----------------------------------------------------------------------------
typedef void* (__thiscall *QueryFieldFn)(void*, int, int);

class STaggedFieldSource
{
public:
    int m_reserved00[8];
    void* m_object;    // 0x20
    int   m_arg;       // 0x24
};

int __fastcall QueryTaggedFieldAsInt(STaggedFieldSource* self, STaggedFieldSource* other)
{
    int a = self->m_arg;
    void* obj = self->m_object;
    int b = other->m_arg;

    void* vtbl = *(void**)obj;
    QueryFieldFn fn = *(QueryFieldFn*)((char*)vtbl + 0x14);
    void* answer = fn(obj, b, a);
    if (answer == 0)
        return 0;

    char tag = *(char*)answer;
    if (tag == 'I')
        return *(int*)((char*)answer + 4);
    if (tag == 'F')
        return (int)(*(float*)((char*)answer + 4));
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C9060 - below this closure's depth (0x6F4C8FF0, whose own IDA
//  signature types this call's third stack slot `float` - a type tag on
//  the callee's own declared parameter, not a claim that `self` is
//  reinterpreted as one here: it is pushed with a plain `push`, the same
//  bytes regardless).  __thiscall(this) + 2 stack args, `retn 4`.
//----------------------------------------------------------------------------
class CQueryTaggedFieldOwner
{
public:
    void QueryTaggedFieldTriple(int a, int b, int c);
};

__declspec(naked) void CQueryTaggedFieldOwner::QueryTaggedFieldTriple(int, int, int)
{
    __asm { mov eax, 0x6F4C8FF0 }
    __asm { jmp eax }
}

void* __fastcall ForwardTaggedFieldTriple(void* self, STaggedFieldSource* other,
                                          STaggedFieldSource* arg)
{
    int edxField = other->m_arg;
    int argField = arg->m_arg;
    CQueryTaggedFieldOwner* obj = (CQueryTaggedFieldOwner*)other->m_object;
    obj->QueryTaggedFieldTriple((int)self, argField, edxField);
    return self;
}
