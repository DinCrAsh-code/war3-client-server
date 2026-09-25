//============================================================================
//  0x6F40FB30 - the +0x130 and +0x140 tenths, added.
//
//  Its own translation unit: the shipped body calls both of the accessors
//  in playerscore_handlefields.cpp for real, and at four instructions each
//  /Ob2 would inline them on sight if the definitions were visible.
//============================================================================
#include "playerscore.h"

unsigned int SPlayerScore::QueryTenth130Plus140()
{
    unsigned int first = QueryTenth130();
    return QueryTenth140() + first;
}
