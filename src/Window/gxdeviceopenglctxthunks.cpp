//============================================================================
//  Naked redirects for CGxDeviceOpenGl_AcquireGLContext/
//  CGxDeviceOpenGl_ReleaseGLContext's own deep OpenGL callees - pixel
//  format selection, context creation, extension-string parsing and GL
//  state-block/texture-pool teardown.  None of this is input routing, the
//  actual subject of this session's task, and each is a genuinely separate
//  chunk of work (a real pixel-format negotiation, a real extension-string
//  parser with its own jump table, a ~130-instruction state-block init) -
//  CLAUDE.md's own "thunk a deep, boring, generic branch and move on"
//  allowance, same as every prior session in this investigation has used.
//
//  All four `retn` bare (0 stack bytes) per their own dumps
//  (asm/sub_6F531D90_*.md, asm/sub_6F5314F0_*.md, asm/sub_6F531600_*.md,
//  asm/sub_6F5328A0_*.md - all pulled this session, all still in `asm/`),
//  so every declaration below pushing zero stack arguments is exactly
//  right for tools/thunk_abi_audit.py.
//============================================================================
#include "gxdeviceopengl.h"

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }               \
    }

ADDR_THUNK(void __fastcall CGxDeviceOpenGl_DestroyTextures(CGxDeviceOpenGl*),
           0x6F531D90)
ADDR_THUNK(HGLRC __fastcall CGxDeviceOpenGl_CreateGLContext(HWND),
           0x6F5314F0)
ADDR_THUNK(void CGxDeviceOpenGl_ParseGLExtensions(), 0x6F531600)
ADDR_THUNK(void __fastcall CGxDeviceOpenGl_InitStateBlock(CGxDeviceOpenGl*),
           0x6F5328A0)

#undef ADDR_THUNK
