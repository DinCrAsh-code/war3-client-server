//  g_unk6FAB778C - the "tempest presence" host: a global object whose
//  vtable-style array at +0x0C/+0x1C (already partially characterized
//  outside this session, see docs/notes/checksum-provider-registry.md)
//  TempestProviderListHash walks below, and whose +0x40/+0x1C/+0x50 also
//  feed CNetData::FireTickNotification's own "birth tag"/"presence tag"
//  legs directly (not through the provider-list walk at all).  Left
//  opaque (raw offset casts, no named struct) rather than typed as
//  SNetSessionInfo/CNetPlayerRecord: this session found no evidence either
//  way for which class it really is, only that it is a *different* object
//  from CNetData_FireTickNotification's own `self` (both are read in that
//  function, at different addresses).
extern void* g_unk6FAB778C;

//  0x6F543D40 - folds `presenceHost`'s own `+0x0C`-array (`+0x1C` entries,
//  each 8 bytes: a tag dword then an object pointer) into one running
//  "tempest" hash: an entry is live when its tag is exactly 0xFFFFFFFE
//  (checked with `cmp ...,0FFFFFFFEh` / `jnz <skip>` - the *equal* branch
//  falls through into processing, so 0xFFFFFFFE is the "populated" tag,
//  not an "empty" sentinel the way the checksum-provider array's null
//  pointers are - an earlier comment on this function had it backwards),
//  and each live entry's own callback lives at its object's vtable slot
//  0x3C/4 - called `(this = entry object, unsigned int* accum)`,
//  returning void and mutating the hash through the pointer rather than
//  through its own return value, which is why `hash` is declared once and
//  its address handed to every call instead of being folded from a
//  returned result the way GameDataContextChecksumProviderHash folds its
//  own callee's return value.  The array pointer and the count are both
//  re-read fresh out of `presenceHost` on every pass (the dump's own
//  `[edi+0Ch]`/`[edi+1Ch]` loads are inside the loop, not hoisted) - the
//  same "a callback in the loop could reach back and mutate the structure
//  being walked" idiom GameDataContextChecksumProviderHash uses for its
//  own `context->m_pCache`.  `retn` bare, `this` in ecx, no stack args,
//  result in eax.  See docs/notes/checksum-provider-registry.md for what
//  is and is not established about the object this walks.
//
//  Kept in its own translation unit rather than folded into
//  netcommand_dispatch.cpp (where the naked thunk used to live) or
//  checksumproviderdebugprint.cpp (its other caller): both of those files
//  call this function, the shipped binary calls it too (never inlines it -
//  it is a real `call sub_6F543D40` at every call site in every dump that
//  reaches it), and with /Ob2 a small function *defined* in the same TU
//  as one of its callers is a standing invitation for this compiler to
//  auto-inline it there even without a request to - which happened once:
//  with the body visible in netcommand_dispatch.cpp,
//  CNetData_FireTickNotification's own listing lost an instruction
//  relative to its committed report (32/55 -> 31/55) purely from having
//  this function's implementation nearby, with no source change to
//  FireTickNotification itself.  A separate TU makes that impossible -
//  this build has no `/GL` whole-program flag, so nothing here can ever
//  be inlined across a translation-unit boundary.
//
//  Written as a `for` loop reading the count fresh out of the condition
//  every pass (rather than an explicit `if (count) do {...} while` guard)
//  because that is what got this compiler to emit the shipped body's own
//  `cmp [edi+1Ch],0 ; jbe <end>` entry test - a hand-written `do/while`
//  guard produced a `je` there instead, a cosmetic but avoidable
//  instruction-selection difference for the same unsigned-count-is-zero
//  check.
unsigned int __fastcall TempestProviderListHash(void* presenceHost)
{
    struct TempestEntry
    {
        unsigned int tag;
        void*        obj;
    };

    unsigned int hash = 0;

    for (unsigned int i = 0; i < *(unsigned int*)((char*)presenceHost + 0x1C); ++i)
    {
        TempestEntry* entries =
            (TempestEntry*)*(void**)((char*)presenceHost + 0x0C);
        TempestEntry* entry = &entries[i];
        if (entry->tag == 0xFFFFFFFEu)
        {
            void* obj = entry->obj;
            if (obj != 0)
            {
                typedef void (__thiscall *TempestHashFn)(void*, unsigned int*);
                TempestHashFn fn = (TempestHashFn)(*(void***)obj)[0x3C / 4];
                fn(obj, &hash);
            }
        }
    }

    return hash;
}
