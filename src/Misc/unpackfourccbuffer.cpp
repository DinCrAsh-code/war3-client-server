//============================================================================
//  0x6F6F19C0 - CItem vtable slot 32/104 closure (depth 7).  Unpack a packed
//  four-character code into a five-byte NUL-terminated string, low byte
//  first: `__fastcall`, ecx = the packed value, edx = the destination
//  buffer.  `retn 0` - both arguments are register-passed.
//
//  Not the same address as Game/objectdatatable.h's UnpackFourCCString
//  (0x6F4C3F80) - a second, textually identical unpacker the linker keeps as
//  its own symbol because nothing in this call tree tail-calls between them.
//============================================================================

void __fastcall UnpackFourCCToBuffer(unsigned int packed, char* out)
{
    //  Written high byte first - the shipped store order, not source order
    //  by coincidence: writing out[0] first makes MSVC emit the stores
    //  ascending instead.
    out[3] = (char)(packed >> 24);
    out[2] = (char)(packed >> 16);
    out[1] = (char)(packed >> 8);
    out[0] = (char)packed;
    out[4] = 0;
}
