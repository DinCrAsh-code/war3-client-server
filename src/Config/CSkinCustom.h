//============================================================================
//  CSkinCustom - checksum-batch-E.  RTTI-confirmed
//  (agent_worktrees/classes/0x6F937B38.json, `CSkinCustom: CMiscCustom,
//  CSynchronousData;`) to derive directly from CMiscCustom (CMiscCustom.h),
//  which itself derives CSynchronousData.  Not one of the six named
//  CGameDataCache checksum-provider array slots
//  docs/notes/checksum-provider-registry.md tracks - a CSynchronousData
//  sibling reached the same way CMiscCustom itself was found (grepping
//  agent_worktrees/classes/*.json for the base), not a provider this
//  batch traced a registration site for.
//
//  CSkinCustom adds no fields and declares no destructor of its own: its
//  own vtable (0x6F937B38) names the *literal same* target address as
//  CMiscCustom's own for slot 0 (ComputeChecksum, 0x6F0020D0 - inherited
//  unchanged) and for the base half of slot 2's own work (DeleteSelf below
//  calls CMiscCustom::~CMiscCustom() directly, 0x6F0024E0 - the identical
//  "no destructor of its own" shape
//  Item/abilitycustomdata.h's CAbilityCustomData::DeleteSelf already
//  established for a different class pair). Slot 3 is this class's own
//  only real override - see below.
//============================================================================
#ifndef CSKINCUSTOM_H
#define CSKINCUSTOM_H

#include "CMiscCustom.h"

class CSkinCustom : public CMiscCustom
{
public:
    //  0x6F30ECC0 - vtable slot 2, the scalar deleting destructor.
    //  Byte-identical in shape to CMiscCustom::DeleteSelf: this class adds
    //  no fields, so its own destructor call reduces to the base's own
    //  ~CMiscCustom() directly - the dump confirms it by calling
    //  0x6F0024E0, the exact same address CMiscCustom::DeleteSelf calls.
    //  See CSkinCustom.cpp.
    CSkinCustom* DeleteSelf(unsigned int flags);

    //  0x6F309820 - vtable slot 3, `mov eax,1 ; retn`: this class's own
    //  override of CMiscCustom::Method3(), always answering 1 where the
    //  base always answers 0.  Purpose not established - see
    //  CMiscCustom.h's own note on the base slot.
    unsigned int Method3() const;
};

#endif
