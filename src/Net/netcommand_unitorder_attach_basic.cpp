//============================================================================
//  0x6F553EF0 - CNetCommandUnitOrderBasic's own Attach hook, the root of the
//  unit-order chain (netcommand_unitorder.h).  Own TU so the chained callers
//  below (netcommand_unitorder_attach_targetpoint.cpp and on) call it for
//  real instead of inlining it away - the same reason every Attach hook in
//  this module gets its own translation unit (netcommand_pause_attach.cpp).
//============================================================================
#include "netcommand_unitorder.h"
#include "cdatastore.h"

void* __fastcall CNetCommandUnitOrderBasic_Attach(CDataStore* reader, CNetCommandUnitOrderBasic* cmd)
{
    reader->ReadWord(&cmd->m_field18);
    reader->ReadDword((unsigned int*)&cmd->m_field1C);
    reader->ReadDword((unsigned int*)&cmd->m_field20);
    reader->ReadDword((unsigned int*)&cmd->m_field24);
    return reader;
}
