//============================================================================
//  The render device singleton, and the handful of its accessors this
//  closure reaches.
//
//  `dword_6FACBD40` holds the device pointer; every one of the seven
//  functions below opens `mov ecx, dword_6FACBD40`.  Nothing here names a
//  class from RTTI - the device object carries no descriptor this closure
//  reaches - so the name is what the object does: it hands out the caps
//  block, the view and projection matrices, the viewport, and it is the
//  thing texture creation is submitted to through vtable slot 0xA8.
//
//  Matrix storage is fmatrix.h's throughout: the 4x4 is four rows of four
//  and the 4x3 four rows of three, the last of which is the translation.
//============================================================================
#ifndef RENDERDEVICE_H
#define RENDERDEVICE_H

//----------------------------------------------------------------------------
//  One dword of load options, and it is a *type* rather than a plain
//  `unsigned int` because of where it lands.  TextureCacheFind takes the
//  name in ecx, never reads edx and cleans four bytes; TextureCacheAdd takes
//  the name in ecx and cleans eight.  `__fastcall` fills ecx and edx before
//  it reaches the stack, so a plain `unsigned int` second parameter goes to
//  edx and both functions clean four bytes too few - a `ret n` mismatch
//  abi_audit calls FATAL.  A small struct by value is not register-eligible
//  under `__fastcall`, so it goes to the stack and both byte counts come
//  out right.
//
//  The five bitfields are the five disjoint masks the key compare tests one
//  at a time (3, 4, 8, 0x10, 0x20), so the dword is a two-bit field and four
//  flags.  What each of them means is *not* recoverable from this closure:
//  0x6F732360 is the only caller that builds one from scratch and all it
//  does is `(ticks & ~0x31) | 0x0E`, which is also why the word itself has
//  to stay reachable.
//----------------------------------------------------------------------------
struct TEXTUREOPTIONBITS
{
    unsigned int m_kind     : 2;    // bits 0-1
    unsigned int m_opt2     : 1;    // bit 2
    unsigned int m_opt3     : 1;    // bit 3
    unsigned int m_opt4     : 1;    // bit 4
    unsigned int m_opt5     : 1;    // bit 5
    unsigned int m_reserved : 26;
};

struct TEXTUREOPTIONS
{
    union
    {
        unsigned int      m_word;
        TEXTUREOPTIONBITS m_bits;
    };
};


//----------------------------------------------------------------------------
//  What CRenderDevice::GetCaps hands back: ten dwords out of the device at
//  +0xAC.  Only the fourth is read in this closure - the maximum texture
//  dimension ShrinkTextureToDeviceLimit halves against - so it is the only
//  one with a name.
//----------------------------------------------------------------------------
struct DEVICECAPS
{
    unsigned int m_reserved00[3];   // +0x00
    unsigned int m_maxTextureDim;   // +0x0C
    unsigned int m_reserved10[6];   // +0x10
};

struct FMATRIX43 { float m[12]; };

//  The 4x4.  The converting constructor is 0x6F5268D0 and it is a
//  constructor rather than a free function because of where its operand
//  lands: the shipped body takes the buffer in ecx, the 4x3 as a *stack*
//  dword and answers the buffer in eax with `mov eax, ecx` on entry, which
//  is __thiscall plus MSVC's "a constructor returns this" - no free
//  __fastcall spelling reaches it (fmatrix.h's three all take their first
//  operand in edx).
//
//  There is deliberately *no* default constructor: a trivial one is still
//  emitted as its own COMDAT, and it would carry the identical readable name
//  `FMATRIX44::FMATRIX44` into build/*.asm, where it shadows the real body
//  the funcmap row points at (measured: 0x6F5268D0 fell from 1.000 to 0.057
//  against a two-instruction empty constructor).  Nothing here ever
//  default-constructs one; the buffers are plain `float[16]`.
struct FMATRIX44
{
    FMATRIX44(const FMATRIX43& m43);    // 0x6F5268D0

    float m[16];
};

//----------------------------------------------------------------------------
//  The one four-dword scratch descriptor the solid-colour texture path
//  builds: a header dword, two pairs the constructor clears in one two-turn
//  loop, and a trailing dword.  The +0x0C pair's first element is the only
//  thing read back out (0x6F4E3C80 puts it in the texture's +0x0C).
//----------------------------------------------------------------------------
//  One colour, and a struct rather than an `unsigned int` for the same
//  reason TEXTUREOPTIONS is one: 0x6F7B8660 takes the image descriptor in
//  ecx and cleans four bytes, which a plain `unsigned int` second parameter
//  would put in edx.  The shipped call site is the other half of the tell -
//  it reserves the argument slot with a `push` and writes the value through
//  a pointer, which is what MSVC emits for a class passed by value and never
//  for an int.
struct TEXTURECOLOR
{
    unsigned int m_argb;
};

struct TEXTUREIMAGE
{
    //  0x6F526F70 - Render/textureimagector.cpp.
    TEXTUREIMAGE();

    int m_kind;         // +0x00
    int m_source[2];    // +0x04
    int m_result[2];    // +0x0C
    int m_extra;        // +0x14
};

class CRenderDevice
{
public:
    //  0x6F527260 - hand back the caps block by value.
    DEVICECAPS GetCaps();
    //  0x6F527310 - the projection matrix, out of +0xFC.
    void GetProjectionMatrix(FMATRIX44* out);
    //  0x6F527330 - the view matrix, out of +0x13C.
    void GetViewMatrix(FMATRIX43* out);
    //  0x6F528200 - the viewport rectangle and its two depth planes.
    void GetViewport(float* outX, float* outY, float* outW, float* outH,
                     float* outNear, float* outFar);

    char       m_reserved00[0xAC];  // +0x00
    DEVICECAPS m_caps;              // +0xAC
    char       m_reservedD4[0x10];  // +0xD4
    float      m_viewport[6];       // +0xE4
    FMATRIX44  m_projection;        // +0xFC
    FMATRIX43  m_view;              // +0x13C
};

//: dword_6FACBD40 - the device pointer, not the device.
extern CRenderDevice* g_renderDevice;

//  0x6F526290 - the caps block, by value, from the singleton.
DEVICECAPS __stdcall GetDeviceCaps();
//  0x6F526A40 - view * projection, sixteen floats into the caller's buffer.
void __fastcall GetViewProjectionMatrix(float* outMatrix);
//  0x6F5266D0 - the viewport, forwarded to the singleton.
void __fastcall GetViewportRect(float* outX, float* outY, float* outW,
                                float* outH, float* outNear, float* outFar);
//  0x6F526510 - submit one texture creation to the device, through its own
//  vtable slot 0xA8.  Seven arguments go down; the eighth thing the shipped
//  code does is zero the caller's result slot first.
int __fastcall SubmitTextureCreate(unsigned int width, unsigned int height,
                                   int format, TEXTUREOPTIONS options, void* texture,
                                   const void* codec, void** outImage);

#endif
