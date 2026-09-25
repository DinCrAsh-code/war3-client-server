//============================================================================
//  0x6F509260 - four floats out of the selected variant, or the record's own
//  default block.
//
//  See modelpick.h for the register convention this cannot spell.  The
//  indexed read is `table[index * 0x28 + 0x18]`, written the way the shipped
//  stream forms it - `index * 5` scaled by eight - because that is one `lea`
//  and a scaled index rather than a multiply.
//============================================================================
#include "modelpick.h"

struct PICKVARIANTRECORD
{
    char     m_reserved00[0x20];    // +0x00
    float    m_defaults[4];         // +0x20
    int      m_hasVariants;         // +0x34
    char*    m_variantTable;        // +0x38
};

struct PICKVARIANTOWNER2
{
    char     m_reserved00[0x98];    // +0x00
    void*    m_selector;            // +0x98
};

void __fastcall QueryVariantVec4_6F509260(const void* record,
                                          const void* owner, float* out)
{
    const PICKVARIANTRECORD* rec = (const PICKVARIANTRECORD*)record;
    const PICKVARIANTOWNER2* own = (const PICKVARIANTOWNER2*)owner;

    if (rec->m_hasVariants != 0 && own->m_selector != 0 &&
        HasVariantSelection_6F514840(own->m_selector))
    {
        int index;
        QueryByte0x58_6F514820(own->m_selector, &index);

        const float* variant =
            (const float*)(rec->m_variantTable + index * 5 * 8 + 0x18);

        out[0] = variant[0];
        out[1] = variant[1];
        out[2] = variant[2];
        out[3] = variant[3];
        return;
    }

    out[0] = rec->m_defaults[0];
    out[1] = rec->m_defaults[1];
    out[2] = rec->m_defaults[2];
    out[3] = rec->m_defaults[3];
}
