//============================================================================
//  0x6F2C7F50 / 0x6F2C8030 / 0x6F2C7A80 - populate the three model token
//  tables.
//
//  One translation unit for all three: none of them calls another, and the
//  three addresses are neighbours.  Their own callers (the lazy getters in
//  modeltokens.cpp) are in a separate one because those calls are real.
//
//  Every entry is one AddToken(table, name, id) against the file-scope
//  singleton, re-read from the global at every call rather than cached in a
//  register - the shipped code reloads it before each one, which is what a
//  plain global reference produces and a local copy does not.
//
//  The ids are FourCCs for the bone and attachment tables (the first four
//  characters of the name, minus the "bone_" prefix where there is one -
//  "bone_chest" maps to 'ches', "overhead" to 'over'), and a small dense
//  enum for the animation table.  Both are written as the literal constants
//  the disassembly pushes, not as computed expressions: two of the
//  attachment FourCCs are rendered by IDA as code addresses
//  ("origin" pushes `offset loc_6F726966+1`, "overhead" `offset
//  loc_6F766570+2`) precisely because they are plain integers that happen to
//  land in the text section when read as pointers.
//============================================================================
#include "modeltokens.h"

//----------------------------------------------------------------------------
//  0x6F2C7F50 - bone names, as the model format spells them.
//----------------------------------------------------------------------------
void FillBoneTokenizer()
{
    AddToken(g_pBoneTokenizer, "bone_chest", 0x63686573);
    AddToken(g_pBoneTokenizer, "bone_foot", 0x666F6F74);
    AddToken(g_pBoneTokenizer, "bone_hand", 0x68616E64);
    AddToken(g_pBoneTokenizer, "bone_head", 0x68656164);
    AddToken(g_pBoneTokenizer, "bone_turret", 0x74757272);
    AddToken(g_pBoneTokenizer, "alternate", 0x616C7465);
    AddToken(g_pBoneTokenizer, "left", 0x6C656674);
    AddToken(g_pBoneTokenizer, "right", 0x72696768);
    AddToken(g_pBoneTokenizer, "mount", 0x6D6F756E);
    AddToken(g_pBoneTokenizer, "smart", 0x736D6172);
}

//----------------------------------------------------------------------------
//  0x6F2C8030 - model attachment points.
//----------------------------------------------------------------------------
void FillAttachmentTokenizer()
{
    AddToken(g_pAttachmentTokenizer, "chest", 0x63686573);
    AddToken(g_pAttachmentTokenizer, "feet", 0x66656574);
    AddToken(g_pAttachmentTokenizer, "foot", 0x666F6F74);
    AddToken(g_pAttachmentTokenizer, "hand", 0x68616E64);
    AddToken(g_pAttachmentTokenizer, "head", 0x68656164);
    AddToken(g_pAttachmentTokenizer, "origin", 0x6F726967);
    AddToken(g_pAttachmentTokenizer, "overhead", 0x6F766572);
    AddToken(g_pAttachmentTokenizer, "sprite", 0x73707269);
    AddToken(g_pAttachmentTokenizer, "weapon", 0x77656170);
    AddToken(g_pAttachmentTokenizer, "alternate", 0x616C7465);
    AddToken(g_pAttachmentTokenizer, "left", 0x6C656674);
    AddToken(g_pAttachmentTokenizer, "right", 0x72696768);
    AddToken(g_pAttachmentTokenizer, "mount", 0x6D6F756E);
    AddToken(g_pAttachmentTokenizer, "rear", 0x72656172);
    AddToken(g_pAttachmentTokenizer, "smart", 0x736D6172);
    AddToken(g_pAttachmentTokenizer, "first", 0x66697273);
    AddToken(g_pAttachmentTokenizer, "second", 0x7365636F);
    AddToken(g_pAttachmentTokenizer, "third", 0x74686972);
    AddToken(g_pAttachmentTokenizer, "fourth", 0x666F7572);
    AddToken(g_pAttachmentTokenizer, "fifth", 0x66696674);
    AddToken(g_pAttachmentTokenizer, "sixth", 0x73697874);
    AddToken(g_pAttachmentTokenizer, "small", 0x736D616C);
    AddToken(g_pAttachmentTokenizer, "medium", 0x6D656469);
    AddToken(g_pAttachmentTokenizer, "large", 0x6C617267);
    AddToken(g_pAttachmentTokenizer, "gold", 0x676F6C64);
    AddToken(g_pAttachmentTokenizer, "rallypoint", 0x72616C6C);
    AddToken(g_pAttachmentTokenizer, "eattree", 0x65617474);
}

