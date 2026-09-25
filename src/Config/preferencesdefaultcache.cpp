//============================================================================
//  0x6F005840 - store one preference's default text into the shared
//  0x1000-byte text-default cache (unk_6FAAE310, the same scratch bucket
//  preferencecachefind.cpp's own note documents), creating the cache entry
//  first if one does not already exist for `key`.  `this` shares
//  CPreferencesWar3::GetIntValue's own `key >= 0x4F` bound
//  (Config/preferences.cpp), so it is almost certainly a sibling method of
//  that class operating on its own base table rather than the +0x28
//  SPreferenceCache - but neither of its own two callees
//  (sub_6F004330/sub_6F005500) is reconstructed by this batch, so `this` is
//  kept opaque per CLAUDE.md's "only touched members get names" rule rather
//  than guessed onto CPreferencesWar3's own layout.
//============================================================================

extern char g_preferenceBucket;   // unk_6FAAE310 - Config/preferences.h

//  0x6F004330 / 0x6F005500 - a hash-table lookup and insert keyed by an
//  int, on `this` directly (no `this` adjustment at the call site - not
//  SPreferenceCache::Find, which is a different address, 0x6F004390).
//  Below this batch's own 43; declared and thunked to their own real
//  addresses.  `__fastcall` would put `key` in edx instead of leaving it
//  on the stack alongside the other argument(s) - both are ecx-only-for-
//  `this` ABIs (`retn 8`/`retn 0Ch`, confirmed by their own dump/call
//  site), so both are methods on a minimal opaque receiver instead.
struct TextDefaultTable
{
    void* Find(int key, void* scratchBucket);
    void* Insert(int key, int, int);
};

void* FindTextDefaultEntry(void* table, int key, void* scratchBucket)
{
    return ((TextDefaultTable*)table)->Find(key, scratchBucket);
}
void* InsertTextDefaultEntry(void* table, int key, int a, int b)
{
    return ((TextDefaultTable*)table)->Insert(key, a, b);
}

__declspec(naked) void* TextDefaultTable::Find(int, void*)
{
    __asm { mov eax, 06F004330h }
    __asm { jmp eax }
}
__declspec(naked) void* TextDefaultTable::Insert(int, int, int)
{
    __asm { mov eax, 06F005500h }
    __asm { jmp eax }
}

void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);

//  `key` and `text` are BOTH stack arguments (`retn 8` confirmed by this
//  function's own dump) with `this` in ecx alone - not edx, which this
//  function never touches.  A free function cannot spell plain `__thiscall`
//  in this compiler (see Net/dataallocator.h's own note on the identical
//  problem for CDataAllocator::ConstructCWidget), so this is a method on a
//  minimal opaque receiver instead, purely for the calling convention.
struct STextDefaultOwner
{
    int StoreTextDefault(int key, const char* text);
};

int __fastcall StoreTextDefaultOn(void* self, int key, const char* text)
{
    return ((STextDefaultOwner*)self)->StoreTextDefault(key, text);
}

int STextDefaultOwner::StoreTextDefault(int key, const char* text)
{
    if (text == 0)
        return 0;
    if (key >= 0x4F)
        return 0;

    void* entry = FindTextDefaultEntry(this, key, &g_preferenceBucket);
    if (entry == 0)
    {
        entry = InsertTextDefaultEntry(this, key, 0, 0);
        *(int*)entry = key;
    }

    Storm_501((char*)entry + 0x18, text, 0x1000);
    return 1;
}
