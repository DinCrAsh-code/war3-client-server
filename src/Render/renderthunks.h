//============================================================================
//  What the text and file-cache modules reach that is below this closure's
//  boundary - `docs/notes/pausegame-closure-boundary.md`'s list.  Each is a
//  `mov eax,<addr>` / `jmp eax` redirect in renderthunks.cpp with the shipped
//  `retn <n>` written next to it, and each goes away when its own module is
//  reconstructed.
//============================================================================
#ifndef RENDERTHUNKS_H
#define RENDERTHUNKS_H

//  0x6F7B8FE0 (`retn 4`) - apply a colour to a laid-out text block.  Reached
//  only through 0x6F4CC470, whose whole body is the forward.
void __fastcall TextEngineTintBlock(void* block, const unsigned int* color,
                                    const float* offsets);

//  0x6F7BA460 (`retn 8`) - build the engine-side font object for a name at a
//  height, into the caller's slot.  Answers zero when the face is not there.
int __fastcall TextEngineCreateFont(const char* name, void* out,
                                    float height, int flags);

//  0x6F7BA790 (`retn 2Ch`) - lay a string out.  Eleven stack arguments plus
//  the font in ecx and the text in edx; the argument count is read off the
//  eleven pushes 0x6F4CCD70 makes and never pops.
void __fastcall TextEngineLayout(void* font, const char* text,
                                 float height, float* position,
                                 float boxWidth, float boxHeight,
                                 float extra, void* out,
                                 int verticalAlign, int horizontalAlign,
                                 int flags, const void* color, float depth);

//----------------------------------------------------------------------------
//  What `.\Texture.cpp` and `.\blp.cpp` reach below the boundary.
//----------------------------------------------------------------------------
//  0x6F7B8660 (`retn 4`) - build the engine-side one-pixel image for a
//  colour, into the caller's descriptor.  The colour is a TEXTURECOLOR by
//  value, which is what puts it on the stack where the callee cleans it.
int __fastcall CreateSolidTextureImage(struct TEXTUREIMAGE* image,
                                       struct TEXTURECOLOR color);

//  0x6F7B40B0 (`retn 0`) - how many mip levels a width by height has.
int __fastcall BlpMipCount(unsigned int width, unsigned int height);

//  The two decode callbacks the texture creation is handed.  Both are
//  `retn 14h` with a receiver in ecx and a second argument in edx, and
//  neither is ever called from here - only its address is taken - so the
//  parameter list exists to make the byte count right.
//  0x6F4DF170 is the `.tga` one and 0x6F4DF3A0 the `.blp` one; IDA never
//  made a function of the first, which is why the dump calls it
//  `byte_6F4DF170`.
int __fastcall TgaTextureDecodeStep(void* a, void* b, int c, int d, int e,
                                    int f, int g);
int __fastcall BlpTextureDecodeStep(void* a, void* b, int c, int d, int e,
                                    int f, int g);

//  0x6F7E5B00 (`retn 8`) - hand one particle emitter its texture.  Below the
//  boundary: it is in the statically-linked region the closure stops at.  A
//  member rather than a free `__fastcall` because of the byte count: the
//  receiver is in ecx and *both* arguments are on the stack.
struct PARTICLEEMITTER
{
    void SetTexture(int id, int alphaMode);
};

//  0x6F7B86C0 (`retn 2Ch`) - the triangle-list intersection: eleven stack
//  arguments plus the ray origin in ecx and its direction in edx.
int __fastcall TriangleListPick(const float* origin, const float* direction,
                                void* record, void* positions, void* indices,
                                int stride, void* matrices, int singleMatrix,
                                int kind, unsigned int count, void* vertices,
                                float* outParameter, float* outDistance);

//  0x6F7B7400 (`retn 0Ch`) - the bounding-sphere test: the ray origin in
//  ecx, its direction in edx, the sphere, the radius and an out slot.
int __fastcall SpherePick(const float* origin, const float* direction,
                          const float* sphere, float radius, float* outT);

//  0x6F7B7810 (`retn 1Ch`) - one collision shape against the ray: the ray
//  origin in ecx, its direction in edx and seven stack arguments.
int __fastcall ShapePick(const float* origin, const float* direction,
                         void* a, void* b, int kind, void* c, void* d,
                         float* outT, void* outHit);

#endif
