//============================================================================
//  0x6F29E570 - CUnit's vtable slot 70 (+0x118, CFloat* Method_0x118(CFloat*)).
//
//  Two CFloat "channels" live in the +0x18-byte reserved run at +0x228 that
//  Save/Load only partly serialise (unit_save.cpp/unit_load.cpp: "the other
//  two dwords in that block are not reached" - +0x22C is one of the two).
//  Channel A is +0x228, channel B is +0x22C.
//
//  If the unit carries the ability GetFourCC_6F113CC0() ('Bply', the same
//  lookup UnitConfiguredMaxLevel/unit_maxlevel.cpp already uses with the
//  identical (1, 0, 1, 1) argument tuple) both channels are replaced with
//  that ability's own type-record probes at +0x210/+0x214
//  (UnitTypeField210_6F29C830/UnitTypeField214_6F29C870,
//  Unit/unittypequeries.cpp), keyed by a rawcode read off the ability
//  object's own +0x114 - channel A from +0x210, channel B from +0x214, the
//  same call order the shipped code uses (214 first, then 210).
//
//  The result is channel B when the unit's +0x1FC field is exactly 0x20
//  *and* bit 0x20 of +0x280's low byte is set, channel A otherwise - one
//  combined AND, not two independent ifs (the shipped code tests +0x1FC
//  first and only reaches the +0x280 bit test on that branch).
//============================================================================
#include "unit.h"
#include "unittypequeries.h"

//  0x6F0787D0 (Unit/unit_herothunks.cpp), 0x6F113CC0
//  (Misc/misc_trivial_getters.cpp) - the same ability lookup
//  UnitConfiguredMaxLevel (unit_maxlevel.cpp) already declares and calls
//  with this identical argument tuple.
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

unsigned int GetFourCC_6F113CC0();

CFloat* CUnit::Method_0x118(CFloat* out)
{
    //  Declared/loaded in the shipped order - +0x22C (channelB) before
    //  +0x228 (channelA), matching `mov ebx,[esi+22Ch]` ahead of
    //  `mov edi,[esi+228h]`.
    CFloat channelB = *(const CFloat*)(m_reserved228 + 0x04);   // +0x22C
    CFloat channelA = *(const CFloat*)(m_reserved228 + 0x00);   // +0x228

    void* ability = ((SUnitAbilityHost*)this)
                        ->FindAbility(GetFourCC_6F113CC0(), 1, 0, 1, 1);
    if (ability != 0)
    {
        unsigned int typeId = *(unsigned int*)((char*)ability + 0x114);
        //  A genuinely uninitialised scratch, not the zeroing default
        //  constructor - the shipped code never stores into this local
        //  before either probe call writes it.
        CFloat temp(kCFloatNoInit);
        channelB = *UnitTypeField214_6F29C870(&temp, typeId);
        channelA = *UnitTypeField210_6F29C830(&temp, typeId);
    }

    if (m_int1FC == 0x20 && (m_flags280 & 0x20) != 0)
        *out = channelB;
    else
        *out = channelA;
    return out;
}
