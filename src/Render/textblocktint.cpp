//============================================================================
//  0x6F4CC470 - hand a laid-out block's colour and shadow offsets to the
//  layout engine.  Four instructions, all of them forwarding: the engine
//  wants the block's own engine-side handle rather than the TEXTBLOCK, the
//  colour by address, and the offsets on the stack.
//
//  `lea edx, [esp+4+arg_0]` - the address of the by-value colour parameter -
//  is what says the colour is a one-dword *class* and not an `unsigned int`;
//  see text.h's CFrameColor.
//============================================================================
#include "text.h"
#include "renderthunks.h"

void __fastcall SetTextBlockTint(void* block, const float* offsets,
                                 CFrameColor color)
{
    TextEngineTintBlock(((TEXTBLOCK*)block)->m_engineBlock, &color.m_value,
                        offsets);
}
