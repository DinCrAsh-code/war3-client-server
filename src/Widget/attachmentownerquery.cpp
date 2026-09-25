//============================================================================
//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner: the owning object,
//  resolved the slow way when SAttachmentTarget::Show finds its +0x30
//  empty (attachmentcounters.cpp).  A naked redirect there until now.
//
//  Resolve the {handle, typeTag} pair at +0x0C/+0x10, insist that what
//  comes back carries the 'lga+' tag at its own +0x0C, then take its +0x30
//  and hand back that object's +0x54 - but only if its +0x20 is clear.
//
//  Two shapes worth stating:
//
//  * **the type check is branchless.**  `xor ecx,ecx` / `cmp` / `setnz cl`
//    / `sub ecx,1` / `and eax,ecx` is a mask, not a jump: the predicate is
//    0 or 1, minus one makes it 0 or ~0, and the AND keeps or clears the
//    pointer.  Written as `if (tag != 'lga+') obj = 0;` this compiler emits
//    a conditional jump instead and the five instructions become three in
//    the wrong shape.
//  * **the +0x30 load is unguarded.**  It happens on the null path too -
//    `mov eax, [eax+30h]` is the join of both arms - so a failed resolve
//    faults here.  That is the shipped code; the guard is upstream in the
//    caller, which only reaches this when it has a live attachment.
//
//  Own translation unit: attachmentcounters.cpp must reach it with a real
//  call.
//============================================================================

//  handle.cpp's, under its real name.
class CHandleObject;
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//  The block the handle names.  'lga+' at +0x0C is the same four-character
//  self-identifying tag SWidgetAgentQuery's own first word carries
//  (widgetagentquery.h).
struct SAttachmentOwnerBlock
{
    char         m_reserved00[0x0C];
    unsigned int m_tag;         // +0x0C - 'lga+'
    char         m_reserved10[0x20];
    void*        m_pOwner;      // +0x30
};

//  ... and what that +0x30 points at.  +0x20 has to be clear for +0x54 to
//  be handed out; nothing here says what either means.
struct SAttachmentOwnerRecord
{
    char  m_reserved00[0x20];
    int   m_gate;               // +0x20
    char  m_reserved24[0x30];
    void* m_pValue;             // +0x54
};

struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();

    char         m_reserved00[0x0C];
    unsigned int m_handle;      // +0x0C
    int          m_typeTag;     // +0x10
};

void* __thiscall SAttachmentOwnerQuery::QueryOwner()
{
    SAttachmentOwnerBlock* block =
        (SAttachmentOwnerBlock*)LookupHandle(m_handle, m_typeTag);
    if (block != 0)
    {
        //  See the note above: a mask, not a branch.
        block = (SAttachmentOwnerBlock*)((unsigned int)block &
                    (unsigned int)((block->m_tag != 0x2B61676Cu) - 1));
    }

    SAttachmentOwnerRecord* owner = (SAttachmentOwnerRecord*)block->m_pOwner;
    if (owner != 0 && owner->m_gate == 0)
        return owner->m_pValue;
    return 0;
}
