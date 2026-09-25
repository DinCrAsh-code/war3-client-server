//============================================================================
//  0x6F553FC0 - CNetCommandUnitOrderTargetImage's Attach hook: TargetPoint's
//  own hook first, then the two unit-handle dwords.  Own TU - see
//  netcommand_unitorder_attach_basic.cpp.
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderTargetPoint_Attach(CDataStore* reader, CNetCommandUnitOrderTargetPoint* cmd);

void* __fastcall CNetCommandUnitOrderTargetImage_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImage* cmd)
{
    CNetCommandUnitOrderTargetPoint_Attach(reader, cmd);
    reader->ReadDword(&cmd->m_handle0);
    reader->ReadDword(&cmd->m_handle1);
    return reader;
}
