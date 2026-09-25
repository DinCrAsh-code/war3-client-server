#include "CMiscCustom.h"

//----------------------------------------------------------------------------
//  0x6F0020A0 - installs the vftable and clears both sources.
//
//  0x6F8750A4 is the live vtable address (found via debugger, not
//  recoverable from the call tree - see the class comment in
//  CMiscCustom.h for why this is a plain pointer store and not
//  `virtual ~CMiscCustom()` building our own).
//----------------------------------------------------------------------------
CMiscCustom::CMiscCustom()
    : m_vtable((void*)0x6F8750A4)
    , m_pPrimary(0)
    , m_pSecondary(0)
{
}

//----------------------------------------------------------------------------
//  0x6F0020D0 - vtable slot 0, ComputeChecksum.  See the header's own note;
//  m_pPrimary is folded first, matching the dump's own `+0x04` before
//  `+0x08` read order (corrected - an earlier pass in this same batch had
//  the two sources backwards; compare.py's own side-by-side caught it:
//  `mov ecx,[edi+4]` is the dump's very first field read).
//----------------------------------------------------------------------------
unsigned int CMiscCustom::ComputeChecksum() const
{
    unsigned int hash = 0;
    if (m_pPrimary != 0)
    {
        unsigned int mixed = hash ^ m_pPrimary->ComputeChecksum();
        hash = (mixed << 3) | (mixed >> 29);
    }
    if (m_pSecondary != 0)
    {
        unsigned int mixed = hash ^ m_pSecondary->ComputeChecksum();
        hash = (mixed << 3) | (mixed >> 29);
    }
    return hash;
}

//----------------------------------------------------------------------------
//  0x6F0020C0 - vtable slot 3.  See the header's own note on why this
//  stays a generic, unnamed accessor.
//----------------------------------------------------------------------------
unsigned int CMiscCustom::Method3() const
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F002170 - is the key present in either source.
//
//  The two tests are written out rather than looped: the shipped code loads
//  each pointer into the call's `this` register and tests it there, and the
//  second `return 1` cross-jumps onto the first one's epilogue.
//----------------------------------------------------------------------------
int CMiscCustom::HasValue(const char* section, const char* key) const
{
    if (m_pPrimary != 0 && m_pPrimary->HasValue(section, key))
        return 1;
    if (m_pSecondary != 0 && m_pSecondary->HasValue(section, key))
        return 1;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F002300 - the value from the first source that has the key, zero if
//  neither does.  The presence test is repeated per source rather than merged
//  with HasValue() above, because a source that has the key must answer for
//  it even if its value fails to parse.
//----------------------------------------------------------------------------
CFloat CMiscCustom::GetFloat(const char* section, const char* key,
                             int index) const
{
    if (m_pPrimary != 0 && m_pPrimary->HasValue(section, key))
        return m_pPrimary->GetFloat(section, key, index);
    if (m_pSecondary != 0 && m_pSecondary->HasValue(section, key))
        return m_pSecondary->GetFloat(section, key, index);
    return g_CFloatZero;
}

//----------------------------------------------------------------------------
//  0x6F002220 - the int sibling of GetFloat above, and it is the same
//  two-source shape.  This was a redirect on the stated grounds that "no
//  dump in this repo carries its body"; one does now
//  (asm/sub_6F29E930_...), and it confirms the guess the old comment
//  refused to make - the same presence-test-then-fetch per source, with
//  zero rather than g_CFloatZero as the answer when neither has the key.
//----------------------------------------------------------------------------
int CMiscCustom::GetInt(const char* section, const char* key, int index) const
{
    if (m_pPrimary != 0 && m_pPrimary->HasValue(section, key))
        return m_pPrimary->GetInt(section, key, index);
    if (m_pSecondary != 0 && m_pSecondary->HasValue(section, key))
        return m_pSecondary->GetInt(section, key, index);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F002400 - the string sibling of GetInt and GetFloat, and the same
//  two-source shape: the first source that *has* the key answers for it,
//  even if its value is empty, and a null is the answer when neither does.
//
//  Unlike its two siblings the shipped body reads the key and the section
//  into registers up front and re-loads only the source pointer per arm,
//  which is what the pair of `push edi` / `push ebx` runs either side of the
//  presence test are.
//----------------------------------------------------------------------------
const char* CMiscCustom::GetString(const char* section, const char* key,
                                   int index) const
{
    if (m_pPrimary != 0 && m_pPrimary->HasValue(section, key))
        return m_pPrimary->GetString(section, key, index);
    if (m_pSecondary != 0 && m_pSecondary->HasValue(section, key))
        return m_pSecondary->GetString(section, key, index);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F002290 - the hardware-float sibling of GetFloat above, and the same
//  two-source shape.  What differs is only the return convention: the value
//  comes back in ST(0) rather than through a CFloat return buffer, so the
//  "neither source had it" answer is a plain `fldz` where GetFloat's is a
//  copy of g_CFloatZero.
//----------------------------------------------------------------------------
float CMiscCustom::GetFloatRaw(const char* section, const char* key,
                               int index) const
{
    if (m_pPrimary != 0 && m_pPrimary->HasValue(section, key))
        return m_pPrimary->GetFloatRaw(section, key, index);
    if (m_pSecondary != 0 && m_pSecondary->HasValue(section, key))
        return m_pSecondary->GetFloatRaw(section, key, index);
    return 0.0f;
}
