//============================================================================
//  0x6F35B380 - CUnitTip::Construct.
//
//  The class name is the shipped code's own: this is the function that
//  stamps `??_7CUnitTip@@6B@`.  It builds the frame base, the empty
//  selectable slot the tip tracks, the backdrop, and three font strings -
//  the player name, a second line pinned under it, and the description
//  pinned under that.
//
//  Every one of the four allocations is the same shape: SMemAlloc with this
//  module's own `__FILE__`/line tag, construct in place only if it
//  succeeded, and store the result (or null) into the member.  The dump
//  gives the line numbers (0x34, 0x4A, 0x57, 0x64) but truncates the file
//  string, so the constant below is the visible prefix completed by
//  convention; it is a Storm leak-log tag, nothing reads it, and both sides
//  canonicalise the operand to an anonymous symbol either way.
//
//  Four float constants, all read out of the binary rather than guessed
//  (`ida_query bytes` on each of IDA's own `flt_` names):
//
//      flt_6F92FE40  0.01f     the backdrop's corner size
//      flt_6F93E8D0  0.0019f   its four inset edges
//      flt_6F93C238  -0.006f   the name line's y offset
//      flt_6F93A06C  0.008f    its x offset
//      flt_6F935DA0  -0.005f   the two stacked lines' y offset
//
//  The five `mov byte ptr [esp+..], N` stores between the steps are the
//  unwind states of the `__except_handler4`-shaped frame this toolchain
//  cannot reproduce - one per constructed member that would have to be
//  unwound if a later one threw (docs/msvc-vc8-idioms.md).
//
//  Own translation unit: eleven real calls out of it.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "storm.h"
#include "agenttypedslots.h"

//  0x6F4C5CF0 - Jass/jassnatives.h's, under the same name.
struct STStringField { void Assign(const char* value); };
//  0x6F009E30 - Config/configgates.cpp.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);

//: off_6FA8C27C - a `const char*` holding "FontHeights", the config section
//: the three font heights come out of.
extern const char* g_pFontHeightsSection;

//  slot 0x64 of the frame base: show.
typedef void (__thiscall *TipShowFn)(void* self);

//  The backdrop descriptor the tip owns: two skin strings and eight floats.
struct SUnitTipBackdrop
{
    char          m_reserved00[0x14];
    STStringField m_background;     // +0x14
    char          m_reserved18[0x08];
    STStringField m_border;         // +0x20
    char          m_reserved24[0x08];
    int           m_alpha;          // +0x2C
    int           m_tiled;          // +0x30
    char          m_reserved34[0x04];
    float         m_cornerSize;     // +0x38
    char          m_reserved3C[0x04];
    float         m_insetLeft;      // +0x40
    float         m_insetRight;     // +0x44
    float         m_insetTop;       // +0x48
    float         m_insetBottom;    // +0x4C
};

static const char kUnitTipFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\CUnitTip.cpp";

