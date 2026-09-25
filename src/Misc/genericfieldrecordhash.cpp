//============================================================================
//  0x6F4C8EA0 / 0x6F4C9050 - a generic "tagged field record collection"
//  checksum walker, reached from CAbilityDatabase::GetChecksum
//  (Item/abilitydatabase_checksum.cpp) through the alias at 0x6F4C9050.
//
//  Neither address names an owning class anywhere this repo has looked -
//  the receiver is reached purely through three vtable slots
//  (+0x0C/+0x10/+0x14), never through a named type - so this is written
//  against the raw interface CLAUDE.md's own vtable-dispatch example
//  documents, not a real class, matching only what the disassembly proves.
//
//  Walk shape: vtable[0x10](collection) returns an outer "group" count;
//  for each group (walked top-down, `do { --groupCount; ... }
//  while (groupCount != 0)` - the "index doubles as remaining count" idiom
//  docs/msvc-vc8-idioms.md documents elsewhere in this repo), vtable[0x0C]
//  (no arguments beyond `this`) returns an inner "record" count for that
//  group, and vtable[0x14](collection, recordIndex, groupIndex) fetches one
//  tagged record pointer or null. A record's first byte is a type tag -
//  'F' (float), 'I' (dword) or 'S' (a pointer to a nul-terminated string
//  one level further out, at `record+4`) - and the value at `record+4`
//  itself for the two scalar tags.
//
//  Every byte contributes `(b*0x7A2D)^b` to the running hash - the same
//  scramble Sync/ctimesynchashinto.cpp's own TimeSyncByteScramble already
//  names for a different accumulator, kept as its own local copy here
//  rather than shared: the two call sites are in unrelated modules and nothing
//  in this dump ever calls one from the other's own translation unit.
//
//  Not called live by anything in this repo yet - checksum-provider-
//  registry.md's own note is that only CAbilityDatabase's vtable slot 2
//  (SetRecordSet) is ever dispatched live; slot 0 (GetChecksum, and
//  therefore this walker) has no live caller found so far.
//============================================================================

//----------------------------------------------------------------------------
//  A single byte, widened to 32 bits, run through the `(b*0x7A2D)^b`
//  scramble - see this file's own header note.
//----------------------------------------------------------------------------
static inline unsigned int FieldByteScramble(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

typedef unsigned int  (__thiscall *GetGroupCountFn)(void*);
typedef unsigned int  (__thiscall *GetRecordCountFn)(void*);
typedef unsigned char* (__thiscall *GetTaggedRecordFn)(void*, int, int);

//  0x6F4C8EA0 - see this file's own header note for the walk shape.
unsigned int __fastcall HashTaggedFieldRecordCollection(void* collection)
{
    unsigned int hash = 0;

    //  `collection`'s own vtable is re-read fresh before every indirect
    //  call below, never hoisted into a local - the same
    //  "the callee itself could reach back and change it" reasoning
    //  GameDataContextChecksumProviderHash's own header comment gives for
    //  its identical re-read of `context->m_pCache`, and exactly what the
    //  dump's own repeated `mov eax,[edi]` before each `call` shows.
    unsigned int groupCount = ((GetGroupCountFn)(*(void***)collection)[0x10 / 4])(collection);
    if (groupCount != 0)
    do
    {
        --groupCount;
        unsigned int recordCount = ((GetRecordCountFn)(*(void***)collection)[0x0C / 4])(collection);
        if (recordCount != 0)
        {
            do
            {
                --recordCount;
                unsigned char* record =
                    ((GetTaggedRecordFn)(*(void***)collection)[0x14 / 4])(collection, recordCount, groupCount);
                if (record != 0)
                {
                    unsigned char tag = record[0];
                    //  Tested in this exact order - F, then I, then S -
                    //  matching the dump's own `cmp cl,'F'`/`cmp cl,'I'`/
                    //  `cmp cl,'S'` chain rather than a `switch`, which
                    //  this compiler is free to reorder by value.
                    if (tag == 'F')
                    {
                        //  The shipped body's own `fld`/`fstp` round trip
                        //  through a stack temp to get at the float's raw
                        //  bits, then the same byte-order extraction 'I'
                        //  uses below: low pair first (`cl`/`ch` of the
                        //  reloaded dword), then the high word's own two
                        //  bytes off a `shr eax,16` (`al`/`ah` of the
                        //  shifted value) - not a uniform `(v>>n)&0xFF`
                        //  four times.
                        float f = *(float*)(record + 4);
                        unsigned int v = *(unsigned int*)&f;
                        unsigned int b0 = v & 0xFF;
                        unsigned int b1 = (v >> 8) & 0xFF;
                        unsigned int folded = FieldByteScramble(b0) + FieldByteScramble(b1);
                        v >>= 16;
                        unsigned int b2 = v & 0xFF;
                        unsigned int b3 = (v >> 8) & 0xFF;
                        folded += FieldByteScramble(b2);
                        hash += FieldByteScramble(b3) + folded;
                    }
                    else if (tag == 'I')
                    {
                        unsigned int v = *(unsigned int*)(record + 4);
                        unsigned int b0 = v & 0xFF;
                        unsigned int b1 = (v >> 8) & 0xFF;
                        unsigned int folded = FieldByteScramble(b0) + FieldByteScramble(b1);
                        v >>= 16;
                        unsigned int b2 = v & 0xFF;
                        unsigned int b3 = (v >> 8) & 0xFF;
                        folded += FieldByteScramble(b2);
                        hash += FieldByteScramble(b3) + folded;
                    }
                    else if (tag == 'S')
                    {
                        const char* s = *(const char**)(record + 4);
                        if (s != 0)
                        {
                            for (; *s; ++s)
                                hash += FieldByteScramble((unsigned char)*s);
                        }
                    }
                }
            } while (recordCount != 0);
        }
    } while (groupCount != 0);

    return hash;
}

//  0x6F4C9050 - `jmp HashTaggedFieldRecordCollection`.  A one-instruction
//  linker-alias-shaped tail call - docs/msvc-vc8-idioms.md's "A
//  one-instruction jmp alias needs both sides to be the same convention" -
//  same `__fastcall(void*)` signature on both ends, so /O2 folds this
//  forwarder into the bare `jmp` the dump shows.
unsigned int __fastcall HashTaggedFieldRecordCollectionAlias(void* collection)
{
    return HashTaggedFieldRecordCollection(collection);
}
