//============================================================================
//  The parameter block 0x6F267500 constructs.  See unitcreateparams.cpp.
//
//  Only the members that constructor touches were named at first; the
//  batch that reconstructed CreateItemAtPosition's own populate step
//  (PopulateItemCreateParams, 0x6F2B90C0, item_populate.cpp) widened it
//  past +0x57 to the fields *that* function touches - a genuinely shared
//  block: its one constructor address is exactly the one the unit factory
//  (0x6F29F990, jasscreateunit.h) calls too, and nothing else in this repo
//  ever declares a local `SUnitCreateParams`, so growing it here cannot
//  move any other function's own frame.
//============================================================================
#ifndef UNITCREATEPARAMS_H
#define UNITCREATEPARAMS_H

//  dword_6F92ED98 - widgetsprite.cpp's integer animation-speed sentinel,
//  re-declared here rather than re-named: one address, one C++ name.
extern const int g_animationSpeedSentinel;
//  dword_6F931ECC - a read-only word this block's +0x24 is seeded from.
extern const int g_unk6F931ECC;
//  dword_6FAB4354 - a mutable global copied into five of the members.
extern int g_unk6FAB4354;

//  The sub-object at +0x2C, 0xC bytes wide, whose own constructor
//  (0x6F011300, `retn 4`, SEH-framed) takes a single zero.  The unit path
//  never reads it back, so it stayed opaque there; PopulateItemCreateParams
//  treats the identical offset as an STStringField and calls Assign() on
//  it (item_populate.cpp), which is why frame.h's own RCStaticString
//  already derives from this type - it is a reference-counted string base,
//  not a plain reserved block.
struct SUnitCreateParamsSub
{
    SUnitCreateParamsSub(int initial);
    char m_reserved00[0x0C];
};

struct SUnitCreateParams
{
    SUnitCreateParams();          // 0x6F267500

    char  m_reserved00[0x0C];
    //  0x0C - PopulateItemCreateParams' own facing angle, in radians
    //  (GetBuildingAngle() * g_CFloatDegToRad).  The unit path leaves this
    //  reserved gap alone.
    int   m_facingRadiansBits;    // +0x0C
    //  0x10 - the item type's own field90, copied through unconverted.
    float m_float10;              // +0x10
    int   m_int14;                // +0x14
    //  0x18 - zeroed by PopulateItemCreateParams; the unit path's own
    //  constructor never touches it (a real gap there, not an oversight).
    int   m_int18;                 // +0x18
    float m_float1C;              // +0x1C - the sentinel, converted
    int   m_int20;                // +0x20 - ctor seeds 1; the item populate
                                   //  step below overwrites it with 0
    int   m_int24;                // +0x24 - zeroed, then re-seeded
    int   m_int28;                // +0x28
    SUnitCreateParamsSub m_sub2C; // +0x2C
    int   m_int38;                // +0x38
    int   m_int3C;                // +0x3C
    int   m_int40;                // +0x40
    int   m_int44;                // +0x44
    int   m_int48;                // +0x48
    int   m_int4C;                // +0x4C
    int   m_int50;                // +0x50
    int   m_int54;                // +0x54
    //  0x58-0x78 - PopulateItemCreateParams' own tail: a constant flags
    //  word, five of ITEM_TYPE_DATA's own fields copied straight across,
    //  the two powerup/pawnable predicates, and a resolved string handle.
    //  The unit path's own frame has room (0x1AC carved for the whole
    //  block and its neighbours) but nothing in this call tree shows it
    //  writing any of these, so they are named for the item path alone.
    int   m_int58;                // +0x58
    int   m_int5C;                // +0x5C
    int   m_int60;                // +0x60
    int   m_int64;                // +0x64
    int   m_int68;                // +0x68
    int   m_int6C;                // +0x6C
    int   m_int70;                // +0x70
    int   m_int74;                // +0x74
    int   m_int78;                // +0x78
};

#endif
