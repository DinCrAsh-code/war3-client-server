//============================================================================
//  0x6F554130 - CNetCommandUnitOrderTargetImageFogged2's Attach hook:
//  TargetImageFogged's own hook first, then two more ints.  Own TU - see
//  netcommand_unitorder_attach_basic.cpp.
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderTargetImageFogged_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImageFogged* cmd);

void* __fastcall CNetCommandUnitOrderTargetImageFogged2_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImageFogged2* cmd)
{
    CNetCommandUnitOrderTargetImageFogged_Attach(reader, cmd);
    reader->ReadDword((unsigned int*)&cmd->m_field48);
    reader->ReadDword((unsigned int*)&cmd->m_field4C);
    return reader;
}
