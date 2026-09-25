//============================================================================
//  The hero-evaluation module (0x6F41Bxxx / 0x6F419xxx / 0x6F413xxx) - which
//  of a player's units count as its heroes, in what order, and what they are
//  worth.  Reached from the end-of-game summary blob
//  (Net/netgameresultblob.cpp), which writes the total into the stream twice
//  - raw, and again divided by ten.
//
//  Two class names here are the shipped binary's own, read out of the Storm
//  allocation tags the two arrays are grown and freed under:
//  ".?AUHeroEval@@" and ".?AUBestHeroData@@".  IDA renders both labels with
//  the `AU` of the `struct` mangling prefix glued to the front
//  (`aAuheroeval`), which is where the temptation to call them
//  "AUHeroEval"/"AUBestHeroData" comes from; the bytes at 0x6FA7BB50 and
//  0x6FA7BB34 say otherwise.
//
//  The whole cluster hangs off one enumeration: every agent of agile type
//  'AHer' - the id 0x6F2075E0 hands back, and Warcraft III's own rawcode for
//  the "Hero" ability every hero unit carries - is offered to
//  0x6F419F80, which keeps the ones belonging to this player.
//============================================================================
#ifndef PLAYERHEROEVAL_H
#define PLAYERHEROEVAL_H

#include "storm.h"

class CAgent;
class CAgentPtr;

//----------------------------------------------------------------------------
//  One row of the ranking table 0x6F41B620 builds: the candidate, what it
//  scored, and how many of the other candidates it beat.  Twelve bytes, which
//  is what both arrays' `SetAlloc` multiplies by.
//----------------------------------------------------------------------------
struct HeroEval
{
    CAgent* m_unit;    // +0x00
    int     m_score;   // +0x04
    int     m_beats;   // +0x08 - the rank, counted rather than sorted
};

//----------------------------------------------------------------------------
//  One row of the snapshot the player record caches at +0x2E4: the hero's
//  type id, its level and its experience.  Same twelve bytes.
//----------------------------------------------------------------------------
struct BestHeroData
{
    unsigned int m_typeId;       // +0x00
    int          m_level;        // +0x04
    int          m_experience;   // +0x08
};

//----------------------------------------------------------------------------
//  The player record, viewed for the hero cluster.  A view struct rather
//  than more members on Player/playerscore.h's SPlayerScore, the same
//  "one object, one view per use" split the rest of this record already
//  uses.
//----------------------------------------------------------------------------
struct SPlayerHeroes
{
    //  0x6F41B5A0 - every hero this player owns, in enumeration order.
    //  Returned **by value**: `this` in ecx, the return buffer pushed last,
    //  `retn 4` and no other argument.
    TSGrowableArray<CAgentPtr> CollectHeroes();

    //  0x6F41B620 - the same list, permuted into rank order.  Also by value.
    TSGrowableArray<CAgentPtr> SelectHeroesByRank();

    //  0x6F41B8E0 - refresh the +0x2E4 snapshot if the ranked list has
    //  changed under it.  `retn 0`.
    void RefreshBestHeroes();

    //  0x6F41BA80 - the player's total hero value, times ten.  `recollect`
    //  picks between walking the ranked list afresh and summing the cached
    //  snapshot.  `retn 4`.
    int TotalHeroValue(int recollect);

    char                        m_reserved000[0x30];
    unsigned char               m_playerIndex;    // +0x30
    char                        m_reserved031[0x2E4 - 0x31];
    TSFixedArray<BestHeroData>  m_bestHeroes;     // +0x2E4
};

//  0x6F2075E0 - the agile type id the enumeration asks for: 'AHer'.
unsigned int __cdecl HeroAgileTypeId();

//  0x6F419F80 - the visitor.  Never called from here, only handed to the
//  enumerator by address; a naked redirect in playerherothunks.cpp.
int __fastcall HeroCollectVisitor(void* agent, void* context);

//  0x6F46D1B0 - Game/game.h's enumerator, declared there.
//  0x6F2A0210 / 0x6F26EB30 - Unit/unit_herolevel.cpp and
//  Unit/unit_heroexperience.cpp.
int __fastcall HeroLevel(void* unit);
int __fastcall HeroExperience(void* unit);

//  0x6F2777B0 / 0x6F277850 / 0x6F2778B0 - the three per-hero value terms
//  (Unit/unit_herovalueterms.cpp).  `retn 4` each, `this` in ecx.
struct SHeroValueSource
{
    int TermA(int summaryOnly);
    int TermB(int summaryOnly);
    int TermC(int summaryOnly);
};

#endif
