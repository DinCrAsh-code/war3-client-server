//============================================================================
//  0x6F395DC0 and 0x6F395EB0 - the cursor's mode array and the state machine
//  that drives it.  Neither calls the other.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "frame.h"
#include "storm.h"          // SMemFree - Storm ordinal 403
#include "handleobject.h"   // GetHandleOrZeroAlias
#include "worldframecursormodearray.h"

class CSprite;

//  0x6F4D7E50 - Widget/widget_spriteplay.cpp's redirect, under the same name
//  and the same signature so both sides reach the one symbol.
float __fastcall SpriteStartAnimation(CSprite* sprite,
                                      const unsigned int* name,
                                      int flags, int extra);
//  0x6F60EDE0 - Misc/trivialpredicates.cpp.
int __fastcall GetIndirectHead_6F60EDE0(const void* self);
//  0x6F4C40A0 - Agent/refcnt.h's TRefCnt::Release, declared where this file
//  can reach it without pulling the whole class in.
void __fastcall ReleaseTextureRef(void* ref);

__declspec(naked) void __fastcall ReleaseTextureRef(void*)
{
    __asm
    {
        mov     eax, 06F4C40A0h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F395DC0 - drop the cursor's mode array.
//
//  The string handed to Storm in its `__FILE__` slot is the RTTI descriptor
//  name of `enum CursorMode` (`??_R0?AW4CursorMode@@@8`, 0x6FA78F10) - what
//  the whole binary passes there for a templated container.  It is spelled
//  as a file-scope constant and not given a DATA name, for the reason
//  docs/msvc-vc8-idioms.md gives: the same descriptor is shared with every
//  other instantiation over the same element type, and naming a shared
//  constant can only break matches elsewhere.
//----------------------------------------------------------------------------
static const char kCursorModeTypeName[] = ".?AW4CursorMode@@";

void CWorldFrameWar3::ReleaseCursorModes()
{
    void* modes = m_pModes;

    if (modes != 0)
        SMemFree(modes, kCursorModeTypeName, -2, 0);

    m_modeCount = 0;
    m_modeCapacity = 0;
    m_pModes = 0;
}

//----------------------------------------------------------------------------
//  0x6F395D60 - push a cursor mode onto the stack at +0x1B0: `explicitMode`
//  when `useExplicit` is set, the live m_cursorMode otherwise.  Skipped
//  when the stack already holds the same value on top and neither
//  `force` nor an unsuppressed change asks for a duplicate - the shipped
//  test is `force || (!m_suppressed && mode != top)`, matching the
//  `jnz push` / `jnz skip` / `jz skip` chain exactly.
//----------------------------------------------------------------------------
void CWorldFrameWar3::PushCursorMode(int force, int useExplicit,
                                      int explicitMode)
{
    int mode = useExplicit ? explicitMode : m_cursorMode;

    TSGrowableArray<CursorMode>* modes = GetCursorModeArray(this);
    int top = modes->m_count ? (int)modes->m_data[modes->m_count - 1] : 0;

    if (force || (!m_suppressed && mode != top))
        *modes->Grow() = (CursorMode)mode;
}

//----------------------------------------------------------------------------
//  0x6F395EB0 - put the cursor into one of eighteen states.
//
//  Every state ends up running one animation token list on the cursor's own
//  sprite; what differs is the list.  States 0-8 run a single token, 9 runs
//  one and then swaps the cursor's texture, and 10-17 run two or three -
//  which is why the shipped code has *two* jump tables, an outer one over
//  the eighteen states and an inner one over `state - 10`.
//
//  Neither table's data words come back: MSVC lays its own out inside the
//  PROC and IDA lists them as separate `jpt_` items the dump does not carry
//  at all, so the two streams differ by whatever the case bodies were
//  ordered into.  See docs/msvc-vc8-idioms.md, "A jump-table switch's own
//  data table shows up on one side only".
//
//  Own translation unit: eight real calls out of it, one of them
//  (0x6F393040) in a different module of the same subsystem.
//----------------------------------------------------------------------------
void CWorldFrameWar3::SetCursorState(int state, void* texture, int flags)
{
    void* render = ((CScreenFrame*)GetGameUI(1, 0))->EnsureRenderObject(0);

    if (render == 0)
        return;

    CSprite* sprite = (CSprite*)GetIndirectHead_6F60EDE0(render);

    if (sprite == 0)
        return;

    if (m_suppressed != 0)
        return;

    unsigned int token;
    unsigned int tokens[3];

    switch (state)
    {
    case 10:
        tokens[0] = 6;
        tokens[1] = 7;
        SpriteStartAnimation(sprite, tokens, 2, 0);
        break;

    case 11:
        tokens[0] = 6;
        tokens[1] = 8;
        SpriteStartAnimation(sprite, tokens, 2, 0);
        break;

    case 12:
        tokens[0] = 6;
        tokens[1] = 9;
        SpriteStartAnimation(sprite, tokens, 2, 0);
        break;

    case 13:
        tokens[0] = 6;
        tokens[1] = 10;
        SpriteStartAnimation(sprite, tokens, 2, 0);
        break;

    case 14:
        tokens[0] = 6;
        tokens[1] = 9;
        tokens[2] = 7;
        SpriteStartAnimation(sprite, tokens, 3, 0);
        break;

    case 15:
        tokens[0] = 6;
        tokens[1] = 9;
        tokens[2] = 8;
        SpriteStartAnimation(sprite, tokens, 3, 0);
        break;

    case 16:
        tokens[0] = 6;
        tokens[1] = 10;
        tokens[2] = 7;
        SpriteStartAnimation(sprite, tokens, 3, 0);
        break;

    case 17:
        tokens[0] = 6;
        tokens[1] = 10;
        tokens[2] = 8;
        SpriteStartAnimation(sprite, tokens, 3, 0);
        break;

    case 1:
    case 2:
    case 3:
        token = 1;
        SpriteStartAnimation(sprite, &token, 1, 0);
        break;

    case 4:
        token = 2;
        SpriteStartAnimation(sprite, &token, 1, 0);
        break;

    case 5:
    case 6:
    case 7:
    case 8:
        token = 3;
        SpriteStartAnimation(sprite, &token, 1, 0);
        break;

    case 0:
        ResetTrack();
        token = 0;
        SpriteStartAnimation(sprite, &token, 1, 0);
        break;

    case 9:
        {
            token = 5;
            SpriteStartAnimation(sprite, &token, 1, 0);

            const char* path = (const char*)texture;
            CTexture* loaded = LoadTextureByPath(path, 0, 0, 1);

            ((SPathField*)&m_texturePath)->Set(path);

            if (loaded != 0)
            {
                SpriteSetTexture(sprite, loaded, 0x15);
                ReleaseTextureRef(loaded);
            }
        }
        break;
    }

    m_cursorMode = state;

    ApplyCursorColor();
}

//----------------------------------------------------------------------------
//  0x6F398F20 - pop the top of the +0x1B0 stack back into effect: `flags=1`
//  is a compile-time constant with nothing else to wait on, so the shipped
//  stream pushes it for the tail SetCursorState call before it ever calls
//  GetHandleOrZeroAlias for the texture argument - which is what a single
//  call expression's right-to-left argument evaluation already produces
//  here, with no separate local needed for either argument.
//----------------------------------------------------------------------------
void CWorldFrameWar3::PopCursorMode()
{
    if (m_suppressed)
        return;

    TSGrowableArray<CursorMode>* modes = GetCursorModeArray(this);
    if (modes->m_count == 0)
        return;

    int mode = (int)modes->m_data[modes->m_count - 1];

    SetCursorState(mode,
        (void*)GetHandleOrZeroAlias(
            (const SHandleHolder*)((char*)this + 0x1C8)),
        1);

    modes->SetCount(modes->m_count - 1);
}
