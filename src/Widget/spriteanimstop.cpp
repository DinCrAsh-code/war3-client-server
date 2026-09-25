//============================================================================
//  0x6F4D4B20 - mark every animation track whose own id list mentions
//  `type` as dirty.
//
//  Own translation unit: it calls MarkModelTrackDirty (0x6F50C4A0,
//  modelinstance.cpp) for real.
//
//  Both loops run backwards, which is what the shipped code's
//  decrement-then-test shape says and not a stylistic choice:
//
//  * the outer walks the track array from the last entry down, and its
//    index is what gets passed to MarkModelTrackDirty - so the loop counter
//    is the track number, not a pointer;
//  * the inner walks one track's id list from the top down and stops at the
//    first match, `break`ing out to the next track rather than marking the
//    same track twice.
//
//  The 0x1C element stride is spelled out by the shipped `esi*8 - esi`
//  then two doublings, i.e. 28 bytes, computed once up front and then
//  decremented alongside the index.
//============================================================================
#include "sprite.h"

//  One entry of the sprite's own animation table (+0x30 -> +0x24), 0x1C
//  bytes wide.
struct SSpriteAnimTrack
{
    char  m_reserved00[0x04];
    int   m_idCount;        // +0x04
    int*  m_pIds;           // +0x08
    char  m_reserved0C[0x10];
};

struct SSpriteAnimTable
{
    char              m_reserved00[0x20];
    unsigned int      m_trackCount;    // +0x20
    SSpriteAnimTrack* m_pTracks;       // +0x24
};

void __fastcall StopSpriteAnimationsOfType(CSprite* self, int type)
{
    if (self->m_pModel == 0)
        return;

    unsigned int track = ((SSpriteAnimTable*)self->m_pModelData)->m_trackCount;
    while (track != 0)
    {
        --track;
        SSpriteAnimTrack* entry =
            &((SSpriteAnimTable*)self->m_pModelData)->m_pTracks[track];

        int id = entry->m_idCount;
        while (id-- != 0)
        {
            if (entry->m_pIds[id] == type)
            {
                MarkModelTrackDirty(self->m_pModel, track);
                break;
            }
        }
    }
}
