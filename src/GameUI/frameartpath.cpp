//============================================================================
//  0x6F3228E0 - ApplyArtPathUnlessModel: hand a path on only when it is
//  *not* a model.
//
//  The test is "do the last four characters equal '.mdl'", spelled as
//  SStrLen followed by a bounded case-insensitive compare four bytes back
//  from the end - and the branch keeps the path when that compare comes
//  back **non-zero**, i.e. when the tail is *not* ".mdl".  So this is the
//  filter on an *art* path: its one caller (0x6F3208C0) is the button-art
//  setup that reads "CommandButtonDisabledArtPath" out of the skin tables,
//  and a model path is what it declines to treat as a texture.  The sense
//  is read off `test eax,eax` / `jz` at 0x6F322904, not inferred from the
//  name of the constant.
//
//  The two constant arguments of that compare are pushed *before* the
//  length call in the shipped stream - both calls' outgoing slots are the
//  same contiguous run, so MSVC fills the ones it already knows first.
//  That is scheduling, not two different calls sharing a frame.
//
//  A null path does nothing at all: the assignment is a tail jump reached
//  only when both tests pass.
//
//  Own translation unit: three real calls.
//============================================================================
#include "config.h"     // SStrnICmp, and the Storm-import declaration style
#include "frame.h"      // CSimpleButton::SetSlotTexture, 0x6F603220

//  Storm ordinal 506 - string length.  Declared as ordinary C++ rather than
//  extern "C", like config.h's own two: the pipeline recovers readable names
//  from the comment MASM writes beside a mangled EXTRN.
unsigned int __stdcall Storm_506(const char* s);

//  Storm ordinal 572 - find the last occurrence of a character.
const char* __fastcall Storm_572(const char* text, char ch);
//  Storm ordinal 578 - SStrPrintf-family bounded formatter.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  0x6F31F530 - GameUI/gameuiskinvalue.cpp.
const char* __fastcall ResolveSkinValue(const char* key, const char* skin);

//  0x6F3208C0 - put the art at `path` on the frame.  `retn 0`, both
//  arguments in registers.
//
//  CItem vtable-closure batch 2.  `frame` is really a CSimpleButton*
//  (0x6F603220 - Frame/buttonslottexture.cpp - is CSimpleButton::
//  SetSlotTexture(int slot, const char*)): slot 1 gets `path` itself, and
//  slot 0 gets a synthesized "<CommandButtonDisabledArtPath skin
//  value>DIS<filename>" path built from the part of `path` after its last
//  path separator (Storm_572 tried backslash then forward slash, in that
//  order - a `path` with neither separator uses the whole string).  Either
//  slot falls back to ResolveSkinValue("DefaultButton", 0) when
//  SetSlotTexture reports failure (a null-returning texture load).
//
//  The shipped body carries a 0x108-byte-frame `/GS` stack cookie
//  (`dword_6FAAE140 ^ esp`, checked via `sub_6F7E1059` at the tail) around
//  the 0x104-byte local path buffer - see docs/msvc-vc8-idioms.md's "An
//  __except_handler4-shaped frame this toolchain cannot reproduce" note:
//  this repo's toolchain is fixed at `/GS-`, so a local buffer this large
//  never earns a cookie check here no matter how the source is spelled.
//  DIFFERS for exactly that (no `__try`/`__finally` involved, so it is not
//  the SEH-frame case that note documents, just the plain-cookie half it
//  says is *also* unreachable at `/GS-`) - the logic past the missing
//  prologue/epilogue cookie matches instruction for instruction.
void __fastcall ApplyFrameArtPath(void* frame, const char* path);

void __fastcall ApplyArtPathUnlessModel(void* frame, const char* path)
{
    if (path != 0 &&
        SStrnICmp(path + Storm_506(path) - 4, ".mdl", 0x7FFFFFFF) != 0)
        ApplyFrameArtPath(frame, path);
}

void __fastcall ApplyFrameArtPath(void* frame, const char* path)
{
    CSimpleButton* button = (CSimpleButton*)frame;

    if (button == 0 || path == 0)
        return;

    if (!button->SetSlotTexture(1, path))
        button->SetSlotTexture(1, ResolveSkinValue("DefaultButton", 0));

    const char* separator = Storm_572(path, '\\');
    if (separator == 0)
        separator = Storm_572(path, '/');
    const char* name = (separator != 0) ? separator + 1 : path;

    const char* disabledPrefix = ResolveSkinValue("CommandButtonDisabledArtPath", 0);

    char disabledPath[0x104];
    Storm_578(disabledPath, 0x104, "%sDIS%s", disabledPrefix, name);

    if (!button->SetSlotTexture(0, disabledPath))
        button->SetSlotTexture(0, ResolveSkinValue("DefaultButton", 0));
}
