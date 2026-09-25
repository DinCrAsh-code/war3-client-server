//============================================================================
//  Index-to-element accessors: `this` in ecx, one index on the stack
//  (`retn 4`), and a base pointer read out of a fixed field.  Every one of
//  them is one multiply and one add, so the element size is exactly what the
//  shipped instruction encodes - a `shl` for a power of two, an `imul` for
//  anything else, and the `lea eax,[eax+eax*4]` pairs for x20/x40.
//
//  None of these calls anything; grouped one TU for the batch, as
//  trivialaccessors_*.cpp already does.  The owning classes are not
//  established, so each is an opaque receiver with only the offsets it
//  actually touches asserted.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F519xxx - a family of six on the same object, all indexing the array
//  whose base is the object's own first field.  Six different element sizes
//  is six different arrays, so this is one accessor per array rather than one
//  templated one used six ways.
//----------------------------------------------------------------------------
struct SRecordArrayOwner
{
    void* __thiscall At8_6F519970(int index);
    void* __thiscall At16_6F519980(int index);
    void* __thiscall At40_6F519990(int index);
    void* __thiscall At20_6F5199A0(int index);
    void* __thiscall At52_6F5199B0(int index);
    void* __thiscall At8_6F5199C0(int index);
    void* __thiscall At16_6F5199D0(int index);
    void* __thiscall At16_6F5199E0(int index);
    void* __thiscall At40_6F5199F0(int index);
};

void* __thiscall SRecordArrayOwner::At8_6F519970(int index)
{
    return *(char**)this + index * 8;
}

void* __thiscall SRecordArrayOwner::At16_6F519980(int index)
{
    return *(char**)this + index * 16;
}

void* __thiscall SRecordArrayOwner::At40_6F519990(int index)
{
    return *(char**)this + index * 40;
}

void* __thiscall SRecordArrayOwner::At20_6F5199A0(int index)
{
    return *(char**)this + index * 20;
}

void* __thiscall SRecordArrayOwner::At52_6F5199B0(int index)
{
    return *(char**)this + index * 52;
}

void* __thiscall SRecordArrayOwner::At8_6F5199C0(int index)
{
    return *(char**)this + index * 8;
}

void* __thiscall SRecordArrayOwner::At16_6F5199D0(int index)
{
    return *(char**)this + index * 16;
}

void* __thiscall SRecordArrayOwner::At16_6F5199E0(int index)
{
    return *(char**)this + index * 16;
}

void* __thiscall SRecordArrayOwner::At40_6F5199F0(int index)
{
    return *(char**)this + index * 40;
}

//----------------------------------------------------------------------------
//  0x6F524xxx / 0x6F744xxx - the same idea with the base in a later field, and
//  one whose element size is itself a field rather than a constant.
//----------------------------------------------------------------------------
struct SStridedArrayOwner
{
    void* __thiscall AtByteOffset_6F524CC0(int offset);
    void* __thiscall AtStoredStride_6F524CF0(int index);
    void* __thiscall At76_6F744390(int index);
};

//  0x6F524CC0 - the index is already a byte offset here: no scale at all.
void* __thiscall SStridedArrayOwner::AtByteOffset_6F524CC0(int offset)
{
    return *(char**)((char*)this + 8) + offset;
}

//  0x6F524CF0 - element size out of +0x08, base out of +0x00.
void* __thiscall SStridedArrayOwner::AtStoredStride_6F524CF0(int index)
{
    return *(char**)this + *(int*)((char*)this + 8) * index;
}

void* __thiscall SStridedArrayOwner::At76_6F744390(int index)
{
    return *(char**)((char*)this + 0x0C) + index * 76;
}

//----------------------------------------------------------------------------
//  0x6F6F8xxx / 0x6F6F9xxx - fixed-size records in a table the object points
//  at; three of them hand back a field of the record rather than the record.
//----------------------------------------------------------------------------
struct SRecordTableOwner
{
    char  __thiscall ByteAt325_6F6F8770(int index);
    char  __thiscall ByteAt325_6F6F8A90(int index);
    int   __thiscall Field0At260_6F6F9000(int index);
    int   __thiscall Field4At260_6F6F9020(int index);
    void* __thiscall Field8At260_6F6F9040(int index);
    void* __thiscall Field4At264_6F6F92F0(int index);
    int   __thiscall Field0At264_6F6F9310(int index);
};

//  0x6F6F8770 / 0x6F6F8A90 - 0x145-byte records; only the first byte of each
//  is read back, and only the low byte of eax is written, so the result is a
//  char and not an int.
char __thiscall SRecordTableOwner::ByteAt325_6F6F8770(int index)
{
    return *(*(char**)((char*)this + 0x1F8) + index * 0x145);
}

char __thiscall SRecordTableOwner::ByteAt325_6F6F8A90(int index)
{
    return *(*(char**)((char*)this + 0x208) + index * 0x145);
}

int __thiscall SRecordTableOwner::Field0At260_6F6F9000(int index)
{
    return *(int*)(*(char**)((char*)this + 8) + index * 0x104);
}

int __thiscall SRecordTableOwner::Field4At260_6F6F9020(int index)
{
    return *(int*)(*(char**)((char*)this + 8) + index * 0x104 + 4);
}

void* __thiscall SRecordTableOwner::Field8At260_6F6F9040(int index)
{
    return *(char**)((char*)this + 8) + index * 0x104 + 8;
}

void* __thiscall SRecordTableOwner::Field4At264_6F6F92F0(int index)
{
    return *(char**)((char*)this + 0x40) + index * 0x108 + 4;
}

int __thiscall SRecordTableOwner::Field0At264_6F6F9310(int index)
{
    return *(int*)(*(char**)((char*)this + 0x40) + index * 0x108);
}
