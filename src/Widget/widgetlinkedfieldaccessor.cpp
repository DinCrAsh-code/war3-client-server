//============================================================================
//  0x6F50C350 - reached from CWidget::Load.  `this` in ecx, no stack
//  arguments (`retn`, i.e. `retn 0`).
//
//  If this+0x98 is null, return 0.  Otherwise the shipped body's own
//  `jmp` lands on an unlabelled two-instruction tail with no dump heading
//  of its own (`dumpfn.py 0x6F514510` reports NO DUMP) - unlike this same
//  closure's sub_6F501D50, whose own tail-jump target IS a named foreign
//  symbol (Concurrency::SchedulerBase::GetNumberOfBoundContexts) and stays
//  a redirect for exactly that reason.  This one is read as the shipped
//  compiler's own tail placement of this function's own continuation
//  (nothing else claims that address), so it is transcribed as ordinary
//  C++ rather than left a thunk: return this->field98->field44's own
//  +0x1C dword.
//============================================================================
struct SWidgetLoadLinkedRecord
{
    char  m_reserved00[0x44];
    void* m_field44;        // +0x44
};

struct SWidgetWithLoadLinkedRecord
{
    int GetLinkedRecordField1C(void);

    char                       m_reserved00[0x98];
    SWidgetLoadLinkedRecord*   m_field98;   // +0x98
};

int SWidgetWithLoadLinkedRecord::GetLinkedRecordField1C(void)
{
    SWidgetLoadLinkedRecord* linked = m_field98;
    if (linked == 0)
        return 0;

    return *(int*)((char*)linked->m_field44 + 0x1C);
}
