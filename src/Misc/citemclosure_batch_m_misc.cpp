//============================================================================
//  CItem-vtable-closure batch M: three unrelated leftovers this BFS depth
//  reaches - a font-host handle resolver (a callee of Frame/
//  messageframethunks.cpp's own SFrameFontHost::SetFont redirect), the
//  CMessageFrame constructor body Frame/messageframethunks.cpp's own
//  MakeMessageFrame wraps, and a CGx text-engine font object builder from
//  the same subsystem as Render/renderthunks.cpp's own
//  TextEngineCreateFont.  None of it CItem/CSelectable positioning or
//  selection logic.
//
//  0% instruction match by construction - see each BEHAVIOUR entry in
//  tools/funcmap.py.
//============================================================================

#define BATCHM_THUNK(sig, addr)        \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//----------------------------------------------------------------------------
//  0x6F5FAED0 - builds a two-bit flag word out of a caller-supplied byte
//  (bit 0 -> flag bit 0, bit 1 -> flag bit 1) and forwards it plus a
//  caller-supplied `this` and a stack float (the font height) to
//  sub_6F4CDE70 (out of this batch's scope) - the "get-or-create font
//  handle" helper SFrameFontHost::SetFont (Frame/messageframethunks.cpp,
//  0x6F5FB960) calls after releasing its own old handle.  `retn 4`.
//----------------------------------------------------------------------------
BATCHM_THUNK(void __stdcall ResolveFontHostHandle(int), 0x6F5FAED0)

//----------------------------------------------------------------------------
//  0x6F623D60 - CMessageFrame's own constructor body: stamps
//  `??_7CMessageFrame@@6B@` / its `_0` twin, forwards to a base constructor
//  (sub_6F608200), allocates two small ".\\CMessageFrame.cpp"-tagged
//  records through Storm_401, and zero/poison-initialises every
//  TSExplicitList<TEXTMSG>/TSExplicitList<TIMERDATA> member
//  Frame/messageframe.h's own CMessageFrame declares
//  (m_messages/m_display/m_timers) plus two more sub-objects this repo does
//  not yet model - the shape MakeMessageFrame (0x6F623ED0,
//  Frame/messageframethunks.cpp) wraps with its own SEH frame and `new`.
//  `retn 0Ch`.
//----------------------------------------------------------------------------
BATCHM_THUNK(void __stdcall CMessageFrame_InitializeFields(int, int, int),
             0x6F623D60)

//----------------------------------------------------------------------------
//  0x6F7B9ED0 - allocates a Storm_401-tagged "AUCGXFONT..." record (a
//  `CGxFont`-shaped engine text object, same text-engine subsystem as
//  Render/renderthunks.cpp's own TextEngineCreateFont), initialises it
//  through sub_6F7C4FC0 when the allocation succeeds, and, given a non-null
//  caller argument, binds a face onto it via sub_6F7B9990.  SEH-framed.
//  `retn 0Ch`.
//----------------------------------------------------------------------------
BATCHM_THUNK(void __stdcall CreateGxFontObjectAndBindFace(int, int, int),
             0x6F7B9ED0)

#undef BATCHM_THUNK
