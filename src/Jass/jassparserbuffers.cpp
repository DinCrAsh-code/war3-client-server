//============================================================================
//  See jassparserbuffers.h.
//============================================================================
#include "jassparserbuffers.h"
#include "storm.h"

//  0x6F4607F0
void SJassParserBuffers::ReleaseBuffers()
{
    SMemFree(m_buffer0C, ".\\parser.cpp", 0x3CB, 0);
    SMemFree(m_buffer10, ".\\parser.cpp", 0x3CC, 0);
    m_buffer0C = 0;
}