//----------------------------------------------------------------------------
//  0x6F2C7A80 - animation names.
//----------------------------------------------------------------------------
void FillAnimationTokenizer()
{
    AddToken(g_pAnimationTokenizer, "attack", 6);
    AddToken(g_pAnimationTokenizer, "birth", 0);
    AddToken(g_pAnimationTokenizer, "cinematic", 0x80000000);
    AddToken(g_pAnimationTokenizer, "death", 1);
    AddToken(g_pAnimationTokenizer, "decay", 2);
    AddToken(g_pAnimationTokenizer, "dissipate", 3);
    AddToken(g_pAnimationTokenizer, "morph", 7);
    AddToken(g_pAnimationTokenizer, "portrait", 0x0A);
    AddToken(g_pAnimationTokenizer, "sleep", 8);
    AddToken(g_pAnimationTokenizer, "spell", 9);
    AddToken(g_pAnimationTokenizer, "stand", 4);
    AddToken(g_pAnimationTokenizer, "walk", 5);
    AddToken(g_pAnimationTokenizer, "alternate", 0x0B);
    AddToken(g_pAnimationTokenizer, "alternateex", 0x0C);
    AddToken(g_pAnimationTokenizer, "berserk", 0x3E);
    AddToken(g_pAnimationTokenizer, "chain", 0x37);
    AddToken(g_pAnimationTokenizer, "channel", 0x14);
    AddToken(g_pAnimationTokenizer, "complete", 0x22);
    AddToken(g_pAnimationTokenizer, "critical", 0x21);
    AddToken(g_pAnimationTokenizer, "defend", 0x15);
    AddToken(g_pAnimationTokenizer, "drain", 0x35);
    AddToken(g_pAnimationTokenizer, "eattree", 0x38);
    AddToken(g_pAnimationTokenizer, "entangle", 0x3D);
    AddToken(g_pAnimationTokenizer, "fast", 0x11);
    AddToken(g_pAnimationTokenizer, "fill", 0x36);
    AddToken(g_pAnimationTokenizer, "flail", 0x3A);
    AddToken(g_pAnimationTokenizer, "flesh", 0x1B);
    AddToken(g_pAnimationTokenizer, "fifth", 0x2B);
    AddToken(g_pAnimationTokenizer, "fire", 0x1A);
    AddToken(g_pAnimationTokenizer, "first", 0x27);
    AddToken(g_pAnimationTokenizer, "five", 0x30);
    AddToken(g_pAnimationTokenizer, "four", 0x2F);
    AddToken(g_pAnimationTokenizer, "fourth", 0x2A);
    AddToken(g_pAnimationTokenizer, "gold", 0x23);
    AddToken(g_pAnimationTokenizer, "hit", 0x1C);
    AddToken(g_pAnimationTokenizer, "large", 0x33);
    AddToken(g_pAnimationTokenizer, "left", 0x18);
    AddToken(g_pAnimationTokenizer, "light", 0x1E);
    AddToken(g_pAnimationTokenizer, "looping", 0x0D);
    AddToken(g_pAnimationTokenizer, "lumber", 0x24);
    AddToken(g_pAnimationTokenizer, "medium", 0x32);
    AddToken(g_pAnimationTokenizer, "moderate", 0x1F);
    AddToken(g_pAnimationTokenizer, "off", 0x3B);
    AddToken(g_pAnimationTokenizer, "one", 0x2C);
    AddToken(g_pAnimationTokenizer, "puke", 0x39);
    AddToken(g_pAnimationTokenizer, "ready", 0x13);
    AddToken(g_pAnimationTokenizer, "right", 0x19);
    AddToken(g_pAnimationTokenizer, "second", 0x28);
    AddToken(g_pAnimationTokenizer, "severe", 0x20);
    AddToken(g_pAnimationTokenizer, "slam", 0x0E);
    AddToken(g_pAnimationTokenizer, "small", 0x31);
    AddToken(g_pAnimationTokenizer, "spiked", 0x10);
    AddToken(g_pAnimationTokenizer, "spin", 0x12);
    AddToken(g_pAnimationTokenizer, "swim", 0x3C);
    AddToken(g_pAnimationTokenizer, "talk", 0x26);
    AddToken(g_pAnimationTokenizer, "third", 0x29);
    AddToken(g_pAnimationTokenizer, "three", 0x2E);
    AddToken(g_pAnimationTokenizer, "throw", 0x0F);
    AddToken(g_pAnimationTokenizer, "two", 0x2D);
    AddToken(g_pAnimationTokenizer, "turn", 0x17);
    AddToken(g_pAnimationTokenizer, "victory", 0x16);
    AddToken(g_pAnimationTokenizer, "work", 0x25);
    AddToken(g_pAnimationTokenizer, "wounded", 0x1D);
    AddToken(g_pAnimationTokenizer, "upgrade", 0x34);
}

