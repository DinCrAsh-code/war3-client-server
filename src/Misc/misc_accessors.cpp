//============================================================================
//  Two one-line accessors this call tree reaches but never gives enough
//  context to name a real owning type for - each is the only instruction
//  touching its field, with no other member of the same object read
//  anywhere in this dump. Kept minimal rather than guessed: an opaque
//  struct with everything before the one named field as a reserved block
//  (the `char m_reservedNN[...]` idiom CLAUDE.md asks for), no invented
//  members past what is actually read.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F60A030 - reads and returns a single game-global dword. Nothing in
//  this call tree writes it or says what it means.
//----------------------------------------------------------------------------
extern int g_unk6FACE754;   // dword_6FACE754

int __fastcall ReadUnk6FACE754()
{
    return g_unk6FACE754;
}

//----------------------------------------------------------------------------
//  0x6F6F9A20 - reads a dword at +0x1D0 of `this`. Only caller in this dump
//  is sub_6F6F9A60, itself not reconstructed - the owning class is unknown
//  beyond "something at least 0x1D4 bytes with an int-sized field there".
//----------------------------------------------------------------------------
struct SUnk6F6F9A20Owner
{
    char m_reserved00[0x1D0];
    int  m_field1D0;
};

int __fastcall ReadUnk6F6F9A20Field(const SUnk6F6F9A20Owner* self)
{
    return self->m_field1D0;
}
