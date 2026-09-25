//============================================================================
//  `.\parser.cpp`'s own compile-buffer pair - two `SMemFree`'d pointers at
//  +0x0C/+0x10 of whatever object owns them.  Nothing in this closure reaches
//  the object's allocation site or any other member, so only the two touched
//  fields are named; everything ahead of them is an untouched gap.
//============================================================================
#ifndef JASSPARSERBUFFERS_H
#define JASSPARSERBUFFERS_H

struct SJassParserBuffers
{
    char  m_reserved00[0x0C];
    void* m_buffer0C;   // +0x0C
    void* m_buffer10;   // +0x10

    //  0x6F4607F0 - `retn 0`.  Frees both buffers (`.\parser.cpp` lines
    //  0x3CB/0x3CC) and re-zeroes +0x0C; +0x10 is left holding its freed
    //  pointer, which is what the shipped code does too - only the first
    //  field is re-cleared.
    void ReleaseBuffers();
};

#endif
