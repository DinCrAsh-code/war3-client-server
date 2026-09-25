//============================================================================
//  The object-data owner's two members 0x6F3F4A30 uses.  A header for the
//  same reason gamedatawriter.h is one.
//============================================================================
#ifndef OBJECTDATAOWNER_H
#define OBJECTDATAOWNER_H

struct SObjectDataOwner
{
    //  0x6F3E94D0 - not in this tree; redirected.
    void  Register(int kind, void** entry);

    //  0x6F3F4A30.
    void* FindAndRegister(const void* key);
};

#endif
