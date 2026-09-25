//============================================================================
//  0x6F6D8410 (IDA's `unknown_libname_949`) - Event's implicit
//  default-member-initialiser. Its own translation unit so the one call
//  site (Event::Event, event.cpp) keeps calling it rather than inlining a
//  three-instruction body away.
//============================================================================
#include "event.h"

void Event::ZeroHandle()
{
    m_handle = 0;
}
