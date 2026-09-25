//============================================================================
//  0x6F495ED0 - NIpse::CPoPosCl::TeardownAndOptionallyRecreatePoSeparate.
//  See poposcl.h's own declaration. Own translation unit, separate from
//  CPoPosCl::Slot4 (poposcl_relatedagentslot4.cpp), so a Slot4-shaped
//  all-zero call site does not get folded straight into Slot4's own body -
//  the same trap Agent/popos_apply_slot4.cpp's own history and
//  Agent/poposbh_relatedagents.cpp's `ClearBcRegistration` already caught.
//============================================================================
#include "poposcl.h"
#include "poseparate_pool.h"
#include "game.h"

namespace NIpse {

void CPoPosCl::TeardownAndOptionallyRecreatePoSeparate(int create, unsigned char opt16,
                                                         unsigned short opt20, unsigned char opt28)
{
    if (m_poSeparate != 0)
    {
        typedef void* (__thiscall *Slot4Fn)(void*, unsigned int);
        ((Slot4Fn)(*(void***)m_poSeparate)[4])(m_poSeparate, 0);
        m_poSeparate = 0;
    }

    if (create == 0)
        return;

    CPoSeparate_AllocateAndOptionallyApply(&m_poSeparate, 0, 1);
    CPoSeparate* fresh = m_poSeparate;
    fresh->m_field14 = (unsigned int)this;
    fresh->SetOptNibbleAtBit16(opt16);
    fresh->SetOptNibbleAtBit20(opt20);
    fresh->SetOptNibbleAtBit28(opt28);

    ((SPoSeparateGlobalListAnchor*)((char*)g_pTimeSync + 0x514))->InsertAfter(fresh);
}

}  // namespace NIpse
