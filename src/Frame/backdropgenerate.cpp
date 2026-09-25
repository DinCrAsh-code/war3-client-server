//============================================================================
//  0x6F61CD10 - CBackdropGenerator::Generate: build the nine pieces of a
//  frame border against the frame that owns them.
//
//  One block per piece, all nine the same shape: allocate 0xE8 bytes - which
//  is sizeof(CSimpleTexture) - tagged with this module's own file name and
//  the shipped line number, construct a CSimpleTexture into it if the
//  allocation succeeded, anchor it, and give it a path.
//
//  The background is drawn on layer 0 and gated on its own file name being a
//  non-empty string; the other eight are on layer 1 and gated on a bit of
//  m_pieces each - 1, 2, 4, 8 for the four edges and 0x10, 0x20, 0x40, 0x80
//  for the four corners, in that order.  Only the background and the four
//  edges are remembered in a member; the four corners are the frame's from
//  the moment they are anchored to it.
//
//  **The corners share one texture, cut into quarters of its right half.**
//  Each takes a u range out of 0.5, 0.625, 0.75, 0.875, 1.0 - four
//  consecutive eighths - with v running 0 to 1, which is why the four blocks
//  differ only in two constants and an anchor index.
//
//  Two things in here are the shipped code's own oddities and are
//  deliberately reproduced:
//
//   * the background's **bottom-right** anchor offsets by `m_insetTop`
//     where its bottom-left uses `m_insetBottom` - `fld [esi+40h]` at
//     0x6F61CE15 against `fld [esi+44h]` at 0x6F61CDF7.  Every other corner
//     is symmetric, so this reads as a copy-paste in the original source;
//     the four insets are all set to the same value by the one caller in
//     this closure, so nothing observes it.
//   * the edge and corner blocks re-read the border file name from +0x20
//     for every piece rather than hoisting it, and each falls back to the
//     empty string (`ValueName`, 0x6F87529C - four zero bytes) on its own.
//
//  Score capped by the unified `__except_handler4`-shaped EH frame: the
//  shipped function carries nine `__try`/`__finally` regions, one per
//  `new`, whose funclets are all the same empty `nullsub_45` tail.  This
//  repo's fixed `/GS- /EHs-c-` emits neither the frame nor the nine state
//  stores - about thirty instructions.  See framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "handleobject.h"       // GetHandleOrZero - 0x6F4C4630

static const char kBackdropFile[] = ".\\CBackdropGenerator.cpp";

//  `ValueName` (0x6F87529C) is four zero bytes - the shared empty string
//  every skin lookup in this binary falls back to.  Spelled as a local
//  literal rather than given a funcmap.DATA name, because naming a *shared*
//  string constant can only break matches elsewhere
//  (docs/msvc-vc8-idioms.md).
static const char kNoTexture[] = "";

//  One corner of a quad's texture coordinates.  Eight floats go to
//  SetTexCoordArray, but they are written as four (u, v) pairs: the shipped
//  code builds each pair in two scratch slots and copies it out as two
//  dwords, which is a whole-object assignment of an eight-byte POD and not
//  eight independent float stores.
struct SBackdropUV
{
    SBackdropUV() { m_u = 0.0f; m_v = 0.0f; }
    SBackdropUV(float u, float v) { m_u = u; m_v = v; }

    float m_u;
    float m_v;
};

