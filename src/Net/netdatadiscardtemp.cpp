//============================================================================
//  0x6F53E2A0 - throw away whatever temporary game-data file the replay or
//  record stream left behind, and put both state machines back to 0.
//
//  The name of the file is copied out of the stream *before* the stream is
//  abandoned - it is the stream's own field and would not survive - which is
//  why the 0x104-byte buffer is there at all.
//
//  Both state resets are followed by a slot-7 call on the sub-object the
//  state belongs to (+0x618 for replay, +0xBE8 for record), which is the
//  same "start again at the front" slot `Net::CGameDescStore` uses.
//
//  The two trailing stores and the `return 1` are unconditional: there is
//  nothing to discard when no stream is selected, and that is not an error.
//============================================================================
#include "netdata.h"
#include "os.h"                 // OsDeleteFile
#include "gamedatawriter.h"     // SGameDataWriter::Abandon, 0x6F535A90

//  0x6F537B20 - Misc/trivialpredicates.cpp.
void* __fastcall GetSelectedSubObject_6F537B20(void* self);

//  0x6F534A00 - Net/datastorereadfixedname.cpp.
struct SNamedDataStream
{
    void CopyName(char* dest, unsigned int size);
    char m_reserved000[0x104];
    char m_name[0x104];
};

struct SNetDataStates
{
    int SetReplayState(unsigned int state);
    int SetRecordState(unsigned int state);
};

//  Slot 8 on the stream ("finish whatever is open"), and slot 7 on each of
//  the two sub-objects ("start again at the front").  Hand-written
//  dispatches: neither class has a vtable this build can declare.
typedef void (__thiscall* StreamFinishFn)(void* self);
typedef void (__thiscall* SubObjectRewindFn)(void* self);

int __fastcall NetDataDiscardTempFile(void* self)
{
    char* p = (char*)self;

    void* stream = GetSelectedSubObject_6F537B20(self);
    if (stream != 0)
    {
        char fileName[0x104];
        ((SNamedDataStream*)stream)->CopyName(fileName, 0x104);

        if (*(const int*)((char*)stream + 0x14) == -1)
            ((StreamFinishFn)(*(void***)stream)[0x20 / 4])(stream);

        ((SGameDataWriter*)stream)->Abandon();
        OsDeleteFile(fileName);

        ((SNetDataStates*)self)->SetReplayState(0);
        ((SubObjectRewindFn)(*(void***)(p + 0x618))[0x1C / 4])(p + 0x618);

        ((SNetDataStates*)self)->SetRecordState(0);
        ((SubObjectRewindFn)(*(void***)(p + 0xBE8))[0x1C / 4])(p + 0xBE8);
    }

    p[0xAD8] = 0;
    *(int*)(p + 0xBDC) = 0;
    return 1;
}
