//============================================================================
//  0x6F75B330 / 0x6F013CC0 - two small, fully self-contained field-reset
//  functions found near JASS_SetItemPosition_callee1's own Storm-gateway
//  frontier (see docs/notes/jass-setitemposition-storm-gates.md).  Neither
//  calls anything and neither is reached by any item-positioning code in
//  this call tree - both are orphaned here the same way
//  docs/targets/CWidget__MoveForward.md's session 6 found
//  sub_6F4D2080/sub_6F4D2150 to be, kept anyway because CLAUDE.md's own
//  brief is to reconstruct what is small and self-contained rather than
//  leave it TODO. The owning class/struct for either is not identified;
//  what is here is exactly what the raw offsets show, nothing more.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F75B330 - zero four dwords at +4/+8/+0xC/+0x10, in that order, then
//  +0 last; returns `this`.
//----------------------------------------------------------------------------
void* __fastcall ZeroFiveDwordFields(void* self)
{
    unsigned int* field = (unsigned int*)self;
    field[1] = 0;
    field[2] = 0;
    field[3] = 0;
    field[4] = 0;
    field[0] = 0;
    return self;
}

//----------------------------------------------------------------------------
//  0x6F013CC0 - reset a 0xB8-byte (46-float) block: zero throughout except
//  four floats at +0x18/+0x2C/+0x40/+0x54 - evenly spaced 0x14 (20) bytes
//  apart - set to 1.0.  Reads as four 20-byte sub-records (a "scale"/
//  "weight" field defaulting to 1.0 followed by four more defaulting to 0)
//  behind a 0x18-byte all-zero header, though nothing in this call tree
//  confirms that reading.
//----------------------------------------------------------------------------
void __fastcall ResetFourWeightedRecords(void* self)
{
    float* f = (float*)self;
    f[0x00 / 4] = 0.0f;
    f[0x04 / 4] = 0.0f;
    f[0x08 / 4] = 0.0f;
    f[0x0C / 4] = 0.0f;
    f[0x10 / 4] = 0.0f;
    f[0x14 / 4] = 0.0f;
    f[0x18 / 4] = 1.0f;
    f[0x2C / 4] = 1.0f;
    f[0x40 / 4] = 1.0f;
    f[0x54 / 4] = 1.0f;
    f[0x1C / 4] = 0.0f;
    f[0x20 / 4] = 0.0f;
    f[0x24 / 4] = 0.0f;
    f[0x28 / 4] = 0.0f;
    f[0x30 / 4] = 0.0f;
    f[0x34 / 4] = 0.0f;
    f[0x38 / 4] = 0.0f;
    f[0x3C / 4] = 0.0f;
    f[0x44 / 4] = 0.0f;
    f[0x48 / 4] = 0.0f;
    f[0x4C / 4] = 0.0f;
    f[0x50 / 4] = 0.0f;
    f[0x58 / 4] = 0.0f;
    f[0x5C / 4] = 0.0f;
    f[0x60 / 4] = 0.0f;
    f[0x64 / 4] = 0.0f;
    f[0x68 / 4] = 0.0f;
    f[0x6C / 4] = 0.0f;
    f[0x70 / 4] = 0.0f;
    f[0x74 / 4] = 0.0f;
    f[0x78 / 4] = 0.0f;
    f[0x7C / 4] = 0.0f;
    f[0x80 / 4] = 0.0f;
    f[0x84 / 4] = 0.0f;
    f[0x88 / 4] = 0.0f;
    f[0x8C / 4] = 0.0f;
    f[0x90 / 4] = 0.0f;
    f[0x94 / 4] = 0.0f;
    f[0x98 / 4] = 0.0f;
    f[0x9C / 4] = 0.0f;
    f[0xA0 / 4] = 0.0f;
    f[0xA4 / 4] = 0.0f;
    f[0xA8 / 4] = 0.0f;
    f[0xAC / 4] = 0.0f;
    f[0xB0 / 4] = 0.0f;
    f[0xB4 / 4] = 0.0f;
}
