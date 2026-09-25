//============================================================================
//  0x6F49DF00 - CPathClient::RunSearch: three-way classifier between a
//  cached point, a full search, and giving up.
//
//  Every branch below is read off the shipped stream rather than inferred:
//
//    * `out` is the scratch, not a local.  The shipped body copies `*out`
//      into two registers on entry (`mov eax,[esi+4]` / `mov ebp,[esi]`) and
//      writes exactly those two dwords back into `*out` on the way to
//      FinishSearch, which is only meaningful if the callees in between are
//      allowed to scribble on it - and they are: NextCachedPoint, Classify
//      and CoarseSearch are all handed `(to, out)`, the same two pointers, in
//      that order.
//    * NextCachedPoint answering **zero** is the failure: `jz` goes to the
//      `mov eax,2` exit.  A hit falls through into Classify.
//    * CoarseSearch answering non-zero returns **0**, not 2.
//    * `best != 0` is a *refusal*: `jnz` leaves with 1.  Only a null lookup
//      and a zero cooldown reach FinishSearch, and the cooldown test is
//      `ja` against a zeroed register, i.e. unsigned.  The two refusals are
//      one `||`, not two `if`s - that is what lets both branches share the
//      single `mov eax,1` exit and lets the second compare re-use the eax
//      the first one has just proved is zero.
//    * Classify's status is compared 0-then-2 in that order (`sub eax,0` /
//      `jz`, `sub eax,2` / `jnz`), and *anything else* - not just 0 - lands
//      on the restore-and-FinishSearch tail.
//
//  The `switch` is not decoration either: MSVC lowers a two-label switch as
//  `sub eax,0` / `jz` / `sub eax,2` / `jnz`, which is the shipped chain, where
//  an if/else-if tests 2 after the whole case-0 body.
//
//  The previous shape here had four of those five the other way round - it
//  returned 0 for a cache miss and 2 for a failed coarse search, took the
//  `best != 0` branch as success, and handed Classify and CoarseSearch a
//  local copy of `m_position` instead of `out` - so this is a behaviour fix,
//  not a scheduling one.  It is written out this way so the next reader can
//  check each branch against the dump.
//============================================================================
#include "pathmove.h"

int CPathClient::RunSearch(const CGridVec2* to, CGridVec2* out)
{
    CGridVec2 saved = *out;

    if (!NextCachedPoint(to, out))
        return 2;

    switch (Classify(to, out))
    {
    case 0:
        if (CoarseSearch(to, out) != 0)
            return 0;

        {
            CHandleObject* best = LookupHandle(m_bestHandle, m_bestTypeTag);
            if (best != 0 || m_cooldown > 0)
                return 1;
        }
        break;

    case 2:
        return 2;
    }

    *out = saved;
    return FinishSearch(to);
}
