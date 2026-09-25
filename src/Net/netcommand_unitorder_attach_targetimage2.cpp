//============================================================================
//  0x6F554020 - CNetCommandUnitOrderTargetImage2's Attach hook: TargetImage's
//  own hook first, then two more handle dwords.  Own TU - see
//  netcommand_unitorder_attach_basic.cpp.
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderTargetImage_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImage* cmd);

void* __fastcall CNetCommandUnitOrderTargetImage2_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImage2* cmd)
{
    CNetCommandUnitOrderTargetImage_Attach(reader, cmd);
    reader->ReadDword(&cmd->m_handle2);
    reader->ReadDword(&cmd->m_handle3);
    return reader;
}
