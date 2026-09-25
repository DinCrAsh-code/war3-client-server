//============================================================================
//  0x6F62AF10 - CEventRegistry::AcquireBucket.
//
//  Eight stripes, chosen by the low three bits of the key: take the stripe's
//  condition variable, walk its bucket backwards for a node with that key,
//  and either hand the node back with the lock still held (reporting which
//  stripe, plus eight when it was taken exclusively) or drop the lock again
//  and report -1.
//
//  A null key takes no lock at all and still writes the -1 out, which is what
//  the two exits below are.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void* CEventRegistry::AcquireBucket(void* key, int exclusive, int* outLock,
                                    int a, int b)
{
    if (key)
    {
        unsigned int stripe = (unsigned int)key & 7;
        Lock(stripe)->Wait(exclusive);

        int link = (int)m_buckets[stripe].Tail();
        while (link > 0)
        {
            SEventNode* node = (SEventNode*)link;
            if (node->m_pKey == key)
            {
                *outLock = stripe + (exclusive ? 8 : 0);
                return node;
            }
            link = (int)m_buckets[stripe].Prev(node);
        }

        Lock(stripe)->SignalOne(exclusive);
    }

    *outLock = -1;
    return 0;
}
