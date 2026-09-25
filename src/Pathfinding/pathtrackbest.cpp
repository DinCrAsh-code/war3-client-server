//============================================================================
//  0x6F49D770 - CPathClient::TrackBest: scan the coarse search's frontier
//  and record the candidate node closest to reproducing the mover's own
//  facing.
//
//  Every candidate in `frontier` is a node record whose own +0x80 field is
//  a direction vector shaped like CPathTrace::m_direction (the same layout
//  CPathClient itself uses for the same purpose is reused here rather than
//  named separately, since nothing in this call tree proves it is a
//  different type).  A candidate with no direction set (a zero vector - and
//  the test is against the g_CFloatZero global, not a literal, which is what
//  `fld` / `fld dword_6FAAE470` / `fucompp` says) is skipped outright.
//
//  Among the rest, the winner is the one whose target handle resolves, whose
//  own client at +0xA8 has no best node yet, whose squared direction length
//  is *strictly shorter* than the mover's, and whose client's kind nibble at
//  +0x8A matches the one taken off `this` - recorded on **that client** via
//  SetBestNode with the mover as its node and a 20-tick cooldown, after which
//  the scan carries on.
//
//  Anything that gets as far as the length test and fails it, or fails the
//  kind test, is recorded on `this` with a 4-tick cooldown and **the function
//  returns** - that arm ends in the epilogue in the shipped code, not in the
//  loop's increment.
//
//  Four of those five facts were the other way round here until they were
//  read back off the dump instruction by instruction; each one is called out
//  again at the line it governs.  The header used to say the *longest*
//  candidate wins and that the 4-tick arm continues the walk; both were
//  wrong.
//============================================================================
#include "pathmove.h"

static inline float AsHardwareFloat(const CFloat& value) { return *(const float*)&value; }

void CPathClient::TrackBest(SPathFrontier* frontier)
{
    CGridVec2* moverDir = (CGridVec2*)((char*)g_pPathMover + 0x80);

    m_bestTypeTag = -1;
    m_bestHandle = (unsigned int)-1;

    CFloat moverLenSq = moverDir->m_y * moverDir->m_y +
                        moverDir->m_x * moverDir->m_x;
    //  The two handle resolves below go through CPathTrace::ResolveTargetHandle
    //  (0x6F495940) rather than being spelled out as LookupHandle on the two
    //  fields: the shipped code has a real `mov ecx,<recv>` / `call` at both
    //  sites, and inlining the pair costs a `mov` per field at each.
    CHandleObject* moverHandle =
        (CHandleObject*)g_pPathMover->ResolveTargetHandle();
    unsigned char kind = (*(unsigned char*)((char*)this + 0x8A)) & 0x0F;

    //  The count is re-read from the frontier on every iteration and the node
    //  array is fetched inside the guard, both because that is what the shipped
    //  stream does (`cmp [ecx+1Ch], eax` before the loop and `cmp eax,[ecx+1Ch]`
    //  at the bottom, with `mov ecx,[ecx+0Ch]` only after the first test).
    for (unsigned int i = 0; i < frontier->m_count; i++)
    {
        void* node = frontier->Nodes()[i];
        if (node == 0)
            continue;

        CHandleObject* nodeHandle =
            (CHandleObject*)((CPathTrace*)node)->ResolveTargetHandle();
        if (nodeHandle == 0)
            continue;

        CGridVec2* nodeDir = (CGridVec2*)((char*)node + 0x80);
        CFloat nodeLenSq = nodeDir->m_y * nodeDir->m_y +
                           nodeDir->m_x * nodeDir->m_x;

        if (AsHardwareFloat(nodeLenSq) == AsHardwareFloat(g_CFloatZero))
            continue;

        CPathClient* asClient = (CPathClient*)(*(void**)((char*)node + 0xA8));
        //  No null test: the shipped code calls straight through
        //  (`mov edi,[esi+0A8h]` / `mov ecx,edi` / `call`), so the callee
        //  is the one that tolerates a null receiver.
        if (asClient->ResolveBestNode() != 0)
            continue;

        if (nodeHandle != moverHandle ||
            (*(unsigned char*)((char*)moverHandle + 0x80) & 8) != 0)
        {
            //  Read straight off the shipped stream: `fld moverLenSq` /
            //  `fld nodeLenSq` / `fcompp` / `test ah,1` / `je` takes the
            //  fallback when C0 is clear, i.e. when the candidate is *not*
            //  shorter than the mover.  So the candidate has to be strictly
            //  shorter to win, and the operand order is part of the answer -
            //  the mover is loaded first.
            if (AsHardwareFloat(moverLenSq) <= AsHardwareFloat(nodeLenSq))
                goto fallback;

            //  The kind nibble compared here is the *client's* (+0xA8), not
            //  the candidate node's: the shipped code reads `[edi+8Ah]` with
            //  edi already holding [esi+0A8h].
            if ((*(unsigned char*)((char*)asClient + 0x8A) & 0x0F) != kind)
                goto fallback;

            asClient->SetBestNode((const CGridRegistration*)g_pPathMover, 0x14);
            continue;
        }

    fallback:
        //  This arm returns; it does not continue the walk.  The shipped block
        //  ends in the function's own epilogue rather than in the loop's
        //  increment, so the first candidate that fails one of the deeper tests
        //  is recorded and the scan stops there.
        SetBestNode((const CGridRegistration*)node, 4);
        return;
    }
}
