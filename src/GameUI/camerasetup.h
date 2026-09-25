//============================================================================
//  CCameraSetup : public CAgentWar3 - RTTI ground truth
//  (user_knowledge.json's own vtables table, vtable @ 0x6F94C10C, 30 slots).
//
//  **Not a CCameraWar3 subclass**, despite sharing its first 24 slots'
//  order and count: the "genuinely inherited, unchanged" slots among those
//  24 name `CAgentWar3::GetRecordVersion` (slot 10, 0x6F001AE0) rather than
//  CAgent's own default CCameraWar3 uses there, and slots 24-29
//  (`CAgentWar3::SetFlags`/`Method_0x64`/`Method_0x68`/`Method_0x6C`/
//  `Method_0x70`/`DumpState`-family) exist at all only because CAgentWar3
//  itself contributes them - CCameraWar3 has no slots past 24 to compare
//  against.  The destructor tail (0x6F3E4DB0, this file's own
//  ~CCameraSetup) settles it outright: it stamps `??_7CAgentWar3@@6B@`,
//  not `??_7CCameraWar3@@6B@`, at +0, and CAgentWar3 is exactly 0x24 bytes
//  (agentwar3.h) - which is exactly where this class's own first
//  registered property sits.  So the two classes are siblings under
//  CAgentWar3, not parent and child, and this file declares CCameraSetup's
//  own +0x24-based layout rather than reusing gameui.h's CCameraWar3 one.
//
//  17 properties (SUnitTrackedRef, the same "vtable pointer + refcount +
//  handle + typeTag" shape gameui.h's SCameraProp wraps, but bare here -
//  0x10 apart, not 0x18: nothing in either dump reads a second modifier
//  slot the way CCameraWar3's own nine do) at +0x24 through +0x94, then a
//  0xC-byte gap of three raw floats Save/Load read and write directly
//  (+0xA4/+0xA8/+0xAC - CDataStoreScratch::WriteRaw's own CFloat calls,
//  0x6F6EFE00/0x6F6EED50, already reconstructed under those names), then
//  nine more properties at +0xB0 through +0x130.  Only the touched range is
//  declared; nothing past +0x134 is established by either dump.
//============================================================================
#ifndef CAMERASETUP_H
#define CAMERASETUP_H

#include "agentwar3.h"
#include "unittrackedref.h"

class CDataStoreScratch;
class CDataStore;

class CCameraSetup : public CAgentWar3
{
public:
    //  slot 1 / +0x04 (0x6F3EAD80) - overrides CAgentWar3::DeleteSelf.
    virtual CAgentWar3* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F3DA5A0) - overrides CAgentWar3::GetAgileTypeId.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F3DA5C0) - overrides CAgent::Method_0x20.
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F3DA8B0) - overrides CAgent::Method_0x24.
    virtual int Method_0x24(int context);
    //  slot 14 / +0x38 (0x6F3E45C0) - overrides CAgentWar3::Save.
    virtual void Save(CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F3E4600) - overrides CAgentWar3::Load.
    virtual void Load(CDataStore* store);
    //  slot 22 / +0x58 (0x6F3DA550) - overrides CAgentWar3::GetClassName.
    virtual const char* GetClassName();

    //  0x6F3E4DB0 - the destructor tail: reset all seventeen properties'
    //  vtable pointers to plain TRefCnt, then re-stamp this object's own
    //  vtable to CAgentWar3's before whatever comes after (this call tree
    //  never reaches further than that one tail jump).
    ~CCameraSetup();

    //  +0x20 - CAgentWar3::m_flags (agentwar3.h).  +0x24 is this class's own
    //  first field.
    SUnitTrackedRef m_prop0;     // +0x24
    SUnitTrackedRef m_prop1;     // +0x34
    SUnitTrackedRef m_prop2;     // +0x44
    SUnitTrackedRef m_prop3;     // +0x54
    SUnitTrackedRef m_prop4;     // +0x64
    SUnitTrackedRef m_prop5;     // +0x74
    SUnitTrackedRef m_prop6;     // +0x84
    SUnitTrackedRef m_prop7;     // +0x94
    //  +0xA4..+0xB0 - three raw CFloats Save/Load carry directly, with no
    //  registration call of their own.
    float           m_saveFloat0;   // +0xA4
    float           m_saveFloat1;   // +0xA8
    float           m_saveFloat2;   // +0xAC
    SUnitTrackedRef m_prop8;     // +0xB0
    SUnitTrackedRef m_prop9;     // +0xC0
    SUnitTrackedRef m_prop10;    // +0xD0
    SUnitTrackedRef m_prop11;    // +0xE0
    SUnitTrackedRef m_prop12;    // +0xF0
    SUnitTrackedRef m_prop13;    // +0x100
    SUnitTrackedRef m_prop14;    // +0x110
    SUnitTrackedRef m_prop15;    // +0x120
    SUnitTrackedRef m_prop16;    // +0x130
};

#endif
