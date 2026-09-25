//============================================================================
//  0x6F303790 - CCameraWar3::Save, vtable slot 14.  0x6F308810 -
//  CCameraWar3::Load, vtable slot 15.  See gameui.h for the class.
//
//  Both follow the same per-property serialization idiom
//  camerasetupsaveload.cpp's own CCameraSetup::Save/::Load already
//  establish (base-class call, then one WriteXxx/ReadXxx per field in
//  declaration order) - just over CCameraWar3's own, much larger field set:
//  a handful of plain scalars, one TRefCnt* handle pair
//  (WriteAgentHandlePairDupC/ReadUnitRef), the FVec3 pan offset, a raw
//  9-dword block, the update timer's own virtual Save/Load slot, two more
//  plain dwords, the +0xA8 filter-slot array, all eleven SCameraProp-family
//  fields' two modifier slots apiece, the pan-active flag, five more
//  TRefCnt* modifier handles, sixteen more plain floats and a second raw
//  9-dword block, the two 3x3 bounds matrices and the sample-weight array -
//  and Load finishes with a tail call into BindControllerProps to re-bind
//  whatever got read back onto the live controller.
//
//  Two leaves shared with this same dump: 0x6F303650 (WriteFilterSlotArray)
//  and 0x6F307A50 (ReadFilterSlotArray) - the +0xA8 array's own
//  three-float-at-a-time walk, __fastcall(store in ecx, the array's own
//  header in edx) rather than a member function, because the shipped call
//  site leaves the array in edx and the store in ecx rather than the other
//  way round a member call would need.  A third leaf, 0x6F3036C0
//  (ReadMovementModifierRef), is Load's own read-side counterpart of the
//  inline TRefCnt*-handle-pair write CCameraWar3::Save spells directly
//  (WriteModifierHandlePair below) for the five plain TRefCnt* modifier
//  fields (m_pRampModifiers/m_pOffsetModifiers/m_pPanMovement) - the same
//  design as floatpropmodifierrefread.cpp's ReadFloatModifierRef, just
//  building a MovementModifier instead of a FloatModifier.
//
//  **Checked against CCameraSetup's own Save/Load (0x6F3E45C0/0x6F3E4600,
//  camerasetupsaveload.cpp) despite vtable_classes.json listing
//  0x6F307A50/0x6F3036C0 as "shared with CCameraSetup": neither dump calls
//  either address.** CCameraSetup::Save/Load are both a flat 19
//  instructions - the CAgentWar3 base call plus three WriteCFloat/ReadCFloat
//  calls over +0xA4/+0xA8/+0xAC and nothing else - already fully covered by
//  camerasetupsaveload.cpp as it stands. The worklist's "shared" label looks
//  like an artifact of the two classes' address-neighbourhood/vtable-slot
//  similarity rather than a real call-graph edge; nothing was wired in here
//  because there is nothing shipped to wire.
//============================================================================
#include "gameui.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "floatpropmodifier.h"     // FloatModifier (handle/typeTag shape)
#include "positionmodifier.h"      // MovementModifier, SMadeModifier
#include "game.h"                  // LookupHandle
#include "storm.h"                 // SMemAlloc / placement new

CDataStoreScratch* __fastcall WriteAgentHandlePairDupC(CDataStoreScratch* store,
                                                        CAgent** slot);
CDataStore* __fastcall ReadUnitRef(CDataStore* store, SCheckedUnitSlot* out);
CDataStore* __fastcall ReadFloatModifierRef(CDataStore* store, FloatModifier** slot);
void* __fastcall MakeCameraController();
void* __fastcall MakeCameraEventSink();

void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//----------------------------------------------------------------------------
//  0x6F303650 - see the file header.  `retn 0`: both parameters register,
//  no stack argument to clean.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall WriteFilterSlotArray(CDataStoreScratch* store,
                                                    const SFilterSlotArray* arr)
{
    store->WriteDword(arr->m_count);

    unsigned int remaining = arr->m_count;
    if (remaining != 0)
    {
        unsigned int offset = 0;
        do
        {
            const float* elem = (const float*)((const char*)arr->m_data + offset);
            store->WriteFloat(elem[0]);
            store->WriteFloat(elem[1]);
            store->WriteFloat(elem[2]);
            offset += 0x0C;
            remaining -= 1;
        } while (remaining != 0);
    }

    return store;
}