void CBackdropGenerator::Generate(CSimpleFrame* frame)
{
    SBackdropUV uv[4];

    //  One pointer for all nine pieces, not one per block: the shipped code
    //  keeps it in a single callee-saved register from the top of the
    //  function to the bottom, which is also what lets the background's
    //  own "allocation failed" arm fall through with no `xor` of its own -
    //  the register is already zero there.  Per-block locals cost that and
    //  swap two registers for the whole function.
    CSimpleTexture* piece = 0;

    const char* background =
        (const char*)GetHandleOrZero((const SHandleHolder*)&m_background);

    if (background != 0 && *background != 0)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 50, 0))
            CSimpleTexture(frame, 0, 1);

        m_pBackground = piece;

        piece->SetPointTo(0, frame, 0,  m_insetLeft,  -m_insetTop,    1);
        piece->SetPointTo(2, frame, 2, -m_insetRight, -m_insetTop,    1);
        piece->SetPointTo(6, frame, 6,  m_insetLeft,   m_insetBottom, 1);
        //  m_insetTop, not m_insetBottom - see the header comment.
        piece->SetPointTo(8, frame, 8, -m_insetRight,  m_insetTop,    1);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_background);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, m_tiled);
    }

    //  --- the four edges ------------------------------------------------
    if (m_pieces & 1)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 61, 0))
            CSimpleTexture(frame, 1, 1);

        m_pEdgeLeft = piece;

        piece->SetWidth(m_edgeSize);
        piece->SetPointTo(0, frame, 0, 0.0f, -m_edgeSize, 1);
        piece->SetPointTo(6, frame, 6, 0.0f,  m_edgeSize, 1);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 1);
    }

    if (m_pieces & 2)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 71, 0))
            CSimpleTexture(frame, 1, 1);

        m_pEdgeRight = piece;

        piece->SetWidth(m_edgeSize);
        piece->SetPointTo(2, frame, 2, 0.0f, -m_edgeSize, 1);
        piece->SetPointTo(8, frame, 8, 0.0f,  m_edgeSize, 1);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 1);
    }

    if (m_pieces & 4)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 81, 0))
            CSimpleTexture(frame, 1, 1);

        m_pEdgeTop = piece;

        piece->SetHeight(m_edgeSize);
        piece->SetPointTo(0, frame, 0,  m_edgeSize, 0.0f, 1);
        piece->SetPointTo(2, frame, 2, -m_edgeSize, 0.0f, 1);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 1);
    }

    if (m_pieces & 8)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 91, 0))
            CSimpleTexture(frame, 1, 1);

        m_pEdgeBottom = piece;

        piece->SetHeight(m_edgeSize);
        piece->SetPointTo(6, frame, 6,  m_edgeSize, 0.0f, 1);
        piece->SetPointTo(8, frame, 8, -m_edgeSize, 0.0f, 1);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 1);
    }

    //  --- the four corners ----------------------------------------------
    if (m_pieces & 0x10)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 101, 0))
            CSimpleTexture(frame, 1, 1);

        piece->SetWidth(m_edgeSize);
        piece->SetHeight(m_edgeSize);
        piece->SetPointTo(0, frame, 0, 0.0f, 0.0f, 1);

        uv[0] = SBackdropUV(0.5f,   0.0f);
        uv[1] = SBackdropUV(0.5f,   1.0f);
        uv[2] = SBackdropUV(0.625f, 0.0f);
        uv[3] = SBackdropUV(0.625f, 1.0f);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 0);
        piece->SetTexCoordArray((const float*)uv);
    }

    if (m_pieces & 0x20)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 120, 0))
            CSimpleTexture(frame, 1, 1);

        piece->SetWidth(m_edgeSize);
        piece->SetHeight(m_edgeSize);
        piece->SetPointTo(2, frame, 2, 0.0f, 0.0f, 1);

        uv[0] = SBackdropUV(0.625f, 0.0f);
        uv[1] = SBackdropUV(0.625f, 1.0f);
        uv[2] = SBackdropUV(0.75f,  0.0f);
        uv[3] = SBackdropUV(0.75f,  1.0f);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 0);
        piece->SetTexCoordArray((const float*)uv);
    }

    if (m_pieces & 0x40)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 139, 0))
            CSimpleTexture(frame, 1, 1);

        piece->SetWidth(m_edgeSize);
        piece->SetHeight(m_edgeSize);
        piece->SetPointTo(6, frame, 6, 0.0f, 0.0f, 1);

        uv[0] = SBackdropUV(0.75f,  0.0f);
        uv[1] = SBackdropUV(0.75f,  1.0f);
        uv[2] = SBackdropUV(0.875f, 0.0f);
        uv[3] = SBackdropUV(0.875f, 1.0f);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 0);
        piece->SetTexCoordArray((const float*)uv);
    }

    if (m_pieces & 0x80)
    {
        piece = new (SMemAlloc(0xE8, kBackdropFile, 158, 0))
            CSimpleTexture(frame, 1, 1);

        piece->SetWidth(m_edgeSize);
        piece->SetHeight(m_edgeSize);
        piece->SetPointTo(8, frame, 8, 0.0f, 0.0f, 1);

        uv[0] = SBackdropUV(0.875f, 0.0f);
        uv[1] = SBackdropUV(0.875f, 1.0f);
        uv[2] = SBackdropUV(1.0f,   0.0f);
        uv[3] = SBackdropUV(1.0f,   1.0f);

        const char* path =
            (const char*)GetHandleOrZero((const SHandleHolder*)&m_border);
        if (path == 0)
            path = kNoTexture;

        piece->SetTexturePath(path, 0);
        piece->SetTexCoordArray((const float*)uv);
    }
}
