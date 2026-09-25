//============================================================================
//  zlib 1.1.4, as it is statically linked into Game.dll.
//
//  Recovered, not invented.  Four things pin the version down and there is no
//  guesswork left in the layout:
//
//    * deflateEnd's three accepted states are 0x2A, 0x71 and 0x29A - zlib's
//      INIT_STATE (42), BUSY_STATE (113) and FINISH_STATE (666) - and it
//      frees pending_buf, head, prev and window *in that order*, which is
//      deflate.c's own TRY_FREE sequence;
//    * the deflate_state offsets it touches (status +0x04, pending_buf +0x08,
//      window +0x30, prev +0x38, head +0x3C) are deflate.h's struct laid out
//      for a 32-bit build, field for field;
//    * inflate_blocks_state carries a `hufts` pointer at +0x24, which exists
//      in 1.1.x and not in 1.0.x;
//    * inflate_blocks_reset's two frees are keyed on mode 4/5 and mode 6 -
//      BTREE, DTREE and CODES in infblock.c's own enum order.
//
//  The one thing that is *not* stock zlib is the calling convention: every
//  function here takes its first two arguments in ecx and edx and the rest on
//  the stack, and so do the zalloc/zfree/checkfn pointers it calls through.
//  The module was built with /Gr, so `__fastcall` is what the whole file is
//  spelled with - including the three function-pointer typedefs, where the
//  convention is part of the type and getting it wrong would push the
//  arguments into the wrong registers at every indirect call.
//============================================================================
#ifndef ZLIBINT_H
#define ZLIBINT_H

typedef unsigned char  Bytef;
typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef void*          voidpf;

//  Return codes.  Only the three this closure produces are named.
#define Z_OK            0
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)

typedef voidpf (__fastcall *alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void   (__fastcall *free_func)(voidpf opaque, voidpf address);
typedef uLong  (__fastcall *check_func)(uLong check, const Bytef* buf,
                                        uInt len);

struct internal_state;

//----------------------------------------------------------------------------
//  z_stream.  The public struct, unchanged.
//----------------------------------------------------------------------------
struct z_stream
{
    Bytef*                  next_in;        // +0x00
    uInt                    avail_in;       // +0x04
    uLong                   total_in;       // +0x08
    Bytef*                  next_out;       // +0x0C
    uInt                    avail_out;      // +0x10
    uLong                   total_out;      // +0x14
    char*                   msg;            // +0x18
    struct internal_state*  state;          // +0x1C
    alloc_func              zalloc;         // +0x20
    free_func               zfree;          // +0x24
    voidpf                  opaque;         // +0x28
    int                     data_type;      // +0x2C
    uLong                   adler;          // +0x30
    uLong                   reserved;       // +0x34
};

typedef z_stream* z_streamp;

//  zlib's own two allocator macros, spelled exactly as zlib spells them so
//  that the argument order at every call site is the shipped one.
#define ZALLOC(strm, items, size) \
    (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr) \
    (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) { if (p) ZFREE(s, p); }

//----------------------------------------------------------------------------
//  deflate.c's deflate_state.  Only the five fields deflateEnd reads are
//  named; the rest are reserved blocks, per the house rule.
//----------------------------------------------------------------------------
struct deflate_state
{
    z_streamp   strm;               // +0x00
    int         status;             // +0x04
    Bytef*      pending_buf;        // +0x08
    char        m_reserved0C[0x30 - 0x0C];
    Bytef*      window;             // +0x30
    uLong       window_size;        // +0x34
    void*       prev;               // +0x38  Posf*
    void*       head;               // +0x3C  Posf*
};

#define INIT_STATE      42
#define BUSY_STATE      113
#define FINISH_STATE    666

//----------------------------------------------------------------------------
//  infcodes.c's state.  Nothing in this closure looks inside one; only its
//  address is ever used, to free it.
//----------------------------------------------------------------------------
struct inflate_codes_state;
typedef struct inflate_codes_state inflate_codes_statef;

//----------------------------------------------------------------------------
//  infblock.c's state.  The union is written out as the overlapping fields
//  it really is: `left`, the trees arm and the decode arm all start at +0x04.
//----------------------------------------------------------------------------
struct inflate_blocks_state
{
    uInt        mode;               // +0x00
    union
    {
        uInt    left;                                   // +0x04 if STORED
        struct
        {
            uInt                    table;              // +0x04
            uInt                    index;              // +0x08
            uInt*                   blens;              // +0x0C
            uInt                    bb;                 // +0x10
            void*                   tb;                 // +0x14
        } trees;
        struct
        {
            inflate_codes_statef*   codes;              // +0x04
        } decode;
    } sub;
    uInt        last;               // +0x18
    uInt        bitk;               // +0x1C
    uLong       bitb;               // +0x20
    void*       hufts;              // +0x24
    Bytef*      window;             // +0x28
    Bytef*      end;                // +0x2C
    Bytef*      read;               // +0x30
    Bytef*      write;              // +0x34
    check_func  checkfn;            // +0x38
    uLong       check;              // +0x3C
};

typedef struct inflate_blocks_state inflate_blocks_statef;

//  infblock.c's mode enum, as far as this closure needs it.
#define IBS_TYPE    0
#define IBS_BTREE   4
#define IBS_DTREE   5
#define IBS_CODES   6

//----------------------------------------------------------------------------
//  inflate.c's internal_state.
//----------------------------------------------------------------------------
struct internal_state
{
    uInt                    mode;       // +0x00
    uInt                    sub;        // +0x04  method / check / marker
    char                    m_reserved08[0x0C - 0x08];
    int                     nowrap;     // +0x0C
    uInt                    wbits;      // +0x10
    inflate_blocks_statef*  blocks;     // +0x14
};

//----------------------------------------------------------------------------
//  Each of these is its own translation unit: the shipped module emits a real
//  `call` from inflate_blocks_free to inflate_blocks_reset and from
//  inflate_blocks_reset to inflate_codes_free, so a single TU at /Ob2 would
//  inline both away and lose all three functions their match.
//----------------------------------------------------------------------------
int  __fastcall deflateEnd(z_streamp strm);                     // 0x6F6B2B70
int  __fastcall inflateEnd(z_streamp z);                        // 0x6F6B3C80
int  __fastcall inflate_blocks_free(inflate_blocks_statef* s,
                                    z_streamp z);               // 0x6F6B6B80
void __fastcall inflate_blocks_reset(inflate_blocks_statef* s,
                                     z_streamp z, uLong* c);    // 0x6F6B6050
void __fastcall inflate_codes_free(inflate_codes_statef* c,
                                   z_streamp z);                // 0x6F6B72F0

#endif
