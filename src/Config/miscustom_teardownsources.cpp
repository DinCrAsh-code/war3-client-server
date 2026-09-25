//============================================================================
//  0x6F002130 - CMiscCustom::TeardownSources(): release each non-null
//  source through its own vtable slot 0 with flags=1 (the same "delete me
//  too" convention this repo's own DeleteSelf overrides use, e.g.
//  Item/abilitycustomdata_deleteself.cpp) and null the pointer. Secondary
//  first, primary second, matching the dump's own field-descending read
//  order - not source-declaration order.
//
//  Own translation unit for the same reason
//  Item/abilitycustomdata_release.cpp's own ReleaseOwnedRefAndFinalize()
//  is: ~CMiscCustom() (miscustom_dtor.cpp) must reach this through a real
//  `call`, not an inlined body - with both in one TU this compiler folds
//  the whole teardown straight into the destructor (and from there into
//  every caller of the destructor, including DeleteSelf), which is not
//  what the shipped `jmp sub_6F002130` tail call shows.
//============================================================================
#include "CMiscCustom.h"

typedef void (__thiscall *ConfigSourceDeleteSelfFn)(void*, unsigned int);

void CMiscCustom::TeardownSources()
{
    if (m_pSecondary != 0)
    {
        ((ConfigSourceDeleteSelfFn)(*(void***)m_pSecondary)[0])(m_pSecondary, 1);
        m_pSecondary = 0;
    }
    if (m_pPrimary != 0)
    {
        ((ConfigSourceDeleteSelfFn)(*(void***)m_pPrimary)[0])(m_pPrimary, 1);
        m_pPrimary = 0;
    }
}
