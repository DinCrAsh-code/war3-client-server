//============================================================================
//  Explicit instantiations for the two intrusive lists and the one growable
//  array this batch's classes carry as members and no other translation unit
//  names.
//
//  None of these has a `### ... @ 0x...` heading of its own in the dump and
//  none is reached from any function in this batch: they exist because a
//  member with a destructor makes the compiler emit a call to it wherever the
//  *containing* class is destroyed, and `verify.py` never links, so the only
//  thing that finds them is tools/link_check.py.  The bodies are the shared
//  templates', unchanged.
//
//  TSList<void,0>, TSList<FRAMENODE,4> and TSList<LIGHTNODE,0> are the other
//  three and live in framelist.cpp and framenodelist.cpp.
//============================================================================
#include "tsfixedlist.inl"
#include "tsarray.inl"
#include "frame.h"

template void TSList<REGIONNODE, 0>::UnlinkAll();
template void TSList<SIMPLEFRAMENODE, 0>::UnlinkAll();
template TSGrowableArray<TSExplicitList<SHORTCUTREGNODE> >::~TSGrowableArray();
