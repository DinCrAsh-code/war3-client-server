//============================================================================
//  0x6F4DEA30 - HASHKEY_TEXTUREFILE::operator==.
//
//  The two option dwords are loaded once and XORed once per field: the
//  shipped stream recomputes `ecx = m_options ^ other.m_options` in front of
//  each of five `test cl, <mask>` with the disjoint, consecutive masks 3, 4,
//  8, 0x10 and 0x20.  That is what a chain of *bitfield* equalities on one
//  dword compiles to, and it is the only spelling that survives: written as
//  five masked equalities on the whole dword, this compiler folds the five
//  into one `test al, 3Fh` (measured: 8 of 41).
//
//  The name compare is last and bounded at 0x104 (MAX_PATH), case
//  insensitive, because a texture path is a file name.
//============================================================================
#include "texture.h"

int HASHKEY_TEXTUREFILE::operator==(const HASHKEY_TEXTUREFILE& other) const
{
    return m_options.m_bits.m_kind == other.m_options.m_bits.m_kind &&
           m_options.m_bits.m_opt2 == other.m_options.m_bits.m_opt2 &&
           m_options.m_bits.m_opt3 == other.m_options.m_bits.m_opt3 &&
           m_options.m_bits.m_opt4 == other.m_options.m_bits.m_opt4 &&
           m_options.m_bits.m_opt5 == other.m_options.m_bits.m_opt5 &&
           SStrnICmp(m_path, other.m_path, 0x104) == 0;
}
