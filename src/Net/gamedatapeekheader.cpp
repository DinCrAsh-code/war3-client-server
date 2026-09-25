//============================================================================
//  0x6F534A70 - open one saved game just far enough to read its header, and
//  hand back five of its fields.  Every out-pointer is optional and tested
//  separately; the answer is 1 when the header read clean.
//
//  The five fields are +0x30 .. +0x40 of the header, in order, and the first
//  of them arrives in edx rather than on the stack because this is a
//  `__fastcall` free function whose first argument is the path.
//============================================================================
#include "netgameload.h"

int __fastcall GameDataPeekHeader(const char* path, unsigned int* outWord30,
                                  unsigned int* outWord34, unsigned int* outWord38,
                                  unsigned int* outWord3C, unsigned int* outCrc)
{
    NetGameLoadFile file(path);

    SGameLoadHeader header;
    int err = GameDataReadHeader((NetGameLoadSource*)&file, &header);

    if (err == 0)
    {
        if (outWord30) *outWord30 = header.m_word30;
        if (outWord34) *outWord34 = header.m_word34;
        if (outWord38) *outWord38 = header.m_word38;
        if (outWord3C) *outWord3C = header.m_word3C;
        if (outCrc)    *outCrc    = header.m_crc;
    }

    return err == 0;
}
