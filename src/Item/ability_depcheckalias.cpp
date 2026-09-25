//============================================================================
//  0x6F052C30 - CAbility::EvaluateDepCheckAlias, slot 154 - previously
//  pending ("a two-level jagged-array 'DepCheckAlias' walk plus one
//  callee with no dump"; both dumped fresh for this session).
//
//  Resolves this ability's own owner through LookupHandle(m_handle,
//  m_typeTag) - the exact same {handle,typeTag}-then-'lga+'-tag-check
//  idiom SAttachmentOwnerQuery::QueryOwner already establishes
//  (attachmentownerquery.cpp), branchless mask included - then, if the
//  owner's own +0x20 gate is clear, asks two per-derived-class vtable
//  hooks (+0x1E0/+0x1E4) whether to short-circuit yes.  Failing both, it
//  walks the "Misc"/"DepCheckAlias" config-driven jagged array
//  (sub_6F40FB70, out of scope for this batch - no dump anywhere in this
//  repo, declared as a naked redirect) against this ability's own owner
//  and type, and finally checks one net-data thread-local flag
//  (IsNetDataFlagSet, Game/gamecontextflag.cpp) as a last-chance yes.
//
//  sub_6F40B490/sub_6F40FB70 are declared, not reconstructed: neither has
//  a dump in this repo, and their real `retn` convention is UNCONFIRMED -
//  read straight off this call tree's own push counts (one stack arg for
//  0x6F40B490, three for 0x6F40FB70, both `this` in ecx) rather than
//  verified against a real listing.  Do not arm this hook without
//  confirming both first (tools/ida_query.py func_at + a bytes read of
//  the tail, per thunk_abi_audit.py's own "no dump body" guidance).
//============================================================================
#include "ability.h"
#include "game.h"

//  Config/configint.cpp - already real.
extern int __fastcall GetConfigInt(const char* section, const char* key, int index);

//  Game/gamecontextflag.cpp.
extern int __fastcall IsNetDataFlagSet(unsigned int mask);

//  attachmentownerquery.cpp's own block/record shapes, redeclared here:
//  this call tree reads the identical two fields off the identical kind
//  of object through the identical branchless mask, but starting from
//  this ability's own {m_handle, m_typeTag} (agent.h) rather than a
//  SAttachmentOwnerQuery's.
struct SAttachmentOwnerBlock
{
    char         m_reserved00[0x0C];
    unsigned int m_tag;         // +0x0C - 'lga+'
    char         m_reserved10[0x20];
    void*        m_pOwner;      // +0x30
};

struct SAttachmentOwnerRecord
{
    char  m_reserved00[0x20];
    int   m_gate;               // +0x20
};

typedef int (__thiscall *AbilityGateFn)(void*);

//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner, already a real body
//  (Widget/attachmentownerquery.cpp).  Redeclared here on that same real
//  class name (not a stand-in - a member-function mangled name encodes
//  its class, so only an exact class-name match binds to the same
//  symbol; an earlier draft here used a different stand-in class name,
//  which produced a *different*, undefined symbol).
struct SAttachmentOwnerQuery { void* __thiscall QueryOwner(); };

//  0x6F2834E0 - already a real body (Unit/unit_visibilitymask.cpp),
//  reconstructed there as a free `__fastcall` function, not a member -
//  called directly below rather than through a stand-in member wrapper.
void* __fastcall UnitVisibilityMaskOwner_6F2834E0(void* self);

//  0x6F40B490/0x6F40FB70 - out of scope for this batch, see header.
class SDepCheckAliasWalker
{
public:
    void* __thiscall ResolveFirstStep(int typeId);
    void* __thiscall WalkSteps(int typeId, int zero, int* outCount, int startOffset);
};
__declspec(naked) void* __thiscall SDepCheckAliasWalker::ResolveFirstStep(int)
{
    __asm { mov eax, 0x6F40B490 }
    __asm { jmp eax }
}
__declspec(naked) void* __thiscall SDepCheckAliasWalker::WalkSteps(int, int, int*, int)
{
    __asm { mov eax, 0x6F40FB70 }
    __asm { jmp eax }
}

int CAbility::EvaluateDepCheckAlias(int* outFlag)
{
    SAttachmentOwnerBlock* block =
        (SAttachmentOwnerBlock*)LookupHandle(m_handle, m_typeTag);
    if (block != 0)
    {
        //  A mask, not a branch - see attachmentownerquery.cpp's own note.
        block = (SAttachmentOwnerBlock*)((unsigned int)block &
                    (unsigned int)((block->m_tag != 0x2B61676Cu) - 1));
    }
    if (block == 0)
        return 0;

    SAttachmentOwnerRecord* owner = (SAttachmentOwnerRecord*)block;
    if (owner->m_gate != 0)
        return 0;

    if (((AbilityGateFn)(*(void***)this)[0x1E0 / 4])(this) != 0)
        return 1;
    if (((AbilityGateFn)(*(void***)this)[0x1E4 / 4])(this) != 0)
        return 1;

    if (m_field54 == 0)
        return 1;

    void* target = (void*)m_field30;
    if (target == 0)
        target = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    void* visibleObject = UnitVisibilityMaskOwner_6F2834E0(target);

    int typeId;
    if (GetConfigInt("Misc", "DepCheckAlias", 0) != 0)
        typeId = (int)m_typeId;
    else
        typeId = (int)((AbilityGateFn)(*(void***)this)[0x1C / 4])(this);

    SDepCheckAliasWalker* walker = (SDepCheckAliasWalker*)visibleObject;
    void* firstStep = walker->ResolveFirstStep(typeId);

    int count = 0;
    int startOffset = *(int*)((char*)m_field54 + 0x28);
    void* secondStep = walker->WalkSteps(typeId, 0, &count, startOffset);

    if (outFlag != 0)
    {
        int both = (firstStep != 0 && count != 0) ? 1 : 0;
        *outFlag = both;
    }

    if (firstStep == 0)
        return 0;
    if (secondStep != 0)
        return 1;
    if (count == 0)
        return 0;

    if (IsNetDataFlagSet(0x100) != 0)
        return 1;

    return 0;
}
