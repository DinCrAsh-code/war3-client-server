//============================================================================
//  0x6F612CB0 - CSimpleGrid::CSimpleGrid.  No EH frame: its base constructor
//  is the only call.
//
//  0x6F612AA0 - SGridRowArray::Destruct: free every row's own cell array
//  (each a `CSimpleFrame*[]`, tagged ".PAVCSimpleFrame@@" the way
//  Storm/storm.h's own SMemFree wrapper expects) and then the row array
//  itself (tagged ".?AV?$TSGrowableArray@PAVCSimpleFrame@@@@" - both strings
//  read verbatim off the shipped body with `worktree_store.py resolve`,
//  not re-derived from the type they nominally name; a Storm-array free tag
//  identifies the *allocating template instantiation*, and this one is
//  shared with an actual `TSGrowableArray<CSimpleFrame*>` elsewhere, not
//  proof this row array is one).
//
//  0x6F612D30 - CSimpleGrid::~CSimpleGrid: the compiler stamps its own
//  vtable at entry automatically (the usual destructor-of-a-polymorphic-
//  class shape - nothing to write for that part), then the shipped stream
//  calls Destruct() explicitly, zeroes the three fields, and calls
//  Destruct() again on the now-empty array - a real, harmless double call
//  the shipped body actually makes (source written that way, not a compiler
//  artifact modelled here as one), before tail-jumping into the base,
//  ~CSimpleFrame (0x6F609DD0, still a thunk: this batch's own scope stops
//  at CSimpleGrid, and reconstructing a base class destructor it merely
//  calls would be a much larger target).
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleGrid::CSimpleGrid(void* parent)
    : CSimpleFrame(parent)
{
    m_12C = 0.0f;
    m_130 = 0.0f;
    m_134 = 0.0f;
    m_138 = 0.0f;
    m_13C = 0.0f;
    m_140 = 0.0f;
    m_144 = 0.0f;
    m_148 = 0.0f;
    m_rowArray.m_alloc = 0;
    m_rowArray.m_count = 0;
    m_rowArray.m_rows = 0;
    m_rowArray.m_chunk = 0;
    m_rowCount = 0;
    m_columnCount = 0;
}

//  0.896 (30/34) as written: the shipped stream re-reads `m_rows` fresh
//  every iteration and adds a running byte offset to it as two separate
//  instructions (add, then a displaced load), where this body's own
//  `m_rows[i].m_cells` folds into one base+index+disp addressing mode - a
//  scheduling-only gap (a byte-offset-accumulator rewrite was tried and
//  scored worse, 0.806, by also losing the outer bounds-check's own
//  register); see funcmap.py's BEHAVIOUR entry.
void SGridRowArray::Destruct()
{
    for (unsigned int i = 0; i < m_count; ++i)
    {
        CSimpleFrame** cells = m_rows[i].m_cells;
        if (cells != 0)
            SMemFree(cells, ".PAVCSimpleFrame@@", -2, 0);
    }
    if (m_rows != 0)
        SMemFree(m_rows, ".?AV?$TSGrowableArray@PAVCSimpleFrame@@@@", -2, 0);
}

//  ~CSimpleGrid's own base-destructor call is not written out here: frame.h
//  now declares a real `CSimpleFrame::~CSimpleFrame()` (thunked in
//  Frame/simpleframe.cpp), so the compiler's own *implicit* base-destructor
//  call - emitted regardless, once any base up the chain has a non-trivial
//  destructor - reaches that real target and tail-jumps into it on its own.
//  An explicit second call here was tried first and cost this function a
//  duplicate call the shipped stream does not have.
CSimpleGrid::~CSimpleGrid()
{
    m_rowArray.Destruct();
    m_rowArray.m_alloc = 0;
    m_rowArray.m_count = 0;
    m_rowArray.m_rows  = 0;
    m_rowArray.Destruct();
}