//----------------------------------------------------------------------------
//  0x6F307A50 - see the file header.  The saved count is checked against a
//  cap - the array's own already-live count if it has one, else a large
//  fixed sentinel (0xFFFF0) - before SetCount ever runs; a count over the
//  cap is treated as a corrupt stream and bumps the read cursor by one
//  instead of reading anything.  `retn 0`.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadFilterSlotArray(CDataStore* store, SFilterSlotArray* arr)
{
    unsigned int cap = (arr->m_count != 0) ? arr->m_count : 0xFFFF0u;
    unsigned int count = 0;
    store->ReadDword(&count);

    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        return store;

    if (count > cap)
    {
        store->m_readPos = store->m_field10 + 1;
        return store;
    }

    arr->SetCount(count);

    for (unsigned int i = 0; i < count; i++)
    {
        float* elem = (float*)((char*)arr->m_data + i * 0x0C);
        store->ReadFloat(&elem[0]);
        store->ReadFloat(&elem[1]);
        store->ReadFloat(&elem[2]);
        if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
            break;
    }

    return store;
}

//----------------------------------------------------------------------------
//  0x6F3036C0 - the MovementModifier twin of
//  floatpropmodifierrefread.cpp's ReadFloatModifierRef; see that file for
//  the shape (same sentinel-less release-then-adopt structure, same
//  resolved-object +0x14/+0x18 handle pair).  Allocation tag/line come
//  straight off the dump's own truncated string
//  ("...\\engine\\source\\agile\\PositionModifier.h", line 0x49).
//----------------------------------------------------------------------------
static const char kMovementModifierAllocTag[] =
    "e:\\drive1\\temp\\buildwar3x\\engine\\source\\agile\\PositionModifier.h";

CDataStore* __fastcall ReadMovementModifierRef(CDataStore* store,
                                               MovementModifier** slot)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDword(&handle);
    store->ReadDword(&typeTag);

    if (handle == (unsigned int)-1)
        return store;

    MovementModifier* made = 0;
    void* raw = SMemAlloc(0x10, kMovementModifierAllocTag, 0x49, 0);
    if (raw != 0)
    {
        made = new (raw) MovementModifier();
        made->m_refcount += 1;
    }

    MovementModifier* old = *slot;
    if (old != 0)
    {
        if (--old->m_refcount == 0)
            ((void (__thiscall *)(void*))(*(void***)old)[0])(old);
    }
    *slot = made;

    SMadeModifier* resolved =
        (SMadeModifier*)LookupHandle(handle, (int)typeTag);
    if (resolved != 0)
    {
        made->m_handle  = resolved->m_handle;
        made->m_typeTag = resolved->m_typeTag;
    }
    return store;
}

//----------------------------------------------------------------------------
//  Save's own inline shape for every plain TRefCnt* modifier reference
//  (both of a SCameraProp's own slots, plus the five bare TRefCnt* fields
//  near +0x454): -1/-1 when null, else the referenced object's own
//  {handle, typeTag} pair at +0x08/+0x0C - FloatModifier's and
//  MovementModifier's own shape (floatpropmodifier.h/positionmodifier.h),
//  read here through a plain word array since the field itself is only
//  ever a bare TRefCnt* and never typed more specifically.  Inlined at
//  every one of its twenty-five call sites the way the shipped stream has
//  it (__forceinline, not a real out-of-line function).
//----------------------------------------------------------------------------
static __forceinline void WriteModifierHandlePair(CDataStoreScratch* store,
                                                   const TRefCnt* ref)
{
    const unsigned int* words = (const unsigned int*)ref;
    if (ref != 0)
        store->WriteDword(words[2]);
    else
        store->WriteDword(0xFFFFFFFFu);
    store->WriteDword(ref != 0 ? words[3] : 0xFFFFFFFFu);
}

