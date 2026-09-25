//============================================================================
//  0x6F50D970 - CModelInstance::ReleaseForSprite: hand this model instance,
//  and every child instance hanging off it, back for one sprite.  A naked
//  redirect in unreconstructed_thunks.cpp until now.
//
//  Three parts: push the sprite at the animation object at +0x98 if there
//  is one, and then - only when bit 0x10 of the flag byte at +0x94 is set -
//  walk a bucket table at +0xC8 (count at +0xC4, stride 12) and recurse
//  into every listed child whose own bit 0 at +0x0C is set.
//
//  **It takes a second argument in edx, and that is why the declaration
//  moved.**  The shipped body's second instruction pair is `mov ebx, edx`,
//  and it puts that value straight back in edx both for
//  SetPairIfFlagged_6F513950 (whose own first stack-free argument *is* edx,
//  sweep_field_setters.cpp) and for every recursive call.  Declared as the
//  thiscall member `CModelInstance::ReleaseForSprite(CSprite*)` it was
//  before, there is no way to name that value at all, so the reconstruction
//  would have handed the shipped 0x6F513950 whatever happened to be in edx.
//  It is a free `__fastcall` taking {instance, notify, sprite} instead, which
//  is what the shipped register use says and what lets the one call site
//  (ReleaseSpriteModel, spriteflags.cpp) forward its own second parameter -
//  the model-release callback its own caller passes it (item_slot16.cpp
//  hands it `ModelReleaseNotify`), and the value the shipping build was
//  already carrying through edx implicitly.  That call site is unchanged in the output: MSVC emits
//  no `mov edx, edx`.
//
//  Two list conventions are the shipped code's and both are signed:
//  a bucket's head at +8 and each node's next at +4 end the walk on
//  `<= 0`, not on null.  `jle` and `jg`, not `jz`/`jnz`.
//
//  Own translation unit: it recurses, and spriteflags.cpp reaches it with a
//  real call.
//============================================================================
#include "sprite.h"

//  sweep_field_setters.cpp's, under its own name.
void __fastcall SetPairIfFlagged_6F513950(void* self, int a, int b);

//  One entry of the child table: only the head at +8 is read, and the
//  stride is twelve bytes.
struct SModelChildBucket
{
    char                 m_reserved00[8];
    struct SModelChild*  m_pHead;      // +0x08
};

struct SModelChild
{
    char            m_reserved00[4];
    SModelChild*    m_pNext;           // +0x04
    CModelInstance* m_pInstance;       // +0x08
    unsigned char   m_flags;           // +0x0C - bit 0 gates the recursion
};

struct SModelInstanceBody
{
    char               m_reserved00[0x94];
    unsigned char      m_flags94;      // +0x94 - bit 0x10 gates the walk
    char               m_reserved95[3];
    void*              m_pAnim;        // +0x98
    char               m_reserved9C[0x28];
    unsigned int       m_bucketCount;  // +0xC4
    SModelChildBucket* m_pBuckets;     // +0xC8
};

void __fastcall ModelInstanceReleaseForSprite(CModelInstance* instance,
                                              void* notify, CSprite* sprite)
{
    SModelInstanceBody* self = (SModelInstanceBody*)instance;

    if (self->m_pAnim != 0)
        SetPairIfFlagged_6F513950(self->m_pAnim, (int)notify, (int)sprite);

    if ((self->m_flags94 & 0x10) == 0)
        return;

    SModelChildBucket* bucket = self->m_pBuckets;
    unsigned int remaining = self->m_bucketCount;
    if (remaining == 0)
        return;

    do
    {
        //  The head is read out of the bucket before the counter is
        //  decremented, which is the shipped order and keeps the `lea
        //  ebp,[eax+8]` cursor pointing at the head rather than the bucket.
        SModelChild* child = bucket->m_pHead;
        remaining--;

        while ((int)(unsigned int)(unsigned long)child > 0)
        {
            if ((child->m_flags & 1) != 0)
                ModelInstanceReleaseForSprite(child->m_pInstance, notify, sprite);
            child = child->m_pNext;
        }

        bucket++;
    } while (remaining != 0);
}
