//============================================================================
//  0x6F6FE790 - SOwnedRefChecksumCache::FoldGroup: one "group" of tagged
//  field records (stride 0x18, elements of both arrays
//  SOwnedRefChecksumCache owns): a seed pair at +0x00/+0x04, then a nested
//  record array (+0x0C count, +0x10 base, stride 0x418) - each record
//  folded through SOwnedRefChecksumCache::FoldRecord
//  (ownedrefchecksumcachefoldrecord.cpp - its own TU, same inlining
//  reason as every other split in this chain).  `this` is forwarded
//  unchanged into FoldRecord - the dump's own `mov ecx,[esp+...+var_4]`
//  spill/reload just before each call to sub_6F6FA650 is exactly that: the
//  incoming receiver is preserved across the loop body's own register
//  pressure and handed straight through, so both vtable dispatches inside
//  FoldRecord go through *this cache's* own vtable, never the group's or
//  the record's.
//============================================================================
#include "customdatachecksumcache.h"

unsigned int __thiscall SOwnedRefChecksumCache::FoldGroup(void* groupVoid)
{
    unsigned char* group = (unsigned char*)groupVoid;
    unsigned int seedA = *(unsigned int*)(group + 0x00);
    unsigned int seedB = *(unsigned int*)(group + 0x04);
    int recordCount = *(int*)(group + 0x0C);

    unsigned int hash = (seedA << 3) | (seedA >> 29);
    hash ^= seedB;
    hash = (hash << 3) | (hash >> 29);

    //  Signed `test ebp,ebp ; jle` guard in the dump, not the unsigned
    //  `jbe` the two count-guarded loops elsewhere in this chain use - kept
    //  as a signed comparison here to match.  `group + 0x10` (the record
    //  array base) is re-read fresh every iteration rather than hoisted
    //  into a local - the dump's own `mov eax,[ebx+16]` sits inside the
    //  loop body, not before it, the same "the callee could reach back and
    //  change it" caution this whole chain applies to every array base it
    //  reads (HashTaggedFieldRecordCollection's own header note).
    if (recordCount > 0)
    {
        unsigned int offset = 0;
        do
        {
            SFieldRecord* record = (SFieldRecord*)(*(char**)(group + 0x10) + offset);
            unsigned int contribution = FoldRecord(record);
            hash ^= contribution;
            hash = (hash << 3) | (hash >> 29);
            offset += 0x418;
            --recordCount;
        } while (recordCount != 0);
    }

    return hash;
}
