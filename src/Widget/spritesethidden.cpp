//============================================================================
//  0x6F4D31D0 - push the "hidden" flag at the sprite's own model instance.
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is eleven instructions.
//  `visible` really does arrive in edx (`test edx,edx` before anything is
//  read off the stack), so the __fastcall declaration its caller
//  (widget_spritevisible.cpp) was already compiled against is right.
//
//  The model instance is fetched *once*, before either branch, and both
//  branches tail-jump through the same register - so the null test, the
//  flag write and the notify are three statements over one local, not two
//  independent paths that each re-read +0x20.
//
//  The flag is 0x80000 of the sprite's own +0x28, set when `visible` is
//  non-zero and cleared when it is zero.  Reading the parameter as
//  "hidden" rather than "visible" would inverted the two branches; the name
//  is kept as the callers already have it and this comment carries the
//  polarity instead.
//
//  Own translation unit: the notify below is reached with a real tail jump.
//============================================================================

//  0x6F50D8C0 - re-push the model instance's own state at whatever is
//  drawing it: a 60-instruction walk over a 12-byte-stride array bounded by
//  the instance's +0xC4, through a vtable slot per element.  Rendering-side
//  and out of scope; `retn` with nothing on the stack, so a thiscall
//  declaration with no arguments pushes exactly what it cleans.
struct SModelInstanceNotify
{
    void __thiscall RefreshDrawState();
};

__declspec(naked) void __thiscall SModelInstanceNotify::RefreshDrawState()
{
    __asm
    {
        mov     eax, 06F50D8C0h
        jmp     eax
    }
}

class CSprite;   //  `class`, not `struct`: MSVC mangles the tag into the
                 //  name, and `struct` here yields ?SpriteSetHidden@@YIXPAUCSprite@@H@Z
                 //  where sprite.h's `class` yields ...PAVCSprite... - two symbols that
                 //  never meet at link time.  See
                 //  docs/notes/verifier-gate-link-failures.md.

struct SSpriteHiddenFlag
{
    char                  m_reserved00[0x20];
    SModelInstanceNotify* m_pModel;     // +0x20
    char                  m_reserved24[4];
    unsigned int          m_drawFlags;  // +0x28
};

void __fastcall SpriteSetHidden(CSprite* sprite, int visible)
{
    SSpriteHiddenFlag* self = (SSpriteHiddenFlag*)sprite;

    SModelInstanceNotify* model = self->m_pModel;
    if (model == 0)
        return;

    if (visible != 0)
    {
        self->m_drawFlags |= 0x80000;
        return model->RefreshDrawState();
    }

    self->m_drawFlags &= ~0x80000u;
    return model->RefreshDrawState();
}

//----------------------------------------------------------------------------
//  0x6F4D3500 / 0x6F4D3510 - two more one-shot notifiers, same shape as
//  SpriteSetHidden above (fetch the sprite's own +0x20 model pointer once,
//  null-guard it, tail into a real probe on modelinstancenotify.cpp's
//  object) but pushing a caller-supplied flag through instead of a literal,
//  and reached from CUnit::RefreshSpriteVisibility's slot-61 override
//  (unit_refreshspritevisibility.cpp) rather than from SetSpriteVisible.
//
//  Both `__fastcall(CSprite*, int)`: `flag` really does arrive in edx before
//  either function reads anything off the stack, matching every other probe
//  on this object family.
//----------------------------------------------------------------------------
extern void __fastcall SModelInstance_PropagateAttachmentFlag_6F502BD0(void*, int);
extern void __fastcall SModelInstance_PropagateFlagRecursive_6F507CE0(void*, int, int);

void __fastcall NotifyModelAttachmentFlag_6F4D3500(CSprite* sprite, int flag)
{
    void* model = ((SSpriteHiddenFlag*)sprite)->m_pModel;
    if (model != 0)
        SModelInstance_PropagateAttachmentFlag_6F502BD0(model, flag);
}

//  Tail-calls PropagateFlagRecursive_6F507CE0 with includeChildren forced
//  to 1 - the literal `push 1` in the dump, not a third argument of this
//  function's own.
void __fastcall NotifyModelTreeFlag_6F4D3510(CSprite* sprite, int flag)
{
    void* model = ((SSpriteHiddenFlag*)sprite)->m_pModel;
    if (model != 0)
        SModelInstance_PropagateFlagRecursive_6F507CE0(model, flag, 1);
}
