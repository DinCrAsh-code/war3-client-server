//============================================================================
//  0x6F3B1760 - `retn 4`, `__fastcall(key in ecx, outA in edx)` plus one
//  stack out-param (outB).  A 12-entry sorted range table: range `i`
//  covers `[boundary[i], boundary[i+1])`, where `boundary` is one run of
//  13 dwords at a 0x10-byte stride (`unk_6F9425F8` names the dword at
//  index 1 - the loop's first "low" read is 0x10 bytes *before* that
//  symbol, i.e. index 0, which IDA never named on its own).  On a hit,
//  the matching range's own two payload dwords - `dword_6F9425EC`/
//  `dword_6F9425F0`, the same 0x10 stride, indexed by the winning range
//  rather than by boundary - land in `*outA`/`*outB`.  This is
//  `sub_6F3B1D00`'s own "resolve a numeric map/script id" lookup, called
//  once for the map's own script (the two literal-named `common.j`/
//  `blizzard.j` calls skip straight past it, per that function's own
//  branch on `ebx`).  Nothing in this closure's own reach names what the
//  12 ranges or their two payload columns actually mean.
//============================================================================
extern char g_scriptRangeBoundaries[];   // unk_6F9425F8
extern char g_scriptRangePayloadA[];     // dword_6F9425EC
extern char g_scriptRangePayloadB[];     // dword_6F9425F0

int __fastcall LookupScriptRangeEntry(unsigned int key, unsigned int* outA, unsigned int* outB)
{
    char* boundaryHigh = g_scriptRangeBoundaries;

    for (unsigned int index = 0; index < 0xC; index++, boundaryHigh += 0x10)
    {
        unsigned int low  = *(unsigned int*)(boundaryHigh - 0x10);
        unsigned int high = *(unsigned int*)boundaryHigh;

        if (key < low)
            continue;
        if (key < high)
        {
            *outA = *(unsigned int*)(g_scriptRangePayloadA + index * 0x10);
            *outB = *(unsigned int*)(g_scriptRangePayloadB + index * 0x10);
            return 1;
        }
    }

    return 0;
}
