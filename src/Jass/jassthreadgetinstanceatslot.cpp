//============================================================================
//  0x6F449BD0 - JassThreadLocal's own +0x90 handle array (m_handleArrayData),
//  indexed.  Own TU for the same reason jassthreadgetfield48.cpp's sibling
//  GetScript() is: so a caller in a different TU emits a real call instead
//  of the compiler inlining this one-line body straight through.
//============================================================================
#include "storm.h"
#include "jassthreadstate.h"
#include "jassinstance.h"

//  0x6F449BD0
JassInstance* JassThreadLocal::GetInstanceAtSlot(unsigned int index)
{
    return m_handleArrayData[index];
}
