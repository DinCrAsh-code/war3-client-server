//============================================================================
//  0x6F261D00 - how many of the table's rows belong to `race`.
//
//  The walk covers two stores at once.  The first `lastIndex` rows come out
//  of the table's own modified-object list at +0x04 - indexed one-based,
//  each object asked for its four-character name and that name packed back
//  into a FourCC - and the rest come out of the shared base store, indexed
//  by a cursor that starts at `-lastIndex` and counts up with the loop, so
//  that it reaches 0 exactly when the modified rows run out.  Either way the
//  answer is one key, and the four questions asked of it are the same:
//
//      * is it a row this table recognises at all      (0x6F2B0000)
//      * whose race column names `race`                (QueryColumn18 +
//                                                       RaceNameToId)
//      * and whose flag column is set                  (QueryColumn38)
//      * then add its own count column                 (QueryColumn88)
//
//  The modified-object handle is released on *every* iteration, including
//  the ones that took the base-store path and left it null - which is why
//  the `xor esi, esi` sits at the top of the loop body rather than inside
//  the first arm.
//
//  The shipped prologue carries a /GS buffer cookie because of the
//  five-byte name buffer; this repo builds at a fixed /GS- and cannot emit
//  the load, the `xor esp` or the check call.
//============================================================================
#include "objectdatatable.h"
#include "vslot4thunk.h"

//  0x6F4C9D20 - the modified-object list's one-based indexer, and
//  0x6F4C8990 - "write your four-character name into this buffer".  Both are
//  naked redirects in objectdatatable_thunks.cpp.
void* __fastcall IndexModifiedObject(void* list, int oneBasedIndex);
//  0x6F4C8990 - "write your four-character name into this buffer".  Three
//  arguments in the shipped ABI, not two: the object arrives in ecx, the
//  buffer in edx and only the size is pushed, and the callee ends `retn 4`.
void __fastcall NamedDataObjectGetName(void* object, char* out, int size);

//  0x6F25B720 - the shared base store, and 0x6F25B1A0 - its own row key by
//  cursor.  Redirects too.
struct SBaseDataStore { int KeyAt(int cursor, int flags); };     // 0x6F25B1A0
SBaseDataStore* __cdecl GetBaseDataStore();

//  0x6F2B0000 - is this key one this table has a row for?  A redirect.
int __fastcall IsKnownDataKey(int key);

struct SVSlot4List { char m_reserved[4]; };

int CObjectDataTable::CountForRace(int race)
{
    int total = 0;

    int lastIndex = CallVSlot4((IVSlot4Target*)
                               *(void**)((char*)this + 4)) - 1;
    int rows = RowCount();
    if (rows <= 0)
        return 0;

    int cursor = -lastIndex;
    int i = 0;
    do
    {
        void* modified = 0;
        int key;

        if (i < lastIndex)
        {
            char name[5];
            modified = IndexModifiedObject(*(void**)((char*)this + 4), i + 1);
            NamedDataObjectGetName(modified, name, 5);
            key = (int)PackFourCCString(name);
        }
        else
        {
            key = GetBaseDataStore()->KeyAt(cursor, -1);
        }

        if (key != 0
            && IsKnownDataKey(key)
            && RaceNameToId(QueryColumn18(modified, key)) == race
            && QueryColumn38(modified, key) != 0)
        {
            total += QueryColumn88(modified, key);
        }

        if (modified != 0)
            ReleaseForward_6F4C8870((TRefCnt*)modified);

        cursor++;
        i++;
    }
    while (i < rows);

    return total;
}
