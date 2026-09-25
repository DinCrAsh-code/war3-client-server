//============================================================================
//  See renderthunks.h.  Nothing in this file is a reconstruction.
//
//  Spelled as a *trailing* macro so tools/thunk_inventory.py counts every one
//  of them: its CALL_THUNK pattern wants `NAME_THUNK(0x<addr>)` after the
//  signature.
//============================================================================
#include "renderthunks.h"
#include "filecache.h"
#include "imagefile.h"
#include "renderdevice.h"

#define RENDER_THUNK(addr)       \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

__declspec(naked) void __fastcall TextEngineTintBlock(void*,
                                                      const unsigned int*,
                                                      const float*) RENDER_THUNK(0x6F7B8FE0)

__declspec(naked) int __fastcall TextEngineCreateFont(const char*, void*,
                                                      float, int) RENDER_THUNK(0x6F7BA460)

__declspec(naked) void __fastcall TextEngineLayout(void*, const char*, float,
                                                   float*, float, float,
                                                   float, void*, int, int,
                                                   int, const void*,
                                                   float) RENDER_THUNK(0x6F7BA790)

__declspec(naked) int __fastcall CreateSolidTextureImage(struct TEXTUREIMAGE*,
                                                          struct TEXTURECOLOR) RENDER_THUNK(0x6F7B8660)

__declspec(naked) int __fastcall BlpMipCount(unsigned int,
                                             unsigned int) RENDER_THUNK(0x6F7B40B0)

__declspec(naked) int __fastcall TgaTextureDecodeStep(void*, void*, int, int,
                                                      int, int,
                                                      int) RENDER_THUNK(0x6F4DF170)

__declspec(naked) int __fastcall BlpTextureDecodeStep(void*, void*, int, int,
                                                      int, int,
                                                      int) RENDER_THUNK(0x6F4DF3A0)

__declspec(naked) void PARTICLEEMITTER::SetTexture(int, int) RENDER_THUNK(0x6F7E5B00)

__declspec(naked) int __fastcall TriangleListPick(const float*, const float*,
                                                  void*, void*, void*, int,
                                                  void*, int, int,
                                                  unsigned int, void*, float*,
                                                  float*) RENDER_THUNK(0x6F7B86C0)

__declspec(naked) int __fastcall SpherePick(const float*, const float*,
                                            const float*, float,
                                            float*) RENDER_THUNK(0x6F7B7400)

__declspec(naked) int __fastcall ShapePick(const float*, const float*, void*,
                                           void*, int, void*, void*, float*,
                                           void*) RENDER_THUNK(0x6F7B7810)

//  0x6F7B2F90 / 0x6F7B2F30 - open and close a `.tga`.  Declared on CTgaFile
//  in imagefile.h so the reader is one class rather than two free functions.
__declspec(naked) int CTgaFile::Open(const char*) RENDER_THUNK(0x6F7B2F90)

__declspec(naked) void CTgaFile::Close() RENDER_THUNK(0x6F7B2F30)

//  0x6F6D7E90 - `WaitForSingleObject(m_handle, INFINITE)`.  IDA models it as
//  a tail chunk of 0x6F4BC650 rather than as a function of its own, which is
//  also why it has no `### … @ 0x…` heading and why PrefetchNode::WaitReady's
//  denominator carries its four instructions.
__declspec(naked) void SLoadEvent::Wait() RENDER_THUNK(0x6F6D7E90)
