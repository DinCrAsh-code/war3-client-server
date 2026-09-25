//============================================================================
//  0x6F4844B0 - NIpse::CPoSeparate::DumpDebugState, vtable slot 10. Prints
//  "\t[PoSe]" then "\t\tsepv x[%s] y[%s] opt[0x%08X]" through the same
//  LogFormatted/FormatCFloat pair Storm/textsink.h already establishes for
//  this whole family (see e.g. Agent/agentbaseabsdebugdump.cpp's own
//  inline-FormatCFloat-as-a-printf-argument shape).
//
//  Argument-evaluation-order note: the dump's own second LogFormatted call
//  pre-computes m_opt into a register and pushes it onto the stack BEFORE
//  either FormatCFloat call runs, then reuses that same stack slot as one
//  of LogFormatted's own trailing variadic arguments once both
//  FormatCFloat calls (each of which only consumes its own one stack
//  argument via `retn 4`, leaving anything pushed earlier alone) have run.
//  That is exactly what right-to-left C++ argument evaluation of
//  `LogFormatted(sink, fmt, FormatCFloat(&m_sepvX,...), FormatCFloat(&m_sepvY,...), m_opt)`
//  produces: `m_opt` (the rightmost argument) evaluates and pushes first,
//  then `FormatCFloat(&m_sepvY,...)`, then `FormatCFloat(&m_sepvX,...)`
//  last, right before `fmt`/`sink` - matching the disassembly's own
//  ordering address-for-address, and confirming (via the reused stack
//  slot, not just the register-argument setup) that m_sepvX really is "x"
//  and m_sepvY really is "y", not swapped.
//============================================================================
#include "poseparate.h"
#include "textsink.h"

namespace NIpse {

void CPoSeparate::DumpDebugState(void* sink)
{
    LogFormatted(sink, "\t[PoSe]");

    LogFormatted(sink, "\t\tsepv x[%s] y[%s] opt[0x%08X]",
                 FormatCFloat(&m_sepvX, 1, -1),
                 FormatCFloat(&m_sepvY, 1, -1),
                 m_opt);
}

}  // namespace NIpse