//----------------------------------------------------------------------------
//  0x6F303790 - CCameraWar3::Save.  See the file header.
//----------------------------------------------------------------------------
void CCameraWar3::Save(CDataStoreScratch* store)
{
    CAgent::Save(store);

    store->WriteDword((unsigned int)m_presetIndex);
    store->WriteDwordAlt2((unsigned int)m_field0x28);
    store->WriteDword((unsigned int)m_controllerMode);
    WriteAgentHandlePairDupC(store, (CAgent**)&m_pPanJob);

    store->WriteFloat(m_offset.m_x);
    store->WriteFloat(m_offset.m_y);
    store->WriteFloat(m_offset.m_z);

    store->WriteDwordArray(m_rawBlock60, 9);

    {
        typedef void (__thiscall *TimerSaveFn)(CAgentTimer*, CDataStoreScratch*);
        void** vtbl = *(void***)&m_updateTimer;
        ((TimerSaveFn)vtbl[0x20 / 4])(&m_updateTimer, store);
    }

    store->WriteDwordAlt((unsigned int)m_cursorPresetPending);
    store->WriteDwordAlt((unsigned int)m_cursorPresetCount);
    WriteFilterSlotArray(store, &m_filterSlots);

    WriteModifierHandlePair(store, m_listenerDistance.m_pModifier);
    WriteModifierHandlePair(store, m_listenerAngle.m_pModifier);
    WriteModifierHandlePair(store, m_targetDistance.m_pModifier);
    WriteModifierHandlePair(store, m_farZ.m_pModifier);
    WriteModifierHandlePair(store, m_nearZ.m_pModifier);
    WriteModifierHandlePair(store, m_fieldOfView.m_pModifier);
    WriteModifierHandlePair(store, m_rotation.m_pModifier);
    WriteModifierHandlePair(store, m_angleOfAttack.m_pModifier);
    WriteModifierHandlePair(store, m_roll.m_pModifier);
    WriteModifierHandlePair(store, m_targetZ.m_pModifier);
    WriteModifierHandlePair(store, m_panZ.m_pModifier);
    WriteModifierHandlePair(store, m_listenerDistance.m_pModifier2);
    WriteModifierHandlePair(store, m_listenerAngle.m_pModifier2);
    WriteModifierHandlePair(store, m_targetDistance.m_pModifier2);
    WriteModifierHandlePair(store, m_farZ.m_pModifier2);
    WriteModifierHandlePair(store, m_nearZ.m_pModifier2);
    WriteModifierHandlePair(store, m_fieldOfView.m_pModifier2);
    WriteModifierHandlePair(store, m_rotation.m_pModifier2);
    WriteModifierHandlePair(store, m_angleOfAttack.m_pModifier2);
    WriteModifierHandlePair(store, m_roll.m_pModifier2);

    store->WriteByte(m_panActive != 0);

    WriteModifierHandlePair(store, m_pRampModifiers[0]);
    WriteModifierHandlePair(store, m_pOffsetModifiers[0]);
    WriteModifierHandlePair(store, m_pPanMovement);
    WriteModifierHandlePair(store, m_pRampModifiers[1]);
    WriteModifierHandlePair(store, m_pOffsetModifiers[1]);

    //  +0x4C4..+0x504 - sixteen floats in a row, walked as one flat run
    //  (matching, not the individually-named fields the header declares for
    //  ProjectGroundPoint's/the JASS natives' own sakes - the shipped
    //  compiler's own generated code advances one shared pointer across all
    //  sixteen rather than re-computing each field's own absolute address,
    //  which only happens for a genuinely flat array walk).
    {
        const float* raw = (const float*)((const char*)this + 0x4C4);
        store->WriteFloat(raw[0]);
        store->WriteFloat(raw[1]);
        store->WriteFloat(raw[2]);
        store->WriteFloat(raw[3]);
        store->WriteFloat(raw[4]);
        store->WriteFloat(raw[5]);
        store->WriteFloat(raw[6]);
        store->WriteFloat(raw[7]);
        store->WriteFloat(raw[8]);
        store->WriteFloat(raw[9]);
        store->WriteFloat(raw[10]);
        store->WriteFloat(raw[11]);
        store->WriteFloat(raw[12]);
        store->WriteFloat(raw[13]);
        store->WriteFloat(raw[14]);
        store->WriteFloat(raw[15]);
    }

    store->WriteDwordArray(m_rawBlock504, 9);
    store->WriteDwordArray(m_toBoundsSpace, 9);
    store->WriteDwordArray(m_fromBoundsSpace, 9);
    store->WriteDwordArray(m_sampleWeights, 9);
}

