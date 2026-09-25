//============================================================================
//  0x6F35E4E0 - CCommandButton::CCommandButton.
//
//  A CSimpleButton over the parent, sixteen of its own fields set, and - only
//  when the caller asks for one - a 0x6D8-byte CCommandButtonData allocated
//  and adopted.  The button then takes its own square size, 0.038 each way.
//
//  Both the size and the record's size are shipped constants: 0.038f is the
//  float at 0x6F93EBBC, which the instruction score cannot see either way (a
//  `fld <symbol>` canonicalises to `fld SYM` on both sides), and 0x6D8 is
//  the `push` in front of the allocation.  The tag names the module and
//  therefore the class.
//
//  The field order below is the shipped one and is load-bearing: the eight
//  zeroed floats and the `fld1` come first, then the four words round the
//  record, then the record itself, then the two size calls, and only then
//  the last five words.  Reordering them costs the alignment of everything
//  after the first move.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with a base and an owned allocation to unwind.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "commandbutton.h"
#include "storm.h"

static const char kCommandButtonFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CCommandButton.cpp";

CCommandButton::CCommandButton(void* parent, int withData)
    : CSimpleButton(parent)
{
    m_span[0] = 0.0f;
    m_span[1] = 0.0f;
    m_span[2] = 0.0f;
    m_span[3] = 0.0f;
    m_span[4] = 0.0f;
    m_span[5] = 0.0f;
    m_span[6] = 0.0f;
    m_span[7] = 0.0f;
    m_iconAlpha = 1.0f;

    m_field188 = 0;
    m_hasData  = 0;
    m_pData    = 0;
    m_field1B4 = 0;

    if (withData != 0)
    {
        void* memory = SMemAlloc(0x6D8, kCommandButtonFile, 0x27, 0);

        m_pData   = memory ? ((CCommandButtonData*)memory)->Construct() : 0;
        m_hasData = 1;
    }

    SetWidth(0.038f);
    SetHeight(0.038f);

    m_field1B0 = 0;
    m_field194 = 0;
    m_field198 = 0;
    m_field19C = 0;
    m_field1A8 = 0;
}
