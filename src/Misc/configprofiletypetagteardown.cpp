//============================================================================
//  0x6F4CED50 - the config-profile module's own teardown: release two
//  RTTI-tagged records, "AuKeyValueProfile" and "AuSectionProfile".  Both
//  are `CDataAllocator` instances releasing through the shared
//  ReleaseAllBlocks (Net/dataallocator.cpp, 0x6F4C1A60) - see
//  spritetypetagteardown.cpp's own header comment for why this file no
//  longer routes through a locally-declared `TypeTagRecord::Destroy`
//  duplicate.
//============================================================================
#include "dataallocator.h"

extern CDataAllocator g_keyValueProfileTag;   // unk_6FAB7E84
extern CDataAllocator g_sectionProfileTag;    // unk_6FAB7E70

void TeardownConfigProfileTypeTags()
{
    //  IDA's own labels ("aAukeyvalueProf"/"aAusectionProfi") are truncated
    //  auto-names for RTTI-mangled strings; exact spelling not recoverable
    //  from this dump and does not affect the instruction stream.
    g_keyValueProfileTag.ReleaseAllBlocks(".?AUAuKeyValueProfile@@", -2, 1);
    g_sectionProfileTag.ReleaseAllBlocks(".?AUAuSectionProfile@@", -2, 1);
}
