//============================================================================
//  ActionQueueRecordArrays::FindAndEraseFirst/Second (0x6F663ED0/0x664070) -
//  in their own translation unit, separate from EraseFirst/EraseSecond's
//  own definitions (actionqueuerecord_arrays.cpp), purely so the compiler
//  cannot inline the erase call the way it does when both live in the same
//  TU - the shipped code keeps this as a real call, and CLAUDE.md's own
//  house rule prefers a second TU over __declspec(noinline) for exactly
//  this reason.
//============================================================================

class ActionQueueRecordArrays
{
public:
    void EraseFirst(unsigned int index);
    int FindAndEraseFirst(unsigned char key);

    void EraseSecond(unsigned int index);
    int FindAndEraseSecond(unsigned char key);

    char           m_reserved00[0xC4];
    unsigned int   m_secondBase;    // +0xC4
    unsigned int   m_firstCount;    // +0xC8
    unsigned int   m_secondCount;   // +0xCC
};

//----------------------------------------------------------------------------
//  0x6F663ED0 - linear search the first array for `key`, erasing it on a
//  match.
//----------------------------------------------------------------------------
int ActionQueueRecordArrays::FindAndEraseFirst(unsigned char key)
{
    unsigned int count = m_firstCount;
    if (count <= 0)
        return 0;

    char* p = (char*)this + 4;
    for (unsigned int i = 0; i < count; ++i, p += 0xC)
    {
        if (*p == (char)key)
        {
            EraseFirst(i);
            return 1;
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F664070 - the second array's own twin of FindAndEraseFirst.
//----------------------------------------------------------------------------
int ActionQueueRecordArrays::FindAndEraseSecond(unsigned char key)
{
    unsigned int count = m_secondCount;
    if (count <= 0)
        return 0;

    unsigned int elemOffset = m_secondBase * 3 * 4;
    char* p = (char*)this + elemOffset + 4;
    for (unsigned int i = 0; i < count; ++i, p += 0xC)
    {
        if (*p == (char)key)
        {
            EraseSecond(i);
            return 1;
        }
    }
    return 0;
}
