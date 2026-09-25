//============================================================================
//  0x6F44CB20 - `retn 0`.  Zeroes nine consecutive dwords starting one past
//  `this`, leaving the first field (a vtable pointer or discriminant this
//  closure never reaches) untouched.  Nothing here reaches the owning type,
//  so it stays a plain `int*` rather than a guessed-at struct.
//============================================================================

//  0x6F44CB20 (__fastcall, `self` arrives in ecx).
void __fastcall ZeroNineFieldsAfterFirst(int* self)
{
    self[1] = 0;
    self[2] = 0;
    self[3] = 0;
    self[4] = 0;
    self[5] = 0;
    self[6] = 0;
    self[7] = 0;
    self[8] = 0;
    self[9] = 0;
}