//----------------------------------------------------------------------------
//  0x6F308810 - CCameraWar3::Load.  See the file header.
//----------------------------------------------------------------------------
void CCameraWar3::Load(CDataStore* store)
{
    CAgent::Load(store);

    if (m_pDefaultController == 0)
    {
        m_pDefaultController = MakeCameraController();
        m_pController = m_pDefaultController;
    }
    if (m_pAltController == 0)
        m_pAltController = MakeCameraEventSink();

    store->ReadDword((unsigned int*)&m_presetIndex);
    store->ReadDwordAlt3((unsigned int*)&m_field0x28);

    unsigned int controllerMode = 0;
    store->ReadDword(&controllerMode);
    m_controllerMode = (int)controllerMode;
    ReadUnitRef(store, (SCheckedUnitSlot*)&m_pPanJob);

    store->ReadFloat(&m_offset.m_x);
    store->ReadFloat(&m_offset.m_y);
    store->ReadFloat(&m_offset.m_z);

    store->ReadDwordArray(m_rawBlock60, 9);

    {
        typedef void (__thiscall *TimerLoadFn)(CAgentTimer*, CDataStore*);
        void** vtbl = *(void***)&m_updateTimer;
        ((TimerLoadFn)vtbl[0x24 / 4])(&m_updateTimer, store);
    }

    store->ReadDwordAlt((unsigned int*)&m_cursorPresetPending);
    store->ReadDwordAlt((unsigned int*)&m_cursorPresetCount);
    ReadFilterSlotArray(store, &m_filterSlots);

    ReadFloatModifierRef(store, (FloatModifier**)&m_listenerDistance.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_listenerAngle.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_targetDistance.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_farZ.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_nearZ.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_fieldOfView.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_rotation.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_angleOfAttack.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_roll.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_targetZ.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_panZ.m_pModifier);
    ReadFloatModifierRef(store, (FloatModifier**)&m_listenerDistance.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_listenerAngle.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_targetDistance.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_farZ.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_nearZ.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_fieldOfView.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_rotation.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_angleOfAttack.m_pModifier2);
    ReadFloatModifierRef(store, (FloatModifier**)&m_roll.m_pModifier2);

    unsigned char panActive = 0;
    store->ReadByte(&panActive);
    m_panActive = panActive;

    ReadMovementModifierRef(store, (MovementModifier**)&m_pRampModifiers[0]);
    ReadMovementModifierRef(store, (MovementModifier**)&m_pOffsetModifiers[0]);
    ReadMovementModifierRef(store, (MovementModifier**)&m_pPanMovement);
    ReadMovementModifierRef(store, (MovementModifier**)&m_pRampModifiers[1]);
    ReadMovementModifierRef(store, (MovementModifier**)&m_pOffsetModifiers[1]);

    //  +0x4C4..+0x504 - see CCameraWar3::Save's own note above.
    {
        float* raw = (float*)((char*)this + 0x4C4);
        store->ReadFloat(&raw[0]);
        store->ReadFloat(&raw[1]);
        store->ReadFloat(&raw[2]);
        store->ReadFloat(&raw[3]);
        store->ReadFloat(&raw[4]);
        store->ReadFloat(&raw[5]);
        store->ReadFloat(&raw[6]);
        store->ReadFloat(&raw[7]);
        store->ReadFloat(&raw[8]);
        store->ReadFloat(&raw[9]);
        store->ReadFloat(&raw[10]);
        store->ReadFloat(&raw[11]);
        store->ReadFloat(&raw[12]);
        store->ReadFloat(&raw[13]);
        store->ReadFloat(&raw[14]);
        store->ReadFloat(&raw[15]);
    }

    store->ReadDwordArray(m_rawBlock504, 9);
    store->ReadDwordArray(m_toBoundsSpace, 9);
    store->ReadDwordArray(m_fromBoundsSpace, 9);
    store->ReadDwordArray(m_sampleWeights, 9);

    BindControllerProps();
}