CUnitTip* CUnitTip::Construct(int style)
{
    new ((void*)this) CSimpleFrame((void*)style);

    new (m_selected) SCheckedSelectableSlot((CAgent*)0);

    //  ??_7CUnitTip@@6B@, 0x6F93D36C (`ida_query names CUnitTip`).  A
    //  literal and not a `virtual` declaration, for the reason
    //  gameuicursorreset.cpp records at length.
    m_vtable = (void*)0x6F93D36C;

    m_hidden = 0;
    ((TipShowFn)(*(void***)this)[0x64 / 4])(this);

    //  --- the backdrop -----------------------------------------------------
    void* block = SMemAlloc(0x50, kUnitTipFile, 0x34, 0);
    SUnitTipBackdrop* backdrop;

    if (block != 0)
    {
        new (block) CBackdropGenerator();
        backdrop = (SUnitTipBackdrop*)block;
    }
    else
    {
        backdrop = 0;
    }

    m_pBackdrop = backdrop;

    const char* border = ResolveSkinValue("ToolTipBorder", 0);
    const char* background = ResolveSkinValue("ToolTipBackground", 0);

    SUnitTipBackdrop* skin = (SUnitTipBackdrop*)m_pBackdrop;

    skin->m_background.Assign(background);
    skin->m_alpha = 0xFF;
    skin->m_tiled = 1;
    skin->m_border.Assign(border);

    ((SUnitTipBackdrop*)m_pBackdrop)->m_cornerSize = 0.01f;

    SUnitTipBackdrop* insets = (SUnitTipBackdrop*)m_pBackdrop;

    insets->m_insetTop = 0.0019f;
    insets->m_insetBottom = 0.0019f;
    insets->m_insetLeft = 0.0019f;
    insets->m_insetRight = 0.0019f;

    ((CSimpleFrame*)this)->SetOwnedFrame(m_pBackdrop);

    //  --- the player-name line --------------------------------------------
    block = SMemAlloc(0xC4, kUnitTipFile, 0x4A, 0);

    CSimpleFontString* nameLine;

    nameLine = new (block) CSimpleFontString(this, 3, 1);

    m_pNameLine = nameLine;

    float height = GetConfigFloatHW(g_pFontHeightsSection,
                                    "UnitTipPlayerName", 0);

    ((CSimpleFontString*)m_pNameLine)->SetFont(
        ResolveSkinValue("MasterFont", 0), height, 0);

    unsigned int color = 0xFFFFFFFFu;

    ((CSimpleFontString*)m_pNameLine)->SetColor(&color);

    ((CSimpleFrame*)m_pNameLine)->SetPointTo(0, (CLayoutFrame*)this, 0, 0.008f, -0.006f, 1);

    ((CSimpleFontString*)m_pNameLine)->SetJustify(1);
    ((CSimpleFontString*)m_pNameLine)->SetText(" ");

    //  --- the second line, pinned under the name --------------------------
    block = SMemAlloc(0xC4, kUnitTipFile, 0x57, 0);

    CSimpleFontString* detailLine;

    detailLine = new (block) CSimpleFontString(this, 3, 1);

    m_pDetailLine = detailLine;

    height = GetConfigFloatHW(g_pFontHeightsSection, "UnitTipPlayerName", 0);

    ((CSimpleFontString*)m_pDetailLine)->SetFont(
        ResolveSkinValue("MasterFont", 0), height, 0);

    color = 0xFFFFFFFFu;

    ((CSimpleFontString*)m_pDetailLine)->SetColor(&color);

    ((CSimpleFrame*)m_pDetailLine)->SetPointTo(0, (CLayoutFrame*)m_pNameLine, 6,
                                               0.0f, -0.005f, 1);

    ((CSimpleFontString*)m_pDetailLine)->SetJustify(1);
    ((CSimpleFontString*)m_pDetailLine)->SetText(" ");

    //  --- the description, pinned under the second line -------------------
    block = SMemAlloc(0xC4, kUnitTipFile, 0x64, 0);

    CSimpleFontString* descLine;

    descLine = new (block) CSimpleFontString(this, 3, 1);

    m_pDescLine = descLine;

    height = GetConfigFloatHW(g_pFontHeightsSection, "UnitTipDesc", 0);

    ((CSimpleFontString*)m_pDescLine)->SetFont(
        ResolveSkinValue("MasterFont", 0), height, 0);

    color = 0xFFFFFFFFu;

    ((CSimpleFontString*)m_pDescLine)->SetColor(&color);

    ((CSimpleFrame*)m_pDescLine)->SetPointTo(0, (CLayoutFrame*)m_pDetailLine, 6,
                                             0.0f, -0.005f, 1);

    ((CSimpleFontString*)m_pDescLine)->SetJustify(1);
    ((CSimpleFontString*)m_pDescLine)->SetText(" ");

    return this;
}
