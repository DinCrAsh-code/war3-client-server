//============================================================================
//  0x6F4CDE70 - find or make the cached font for a name, a style and a
//  height.
//
//  The cache key is the three of them printed into a 0x114-byte buffer with
//  `"%s-%d-%f"`, which is also why this function carries the shipped /GS
//  prologue and epilogue this repo's fixed `/GS-` cannot emit: the cookie
//  load, the `xor esp`, the check call and the four bytes of displacement the
//  cookie slot puts under every `[esp+N]` are the bulk of what is missing
//  here and none of it is behavioural.
//
//  The height is first divided by the UI space's Y span and then *clamped
//  down* to 1.0 - a font taller than the whole screen is asked for at one
//  screen height.
//============================================================================
#include "text.h"
#include "renderthunks.h"

static const char kStormTemplateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h";
static const char kFontTag[] = "HTEXTFONT";
static const char kFontKeyFormat[] = "%s-%d-%f";

void* __fastcall CreateFont(const char* name, float height, int flags)
{
    char key[0x114];

    height = DivideByConst_6F93C218(height);
    if (!(height < 1.0f))
        height = 1.0f;

    Storm_578(key, sizeof(key), kFontKeyFormat, name, flags, height);

    FONTHASHOBJ* cached = g_fontTable.Ptr(key);
    if (cached != 0)
        return ((TRefCnt*)cached)->AddRef("HTEXTFONT");

    unsigned int hash = Storm_590(key);

    FONTHASHOBJ* node = g_fontTable.NewNode(hash, 0, 0);
    node->m_hashval = hash;

    if (node->m_key != key)
    {
        if (node->m_key != 0)
            SMemFree((void*)node->m_key, kStormTemplateHeader, 0xA38, 0);

        node->m_key = Storm_507(key, kStormTemplateHeader, 0xA39);
    }

    int engineFlags = 0;
    if (flags & 1)
        engineFlags = 1;
    if (flags & 2)
        engineFlags |= 2;

    if (TextEngineCreateFont(name, &node->m_face, height, engineFlags) == 0)
    {
        g_fontTable.Unlink(node);

        //  Slot 0 of the table's own vtable hands the node back to whatever
        //  allocated it; see Containers/allochashtable.h for why the two
        //  reached slots are dispatched by hand.
        typedef void (__thiscall *FreeNodeFn)(void*, FONTHASHOBJ*);
        ((FreeNodeFn)(*(void***)&g_fontTable)[0])(&g_fontTable, node);
        return 0;
    }

    return ((TRefCnt*)node)->AddRef("HTEXTFONT");
}
