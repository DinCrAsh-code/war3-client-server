//============================================================================
//  sub_6F6506A0 - 0x6F6506A0, shared by NetProvider__Method_0x90's own
//  worker chain and by both W3GS action-handler dumps (Probably_W3GS_
//  ActionHandler and its MaybeServerVersion sibling), so it is written here
//  rather than in any one dump's own source file.
//
//  A null-safe fixed-size compare: `ptr == 0` short-circuits to "matches"
//  (`return 1`) before any byte is read, otherwise the first 16 bytes of
//  `*ptr` are compared against a fixed global pattern and the result is the
//  boolean equality (1 if equal, 0 if any byte differs) - not memcmp's
//  three-way sign. Every caller in this dump treats the result as a plain
//  boolean (`test eax, eax` / `jnz`), consistent with that.
//
//  `g_fixedPattern16` (`unk_6F9709F8`) is an unidentified 16-byte global;
//  nothing in any of these three call trees says what it represents beyond
//  "the pattern this compare is against", so it is left untyped (byte
//  array) rather than guessed at (a GUID/IPv6-address-shaped key is
//  plausible from the call sites but not confirmed).
//============================================================================

extern "C" int memcmp(const void*, const void*, unsigned int);

extern const unsigned char g_fixedPattern16[16];

//  Called as `lea ecx, [obj+0x90]; call sub_6F6506A0` - `this` in ecx, no
//  stack argument, `retn` with nothing to clean up: plain thiscall on a
//  16-byte key, not a free function taking an explicit pointer.
struct FixedPatternKey
{
    int Matches();
};

int FixedPatternKey::Matches()
{
    return !this || memcmp(this, g_fixedPattern16, 16) == 0;
}
