//============================================================================
//  0x6F262A70 - SAliasableRecord::ResolveTypeId: resolve a record's
//  effective type id - its own +0x0C field when it is set, otherwise its
//  "alias" string field packed into a FourCC (PackFourCCString,
//  Net/packfourcc.cpp).  `this` only, `retn 0`.
//
//  Own translation unit: 0x6F262xxx is a different shipped module from
//  0x6F052xxx (ability_slots.cpp, Method_6F052B10 - worklist slot 32 -
//  its only caller here), and this function is small enough that /Ob2
//  folds it straight into its caller when they share a TU, which is not
//  what the shipped code's own real `call` does.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F2628E0 - CAbility depth<=5 closure, batch 2: reconstructed for real
//  in Item/abilitygenericfieldrecord.cpp (SGenericFieldRecord's own file,
//  alongside its two siblings 0x6F2625F0/0x6F262760).  Declared locally
//  here the same way that file's own callers already redeclare a shared
//  type rather than pull in a header for one member: a second declaration
//  with the identical signature is the same symbol as far as the linker
//  is concerned.
//----------------------------------------------------------------------------
class SGenericFieldRecord
{
public:
    void* __thiscall QueryStringField(const char* fieldName);
};

//  0x6F4C3F50 - Net/packfourcc.cpp; declared locally the same way
//  item_place.cpp already does, since that file does not expose it
//  through a shared header.
unsigned int __fastcall PackFourCCString(const char* text);

class SAliasableRecord
{
public:
    unsigned int __thiscall ResolveTypeId();
};

unsigned int SAliasableRecord::ResolveTypeId()
{
    if (*(unsigned int*)((char*)this + 0x0C) == 0)
    {
        void* alias = ((SGenericFieldRecord*)this)->QueryStringField("alias");
        return PackFourCCString((const char*)alias);
    }

    return *(unsigned int*)this;
}
