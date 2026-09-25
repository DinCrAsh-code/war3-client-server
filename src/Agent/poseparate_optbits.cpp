//============================================================================
//  NIpse::CPoSeparate::SetOptNibbleAtBit16/20/28 (0x6F494FB0/0x6F494FD0/
//  0x6F494FF0) - three small stack-argument setters that pack a value into
//  a fixed bit position of m_opt (+0x20), each with its own mask. See
//  poseparate.h's own header comment.
//============================================================================
#include "poseparate.h"

namespace NIpse {

//  0x6F494FB0 - shift 0x10 (bits 16-23), but the mask clears only bits
//  16-19 (0xFFF0FFFF) - one nibble, not the full byte the shift would
//  otherwise occupy. Transcribed exactly as shipped.
void CPoSeparate::SetOptNibbleAtBit16(unsigned char value)
{
    m_opt = ((unsigned int)value << 0x10) | (m_opt & 0xFFF0FFFFu);
}

//  0x6F494FD0 - shift 0x14 (bits 20-...), mask 0xF00FFFFF clears bits
//  20-23.
void CPoSeparate::SetOptNibbleAtBit20(unsigned short value)
{
    m_opt = ((unsigned int)value << 0x14) | (m_opt & 0xF00FFFFFu);
}

//  0x6F494FF0 - shift 0x1C (bits 28-31), mask 0x0FFFFFFF clears the top
//  nibble.
void CPoSeparate::SetOptNibbleAtBit28(unsigned char value)
{
    m_opt = ((unsigned int)value << 0x1C) | (m_opt & 0x0FFFFFFFu);
}

}  // namespace NIpse
