//============================================================================
//  0x6F367FF0 - CChatEditBar::CChatEditBar: the ": " line the player types
//  chat into, and the message frame above it.
//
//  The class name is the shipped one (??_7CChatEditBar@@6B@ at +0x00 and its
//  `_0` twin at +0x0B4) and the base chain is read off the constructors, not
//  guessed: CChatEditBar -> CBackdropFrame (0x6F620ED0) -> CModelFrame
//  (0x6F621B70) -> CFrame (0x6F608200).  Both intermediates are reconstructed
//  in src/Frame/, which is what lets this one be a constructor at all.
//
//  It builds two children and holds each by count:
//
//    * the edit box, given the "ChatFont" skin at the [FontHeights]
//      ChatEditBar size, justify 8, dirty flag 1, colour 0xFFCCAC12, anchored
//      by its points 0 and 6 to the bar's own at (0.003, +-0.0015), and a
//      0xFF000000 shadow offset by (0.001, -0.001);
//    * the message frame, capped at 0x7F lines, given the chat font name the
//      preferences resolve (a 0x104-byte buffer) at the same size, white,
//      justify 8, anchored point 0 to point 2 of the edit box and point 8 to
//      point 8 of the bar, with the same shadow - this one applied to
//      whatever its +0x254 hands back rather than to the frame itself.
//
//  Both handles go in through the same three-step counted-reference swap the
//  shipped code writes out inline: addref the new one, release the old one,
//  store.  Both children then get `m_flags |= m_flagMask | 2`.
//
//  The last two calls are hand-written dispatches through the message
//  frame's own slot 2, and vtable_dispatch_audit.py cannot judge them - the
//  receiver is not `this`.  Their argument count is the shipped call sites'
//  (0x6F368375 and 0x6F36838D push exactly three dwords each), and both push
//  the same event id twice, which is the shipped code and not a typo here.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base to unwind, *plus* the two /GS cookie checks its
//  0x104-byte font-name buffer earns it.
//
//  Own translation unit: twenty-two real calls out of it.
//============================================================================
#include "modelframe.h"
#include "gameui.h"
#include "refcnt.h"
#include "clayer.h"
#include "renderdevice.h"       // TEXTURECOLOR

//  0x6F009E30 - Config/configgates.cpp.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);
//  off_6FA8C27C - "FontHeights", the config section the font heights come
//  out of.  Declared exactly as GameUI/gameuiunittip.cpp declares it.
extern const char* g_pFontHeightsSection;
//  0x6F5BCAC0 - GameUI/chatfontname.cpp.
void __fastcall GetChatFontName(char* buffer, unsigned int size);
//  0x6F614650 - Misc/trivialaccessors_02_03.cpp: the frame's own +0x254.
int __fastcall GetField0x254_6F614650(const void* self);

//  0x6F611390 / 0x6F611590 - Misc/sweep_field_setters.cpp, under the names
//  that file gives them.
struct SDirtyFlagOwner
{
    void __thiscall SetField21C(int value);
    void __thiscall SetField1F4IfChanged(const int* value);
};

//----------------------------------------------------------------------------
//  The two children.  GameUI/gameui.h holds both as `CTextFrame*`, and the
//  two setter *clusters* say they are not the same class: the prefix label's
//  five setters are all 0x6F5FB960/0x6F611xxx and the entry box's four are
//  all 0x6F613xxx/0x6F616250.  Neither cluster's class is named by anything
//  in this tree, so each is named here for what the bar builds it as, and
//  each carries only the methods the bar reaches - no members and no layout,
//  which is why the fields stay CTextFrame* in the header and every use here
//  is a cast.  All nine setters are still redirects.
//----------------------------------------------------------------------------
class CChatPrefixLabel
{
public:
    //  0x6F5FB960 - `retn 0Ch`.
    void SetFont(const char* path, float height, int flags);
    //  0x6F611EB0 - `retn 4`.
    void SetJustify(int justify);
    //  0x6F611F90 - `retn 8`: the colour is a four-byte class by value and
    //  the offset arrives by address, which is what the shipped `push` of a
    //  reserved slot written through esp says.
    void SetShadow(TEXTURECOLOR colour, const float* offset);
};

class CChatEntryBox
{
public:
    //  0x6F616250 - `retn 4`.
    void SetMaxLines(int lines);
    //  0x6F613CA0 - `retn 0Ch`.
    void SetFont(const char* path, float height, int flags);
    //  0x6F613D60 - `retn 4`.
    void SetColor(const unsigned int* colour);
    //  0x6F613D90 - `retn 4`.
    void SetJustify(int justify);
};

//  Slot 2 (+0x08) of the message frame - register one event id.  Three
//  dwords, which is what both call sites push.
typedef void (__thiscall *MessageFrameRegisterFn)(void* self, int a, int b,
                                                  void* context);

