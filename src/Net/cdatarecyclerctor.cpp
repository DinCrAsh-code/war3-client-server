//============================================================================
//  0x6F4C65C0 and 0x6F4C6610 - CDataRecycler's two constructors.
//
//  The two-argument one clamps both bounds to at least one and then clamps
//  the live count to the limit, and it writes the count *twice* - once
//  before the test and once in the arm that keeps it - which is what a clamp
//  written as two chained selects comes out as (docs/msvc-vc8-idioms.md, "A
//  clamp written as two chained selects duplicates the tail").
//
//  The limit is tested signed (`jg`) and the count unsigned (`ja`), which is
//  what fixes their two types.
//============================================================================
#include "cdatarecycler.h"

CDataRecycler::~CDataRecycler()
{
    m_vtable = (void*)g_vftableCDataRecycler;
}

CDataRecycler::CDataRecycler(unsigned int count, int limit)
{
    m_vtable = (void*)g_vftableCDataRecycler;
    m_field0C = 0;
    m_field10 = 0;
    m_field14 = 0;

    if (limit <= 1)
        limit = 1;
    m_limit = limit;

    if (count <= 1)
        count = 1;

    m_count = count;
    if (count >= (unsigned int)limit)
        m_count = (unsigned int)limit;
}
