//============================================================================
//  0x6F007670 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): load a record via LoadFontRecordsThunk
//  (0x6F553CC0, shared with GameArchiveHardwareCheck's own unrelated use of
//  it - Game/gamearchivehwcheck.cpp) and, if its own "kind" field reads 2,
//  hash a chunk of it through HashDeviceStringThunk (0x6F685B40) and
//  refuse three specific Chinese locale IDs (0x804 zh-CN, 0x1004 zh-SG,
//  0xC04 zh-HK) - read as a region check, though nothing in this call tree
//  names the record type or confirms that reading.
//
//  The shipped body carries the __except_handler4-shaped SEH frame this
//  fixed /GS- /EHs-c- toolchain cannot reproduce from any source shape -
//  see docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" - so the frame furniture below is omitted
//  and the body written plain.
//============================================================================

//  0x6F553CC0/0x6F685B40 - already reconstructed
//  (Render/fontloadandhashthunks.cpp), same declared shape reused here.
int  __fastcall LoadFontRecordsThunk(void*, void*, int);
void __fastcall HashDeviceStringThunk(void*, void*, void*, void*);

//  0x6F6C4BF0 - already reconstructed (Window/oslocaleid.cpp).
unsigned short __fastcall GetSystemUILanguageId();

//  0x6F0071B0 - already reconstructed (Misc/fixedstringrelease.cpp):
//  releases `self`+8 if non-null.
void __fastcall ReleaseFixedStringField(void* self);

//  Storm ordinal 403 - already declared this shape elsewhere (SMemFree).
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

int __fastcall GameMainCheckLocaleAllowed()
{
    //  Three opaque scratch locals this function itself builds; only the
    //  fields actually read are named.  `record` is the one struct
    //  LoadFontRecordsThunk fills in (kind at +4, a released pointer at
    //  +8 - the same +8 ReleaseFixedStringField's own entry documents).
    struct { int f0; int kind; void* ptr; } record = { 0, 0, 0 };
    unsigned char statusByte = 0;
    unsigned int scratch;
    unsigned int hashOut;
    void* hashScratch;
    unsigned int hashScratch2;

    //  The shipped body's `jz`/`jnz` early-outs and the refusal's own
    //  early `return 0` (skipping the free below) are reproduced with
    //  gotos rather than nested ifs, matching the dump's own control flow
    //  instruction for instruction rather than its C-level shape.
    if (!LoadFontRecordsThunk(&record, &statusByte, (int)&scratch))
        goto allowed;
    if (record.kind != 2)
        goto allowed;

    {
        unsigned int locale;
        HashDeviceStringThunk((char*)record.ptr + 0x20, &hashOut, &hashScratch, &hashScratch2);
        locale = GetSystemUILanguageId();

        if (hashOut != 0x13)
            goto allowed;
        if (locale == 0x804 || locale == 0x1004 || locale == 0xC04)
            goto allowed;

        ReleaseFixedStringField(&record);
        return 0;
    }

allowed:
    if (record.ptr)
        SMemFree(record.ptr, ".?AVAUFixedString@@", -2, 0);
    return 1;
}
