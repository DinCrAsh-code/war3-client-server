//============================================================================
//  0x6F5540B0 - CNetCommandUnitOrderTargetImageFogged's Attach hook.  Calls
//  TargetPoint's own hook directly (this class derives from TargetPoint,
//  not TargetImage), then its own three dwords, one byte and two CFloats.
//  Own TU - see netcommand_unitorder_attach_basic.cpp.
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderTargetPoint_Attach(CDataStore* reader, CNetCommandUnitOrderTargetPoint* cmd);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

void* __fastcall CNetCommandUnitOrderTargetImageFogged_Attach(CDataStore* reader, CNetCommandUnitOrderTargetImageFogged* cmd)
{
    CNetCommandUnitOrderTargetPoint_Attach(reader, cmd);
    reader->ReadDword(&cmd->m_field30);
    reader->ReadDword(&cmd->m_field34);
    reader->ReadDword(&cmd->m_field38);
    reader->ReadByte(&cmd->m_field3C);
    ReadCFloat(reader, &cmd->m_field40);
    ReadCFloat(reader, &cmd->m_field44);
    return reader;
}
