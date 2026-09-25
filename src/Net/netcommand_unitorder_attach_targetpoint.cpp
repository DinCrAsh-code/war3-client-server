//============================================================================
//  0x6F553F60 - CNetCommandUnitOrderTargetPoint's Attach hook: the base's
//  own hook first, then the two CFloat coordinates.  Own TU - see
//  netcommand_unitorder_attach_basic.cpp.
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderBasic_Attach(CDataStore* reader, CNetCommandUnitOrderBasic* cmd);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

void* __fastcall CNetCommandUnitOrderTargetPoint_Attach(CDataStore* reader, CNetCommandUnitOrderTargetPoint* cmd)
{
    CNetCommandUnitOrderBasic_Attach(reader, cmd);
    ReadCFloat(reader, &cmd->m_x);
    ReadCFloat(reader, &cmd->m_y);
    return reader;
}
