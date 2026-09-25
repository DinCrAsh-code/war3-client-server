//============================================================================
//  The two image-file readers the texture module drives: `.\blp.cpp`'s
//  CBlpFile and the `.tga` one beside it.
//
//  The BLP header is the format's own, and the length is the proof: the
//  constructor memsets 0x49C bytes and writes 'BLP1' at the front, and
//  0x49C is exactly BLP1's seven header dwords plus sixteen mip offsets,
//  sixteen mip sizes and a 256-entry palette.  Every field this closure
//  reads lands where that layout puts it - the alpha-bit count at +0x08
//  (zero means "no alpha", which is what sets the texture's flag bit 0),
//  the dimensions at +0x0C and +0x10, the picture type at +0x14 (which
//  picks the texture format) and the mip flag at +0x18.
//
//  The `.tga` reader is the same shape at the other format: an 18-byte TGA
//  header at +0x08, whose width, height and image-descriptor byte land at
//  +0x14, +0x16 and +0x19 of the object - which is where 0x6F4DF230 reads
//  them.  Bits 0-3 of the descriptor are TGA's alpha-channel bit count, so
//  the same "no alpha" test falls out.
//============================================================================
#ifndef IMAGEFILE_H
#define IMAGEFILE_H

//: The module tag `.\blp.cpp` hands Storm.
extern const char kBlpCpp[];

//----------------------------------------------------------------------------
//  BLP1's on-disk header, palette and all.
//----------------------------------------------------------------------------
struct BLPHEADER
{
    unsigned int m_magic;           // +0x000  'BLP1'
    unsigned int m_compression;     // +0x004
    unsigned int m_alphaBits;       // +0x008
    unsigned int m_width;           // +0x00C
    unsigned int m_height;          // +0x010
    unsigned int m_pictureType;     // +0x014
    unsigned int m_hasMips;         // +0x018
    unsigned int m_mipOffsets[16];  // +0x01C
    unsigned int m_mipSizes[16];    // +0x05C
    unsigned int m_palette[256];    // +0x09C
};

//----------------------------------------------------------------------------
//  TGA's 18-byte header.  Only the three fields the texture path reads are
//  named; the colour-map block in between is left as a gap.
//----------------------------------------------------------------------------
struct TGAHEADER
{
    unsigned char  m_idLength;      // +0x00
    unsigned char  m_colorMapType;  // +0x01
    unsigned char  m_imageType;     // +0x02
    unsigned char  m_reserved03[9]; // +0x03  colour map spec and origin
    unsigned short m_width;         // +0x0C
    unsigned short m_height;        // +0x0E
    unsigned char  m_pixelDepth;    // +0x10
    unsigned char  m_descriptor;    // +0x11  bits 0-3: alpha bit count
};

//----------------------------------------------------------------------------
//  CBlpFile - `.\blp.cpp`.
//----------------------------------------------------------------------------
class CBlpFile
{
public:
    //  0x6F4DDCD0 - Render/blpfilector.cpp.  Out of line, and a real call at
    //  its one call site.
    CBlpFile();
    //  Inline: 0x6F4DF470 has the Release call written out at each of its
    //  exits, and the out-of-line copy the unwind funclet reaches is
    //  0x6F4DDD50, which is not in this closure.
    ~CBlpFile() { Release(); }

    //  0x6F78A740 - Render/blpfilerelease.cpp.
    void Release();
    //  0x6F78A800 - Render/blpfileload.cpp.
    int Load(const char* path, int takeOwnership);

    int          m_reserved00;      // +0x000
    void*        m_scratch;         // +0x004  freed at .\blp.cpp line 0x5B
    BLPHEADER    m_header;          // +0x008 .. +0x4A3
    void*        m_data;            // +0x4A4  the loaded file bytes
    int          m_dataSize;        // +0x4A8
    void*        m_scratch4AC;      // +0x4AC  freed at .\blp.cpp line 0x5E
    char         m_reserved4B0[4];  // +0x4B0
    int          m_budget;          // +0x4B4  0x8000
    int          m_mipCount;        // +0x4B8
    int          m_quality;         // +0x4BC  100
    char         m_path[0x104];     // +0x4C0 .. +0x5C3
    int          m_ownsData;        // +0x5C4  what Release hands the cache
    char         m_reserved5C8[4];  // +0x5C8
    int          m_reserved5CC;     // +0x5CC
    int          m_reserved5D0;     // +0x5D0
    int          m_reserved5D4;     // +0x5D4  1
};

//----------------------------------------------------------------------------
//  The `.tga` reader.  Its constructor is inline - 0x6F4DF230 has the five
//  zero stores written out - and its Open and Close are below this closure's
//  boundary (renderthunks.h).
//----------------------------------------------------------------------------
class CTgaFile
{
public:
    CTgaFile()
    {
        m_data = 0;
        m_size = 0;
        m_reserved1C = 0;
        m_reserved3C = 0;
        m_reserved40 = 0;
    }
    ~CTgaFile() { Close(); }

    //  0x6F7B2F90 / 0x6F7B2F30 - renderthunks.cpp; below the boundary.
    int  Open(const char* path);
    void Close();

    void*        m_data;            // +0x00
    unsigned int m_size;            // +0x04
    TGAHEADER    m_header;          // +0x08
    char         m_reserved1A[2];   // +0x1A
    int          m_reserved1C;      // +0x1C
    char         m_reserved20[0x1C];// +0x20
    int          m_reserved3C;      // +0x3C
    int          m_reserved40;      // +0x40
};

#endif
