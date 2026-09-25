//============================================================================
//  0x6F00CC00 - walk `path` back from its own end looking for the first
//  `.` or `\` (stopping outright at a `\` with no truncation, truncating
//  at a `.`), then append `suffix` at whatever length that leaves - the
//  "drop an existing extension, then hang a new suffix off the path" shape
//  jscript-bin1's own closure (callers of the JASS `.j` script-file
//  reader) reaches through 0x6F011D70.
//
//  `Storm_501` here is the same bounded string-append the RCString::Concat
//  note in tools/funcmap.py already documents - `Storm_501(dest, src,
//  maxlen)` - not declared in os.h (only Storm_506 is), so it is declared
//  locally with the exact signature that note gives.
//============================================================================
#include "os.h"   // Storm_506, Storm_501

void __fastcall PathTrimTrailingDotOrSlashThenAppend(char* path, const char* suffix)
{
    int i = (int)Storm_506(path) - 1;

    while (i >= 0)
    {
        char c = path[i];
        if (c == '.')
        {
            path[i] = 0;
            break;
        }
        if (c == '\\')
            break;
        i -= 1;
    }

    Storm_501(path + Storm_506(path), suffix, 0x7FFFFFFF);
}
