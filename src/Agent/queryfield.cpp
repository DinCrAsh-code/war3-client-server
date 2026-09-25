//============================================================================
//  0x6F4C8820 - forward a lookup through one object's own virtual method
//  (vtable slot 5, `+0x14`) with two tag pointers pulled out of `self` and
//  `other`, then read back an int through a result checked for a leading
//  'S' byte first.  What the virtual call and the 'S'-tagged record are is
//  outside this call tree; only the shape this function itself reads and
//  writes is named.
//============================================================================

class ILookupTarget
{
public:
    //  Five unnamed slots ahead of the one this call tree reaches - nothing
    //  says what they are, only that Query is the sixth (vtable+0x14).
    virtual void* m_reservedSlot0() = 0;
    virtual void* m_reservedSlot1() = 0;
    virtual void* m_reservedSlot2() = 0;
    virtual void* m_reservedSlot3() = 0;
    virtual void* m_reservedSlot4() = 0;
    virtual void* Query(void* otherTag, void* selfTag) = 0;   // vtable+0x14
};

struct SHasLookupTarget
{
    char              m_reserved00[0x20];
    ILookupTarget*    m_target;   // +0x20
    void*             m_tag;      // +0x24
};

int __fastcall QueryTaggedField(SHasLookupTarget* self, SHasLookupTarget* other)
{
    void* result = self->m_target->Query(other->m_tag, self->m_tag);
    if (result != 0 && *(char*)result == 'S')
        return *(int*)((char*)result + 4);
    return 0;
}
