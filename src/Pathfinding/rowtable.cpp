//============================================================================
//  0x6F0098F0 - a two-level table lookup: a global "current row" index times
//  two plus the caller's column, into a flat dword table.  -1 in either the
//  row or the column is "nothing selected" and returns 0 without touching
//  the table.  Nothing else in this call tree reaches the table's producer,
//  so its real shape (row stride, element type) is not recoverable beyond
//  "dword, stride 2" - named generically rather than guessed at.
//============================================================================
extern int g_rowTableCurrentRow;    // dword_6FA4F6E8, -1 = none selected
extern int g_rowTableData[];        // dword_6FA4F6F0

int __fastcall RowTableLookup(int column)
{
    int row = g_rowTableCurrentRow;
    if (row == -1)
        return 0;
    if (column == -1)
        return 0;
    return g_rowTableData[column + row * 2];
}
