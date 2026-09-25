//============================================================================
//  0x6F040030-0x6F040110 / 0x6F0407B0-0x6F040810 - worklist batch E, slots
//  167-171/177-179: eight of CAbility's own field accessors, one shipped
//  module (0x6F0400xx/0x6F0407xx) apart from ability_slots.cpp's own
//  0x6F02Exx/0x6F0522xx.
//
//  Every one of the eight opens the identical two-call preamble: resolve
//  the ability's cached generic field-lookup record
//  (CAbility::GetCachedFieldRecord, ability_slots.cpp, 0x6F02E640 - not a
//  vtable slot itself, just a real out-of-line call every one of these
//  eight makes), and on a cache miss fall back to a fresh
//  LookupAbilityFieldRecord(m_typeId, "CAbility::GetXxx") lookup
//  (ability_typefieldrecord.cpp; the string is a Storm allocation-log
//  tag, not a field selector - every one of these eight gets back the
//  same per-typeId record).  What differs per function is
//  only the field-name string and which offset(s) of the resolved record
//  get read afterward - five distinct shapes:
//
//      GetEffectArt/GetAreaEffectArt/GetTargetArt - a count at one offset,
//      an array of pointers at the next dword, indexed by the caller's
//      argument clamped to [0, count-1];
//      GetEffectSound/GetEffectSoundLooped - a single dword, `this`
//      unused otherwise, no stack argument;
//      GetMissileSpeed/GetMissileArc - a single dword copied through the
//      caller's out pointer, which is handed straight back (the `mov
//      eax,[esp+arg_0]` before the store, same shape as
//      DefaultFloatFieldQuery's own out-pointer return);
//      GetMissileHoming - a single dword returned directly, no out
//      pointer at all.
//
//  The count+array shape's clamp is a mask-free `cmp`/`jb`: the shipped
//  code keeps `count-1` in the same register it would overwrite with the
//  caller's index, and only overwrites it when the index is provably in
//  range (`count-1 >= index`, unsigned) - written below as an ordinary
//  clamp rather than the branch's own shape, since both produce the same
//  value and this compiler's own codegen for the clamp idiom already
//  matches it (see DefaultIntFieldQuery's own family for the same
//  "shape, not exact branch direction" reasoning).
//============================================================================
#include "ability.h"
#include "CFloat.h"
#include "ability_typefieldrecord.h"

//----------------------------------------------------------------------------
//  The shared count+array read, factored only because C++ has no way to
//  spell "repeat this exact sequence" without one - each caller below is
//  its own real function at its own address, and the shipped binary
//  duplicates this exact instruction sequence at each of the three call
//  sites rather than sharing it, so this helper is `inline` and expected
//  to be folded back into each caller rather than emitting a real call.
//----------------------------------------------------------------------------
static inline void* IndexFieldArray(void* record, int countOffset,
                                    int arrayOffset, int index)
{
    int count = *(const int*)((const char*)record + countOffset);
    if (count == 0)
        return 0;

    //  The clamped index, not `index` itself, is what stays live in one
    //  register throughout: it starts as count-1 and is overwritten with
    //  the caller's index only when that index is provably in range -
    //  the opposite direction from an `if (index > last) index = last;`
    //  clamp, and the one the shipped code actually takes.
    unsigned int clamped = (unsigned int)(count - 1);
    if (clamped >= (unsigned int)index)
        clamped = (unsigned int)index;

    void** rows = *(void***)((char*)record + arrayOffset);
    return rows[clamped];
}

//----------------------------------------------------------------------------
//  0x6F040030 - slot 167, GetEffectArt.  Count at +0xD4, array at +0xD8.
//----------------------------------------------------------------------------
void* CAbility::GetEffectArt(int index)
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetEffectArt");
    return IndexFieldArray(record, 0xD4, 0xD8, index);
}

//----------------------------------------------------------------------------
//  0x6F040080 - slot 168, GetAreaEffectArt.  Count at +0xE0, array at +0xE4.
//----------------------------------------------------------------------------
void* CAbility::GetAreaEffectArt(int index)
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetAreaEffectArt");
    return IndexFieldArray(record, 0xE0, 0xE4, index);
}

//----------------------------------------------------------------------------
//  0x6F040110 - slot 169, GetTargetArt.  Count at +0xBC, array at +0xC0.
//----------------------------------------------------------------------------
void* CAbility::GetTargetArt(int index)
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetTargetArt");
    return IndexFieldArray(record, 0xBC, 0xC0, index);
}

//----------------------------------------------------------------------------
//  0x6F0400D0 - slot 170, GetEffectSound.  A single dword at +0x30, no
//  stack argument.
//----------------------------------------------------------------------------
void* CAbility::GetEffectSound()
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetEffectSound");
    return *(void**)((char*)record + 0x30);
}

//----------------------------------------------------------------------------
//  0x6F0400F0 - slot 171, GetEffectSoundLooped.  Same shape, +0x34.
//----------------------------------------------------------------------------
void* CAbility::GetEffectSoundLooped()
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetEffectSoundLooped");
    return *(void**)((char*)record + 0x34);
}

//----------------------------------------------------------------------------
//  0x6F0407B0 - slot 177, GetMissileSpeed.  A single dword at +0x68,
//  copied through the caller's out pointer (which is handed back
//  unchanged, the same shape DefaultFloatFieldQuery's family uses).
//----------------------------------------------------------------------------
CFloat* CAbility::GetMissileSpeed(CFloat* out)
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetMissileSpeed");
    *out = *(const CFloat*)((char*)record + 0x68);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F0407E0 - slot 178, GetMissileArc.  Same shape, +0x6C.
//----------------------------------------------------------------------------
CFloat* CAbility::GetMissileArc(CFloat* out)
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetMissileArc");
    *out = *(const CFloat*)((char*)record + 0x6C);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F040810 - slot 179, GetMissileHoming.  A single dword at +0x70,
//  returned directly - no out pointer at all, unlike its two neighbours
//  above.
//----------------------------------------------------------------------------
int CAbility::GetMissileHoming()
{
    unsigned int typeId = m_typeId;
    void* record = GetCachedFieldRecord();
    if (record == 0)
        record = LookupAbilityFieldRecord(typeId, "CAbility::GetMissileHoming");
    return *(int*)((char*)record + 0x70);
}