SSelectionHost::SSelectionHost(CGameUI* ui, void* owner, int frameId)
    : CBackdropFrame((int)ui, (int)owner, frameId)
{
    m_entryActive = 0;
    m_pEntryBox   = 0;
    m_pPrefixLabel = 0;
    m_colonTarget = 0;

    SetBackdropArt(ResolveSkinValue("ToolTipBackground", 0), 0xFF, 1,
                   ResolveSkinValue("ToolTipBorder", 0), 1);

    m_inset[2] = 0.0019f;
    m_inset[3] = 0.0019f;
    m_inset[0] = 0.0019f;
    m_inset[1] = 0.0019f;

    m_field1D8 = 1;
    m_float1B8 = 0.01f;

    //  The counted-reference swap, written out because that is what the
    //  shipped stream is: addref the new one, release the old one, store.
    CTextFrame* box = CreatePrefixLabel(0, 0);
    if (box != 0)
        ((TRefCnt*)box)->m_refcount++;

    TRefCnt* oldBox = m_pPrefixLabel;
    if (oldBox != 0)
    {
        if (--oldBox->m_refcount == 0)
            oldBox->ReleaseSelf();
    }
    m_pPrefixLabel = box;

    ((CLayer*)box)->m_flags |= ((CLayer*)box)->m_flagMask | 2;

    float height = GetConfigFloatHW(g_pFontHeightsSection, "ChatEditBar", 0);
    ((CChatPrefixLabel*)m_pPrefixLabel)->SetFont(
        ResolveSkinValue("ChatFont", 0), height, 0);

    ((CChatPrefixLabel*)m_pPrefixLabel)->SetJustify(8);
    ((SDirtyFlagOwner*)m_pPrefixLabel)->SetField21C(1);

    int colour = (int)0xFFCCAC12;
    ((SDirtyFlagOwner*)m_pPrefixLabel)->SetField1F4IfChanged(&colour);

    ((CLayoutFrame*)((char*)m_pPrefixLabel + 0xB4))->SetPointTo(
        0, (CLayoutFrame*)((char*)this + 0xB4), 0, 0.003f, 0.0015f, 1);
    ((CLayoutFrame*)((char*)m_pPrefixLabel + 0xB4))->SetPointTo(
        6, (CLayoutFrame*)((char*)this + 0xB4), 6, 0.003f, -0.0015f, 1);

    float shadow[2];
    shadow[0] = 0.001f;
    shadow[1] = -0.001f;
    TEXTURECOLOR black;
    black.m_argb = 0xFF000000;
    ((CChatPrefixLabel*)m_pPrefixLabel)->SetShadow(black, shadow);

    char fontName[0x104];
    GetChatFontName(fontName, 0x104);

    CTextFrame* entry = CreateEntryBox(0, 0);
    if (entry != 0)
        ((TRefCnt*)entry)->m_refcount++;

    TRefCnt* oldMessages = m_pEntryBox;
    if (oldMessages != 0)
    {
        if (--oldMessages->m_refcount == 0)
            oldMessages->ReleaseSelf();
    }
    m_pEntryBox = entry;

    ((CChatEntryBox*)entry)->SetMaxLines(0x7F);

    ((CLayer*)m_pEntryBox)->m_flags |= ((CLayer*)m_pEntryBox)->m_flagMask | 2;

    float height2 = GetConfigFloatHW(g_pFontHeightsSection, "ChatEditBar", 0);
    ((CChatEntryBox*)m_pEntryBox)->SetFont(fontName, height2, 0);

    unsigned int white = 0xFFFFFFFF;
    ((CChatEntryBox*)m_pEntryBox)->SetColor(&white);

    ((CChatEntryBox*)m_pEntryBox)->SetJustify(8);

    ((CLayoutFrame*)((char*)m_pEntryBox + 0xB4))->SetPointTo(
        0, (CLayoutFrame*)(m_pPrefixLabel ? (char*)m_pPrefixLabel + 0xB4 : 0), 2,
        0.002f, 0.0f, 1);
    ((CLayoutFrame*)((char*)m_pEntryBox + 0xB4))->SetPointTo(
        8, (CLayoutFrame*)((char*)this + 0xB4), 8, -0.0015f, -0.0015f, 1);

    float shadow2[2];
    shadow2[0] = 0.001f;
    shadow2[1] = -0.001f;
    black.m_argb = 0xFF000000;
    ((CChatPrefixLabel*)GetField0x254_6F614650(m_pEntryBox))->SetShadow(
        black, shadow2);

    ((MessageFrameRegisterFn)(*(void***)m_pEntryBox)[8 / 4])(
        m_pEntryBox, 0x40060064, 0x40060064, this);
    ((MessageFrameRegisterFn)(*(void***)m_pEntryBox)[8 / 4])(
        m_pEntryBox, 0x40060066, 0x40060066, this);

    ((SSelectionHost*)this)->SetEntryActive(0);
}
