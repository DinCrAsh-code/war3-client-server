//============================================================================
//  g_indexedRecordArray_6FAB4E90 / g_indexedRecordCount_6FAB4E74 - a
//  pointer array and its own element count, reached from createunit-G2's
//  own dump.  Not otherwise identified.
//============================================================================
#ifndef INDEXEDRECORDARRAY_H
#define INDEXEDRECORDARRAY_H

extern void** g_indexedRecordArray_6FAB4E90;
extern int   g_indexedRecordCount_6FAB4E74;

//  0x6F2DEC10 - indexedrecordarray.cpp.
void* __fastcall GetIndexedRecord_6FAB4E90(int index);

#endif
