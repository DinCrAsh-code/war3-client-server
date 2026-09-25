//============================================================================
//  The small CMissileFire vtable overrides that are single-purpose leaves
//  too small to earn their own translation unit: the FourCC constant, the
//  GetRecordVersion additive constant, the class name, and the new
//  slot-50 override (Method_0xC8).  None of these four addresses appears
//  in any asm/*.md dump; each is a direct, instruction-by-instruction
//  transcription of its own agent_worktrees raw_asm.
//============================================================================
#include "missilefire.h"

//----------------------------------------------------------------------------
//  0x6F0DBB30 - vtable slot 7.  A bare FourCC-shaped constant, same shape
//  as every other level of this family.
//----------------------------------------------------------------------------
unsigned int CMissileFire::GetAgileTypeId() const
{
    return 0x4D666972u;
}

//----------------------------------------------------------------------------
//  0x6F0F66F0 - vtable slot 10, GetRecordVersion.  A direct (non-virtual)
//  call straight to the shared CAgent::GetRecordVersion leaf (agent.h,
//  "always 0") - not a chained CMissile::GetRecordVersion() (which itself
//  adds 14 over the same leaf) - plus eighteen at this level.
//----------------------------------------------------------------------------
int CMissileFire::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 0x12;
}

//----------------------------------------------------------------------------
//  0x6F0DBAE0 - vtable slot 22.  "CMissileFire", the untruncated string
//  per agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CMissileFire::GetClassName()
{
    return "CMissileFire";
}

//----------------------------------------------------------------------------
//  0x6F0E4C50 - vtable slot 50, Method_0xC8.  Genuinely new past
//  CMissile's own 50 slots.
//
//  Resolves an answer through arg0's own +0x38 sub-object (a real virtual
//  call on *that* object - vtable slot 0xEC/4 = 59, a different, unrelated
//  class's vtable reached only by byte offset, the same "punned raw
//  vtable slot on someone else's object" idiom missile_core.cpp's own
//  GetImpactApplyFlag override already uses), or -1 if that sub-object is
//  null, into m_fieldAC; copies `value` straight into m_fieldB0; assigns
//  `floatF`/`floatG` - both `const CFloat*`, forwarded straight through
//  from this function's own caller rather than freshly taken addresses,
//  confirmed by the raw disassembly pushing the incoming stack arguments
//  unchanged - into m_floatF/m_floatG through their own vtable-slot-0
//  "assign and notify" (same idiom missile_dumpflags.cpp's own LoadFlags
//  uses); then tail-calls a second deep, out-of-depth-budget leaf
//  (0x6F2C3F70 - real floating-point work through several more virtual
//  calls on arg0's own sub-objects, several levels past this session's
//  depth budget) with arg0.  Best-effort: nothing in this dump's own
//  closure names what arg0 or its +0x38 sub-object actually are.
//----------------------------------------------------------------------------
typedef int  (__thiscall *VSlot0xECFn)(void* self);
typedef void (__thiscall *FloatMiniAssignFn)(void*, const void*, int);

//  0x6F2C3F70 - deep leaf: several virtual calls through arg0's own
//  +0x90/+0x98/+0x6C sub-objects plus real CFloat/FPU arithmetic, several
//  levels past this session's depth budget.  Naked redirect to the real,
//  unhooked shipped body, same convention CMissile.md's own deep
//  impact-reconfirmation closures use.
struct SMissileFireDeepReport
{
    void __thiscall Report(void* arg0);
};

__declspec(naked) void __thiscall SMissileFireDeepReport::Report(void*)
{
    __asm { mov eax, 0x6F2C3F70 }
    __asm { jmp eax }
}

void CMissileFire::Method_0xC8(void* arg0, unsigned int value,
                                const CFloat* floatF, const CFloat* floatG)
{
    void* sub = *(void**)((char*)arg0 + 0x38);
    m_fieldAC = sub ? ((VSlot0xECFn)(*(void***)sub)[0xEC / 4])(sub) : -1;
    m_fieldB0 = (int)value;

    ((FloatMiniAssignFn)(*(void***)&m_floatF)[0])(&m_floatF, floatF, 1);
    ((FloatMiniAssignFn)(*(void***)&m_floatG)[0])(&m_floatG, floatG, 1);

    ((SMissileFireDeepReport*)this)->Report(arg0);
}
