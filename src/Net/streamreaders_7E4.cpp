//============================================================================
//  0x6F7E46xx-0x6F7E48xx - one small byte-stream cursor's read primitives.
//  The object holds a byte offset at +0x08 and a base pointer at +0x10; each
//  reader loads base[offset], advances the offset by the size it read, and
//  returns the value.  The 0x6F7E47B0/0x6F7E47C0/0x6F7E47F0 trio and the
//  0x6F7E4670/0x6F7E4680 pair are byte for byte identical to each other:
//  this image was linked without /OPT:ICF (agentdefaults.cpp), so identical
//  bodies at different addresses stay separate functions.
//
//  Free __fastcall taking the cursor explicitly, for the same reason as
//  trivialaccessors_*.cpp - `this` in ecx, no stack argument, and no class
//  identity established for the cursor itself.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F7E4670 / 0x6F7E4680 - read one byte and advance by 1.  The shipped
//  code writes only `al` and leaves the rest of eax as it found it, which is
//  what a byte-sized return value looks like.
//----------------------------------------------------------------------------
unsigned char __fastcall StreamReadByte_6F7E4670(void* self)
{
    char* p = (char*)self;
    int offset = *(int*)(p + 8);
    const unsigned char* base = *(const unsigned char**)(p + 0x10);
    unsigned char value = base[offset];
    *(int*)(p + 8) = offset + 1;
    return value;
}

unsigned char __fastcall StreamReadByte_6F7E4680(void* self)
{
    char* p = (char*)self;
    int offset = *(int*)(p + 8);
    const unsigned char* base = *(const unsigned char**)(p + 0x10);
    unsigned char value = base[offset];
    *(int*)(p + 8) = offset + 1;
    return value;
}

//----------------------------------------------------------------------------
//  0x6F7E47B0 / 0x6F7E47C0 / 0x6F7E47F0 - read one dword and advance by 4.
//----------------------------------------------------------------------------
int __fastcall StreamReadDword_6F7E47B0(void* self)
{
    char* p = (char*)self;
    int offset = *(int*)(p + 8);
    const char* base = *(const char**)(p + 0x10);
    int value = *(const int*)(base + offset);
    *(int*)(p + 8) = offset + 4;
    return value;
}

int __fastcall StreamReadDword_6F7E47C0(void* self)
{
    char* p = (char*)self;
    int offset = *(int*)(p + 8);
    const char* base = *(const char**)(p + 0x10);
    int value = *(const int*)(base + offset);
    *(int*)(p + 8) = offset + 4;
    return value;
}

int __fastcall StreamReadDword_6F7E47F0(void* self)
{
    char* p = (char*)self;
    int offset = *(int*)(p + 8);
    const char* base = *(const char**)(p + 0x10);
    int value = *(const int*)(base + offset);
    *(int*)(p + 8) = offset + 4;
    return value;
}

//----------------------------------------------------------------------------
//  0x6F7E4800 - read one float and advance by 4.  The push/fstp/fld/pop
//  around the return is MSVC's ordinary float-return spill, not part of the
//  read itself.
//----------------------------------------------------------------------------
float __fastcall StreamReadFloat_6F7E4800(void* self)
{
    char* p = (char*)self;
    int offset = *(int*)(p + 8);
    const char* base = *(const char**)(p + 0x10);
    float value = *(const float*)(base + offset);
    *(int*)(p + 8) = offset + 4;
    return value;
}
