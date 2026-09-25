//============================================================================
//  CSelectionManager - the object that owns the on-ground selection circles.
//
//  The class name is the shipped one and not a guess: its constructor
//  (0x6F382D40) stamps `??_7CSelectionManager@@6B@` over its own vtable
//  pointer after calling CObserver's constructor, so the base is CObserver
//  and the name is the one the shipped build compiled.
//
//  Only the fields this slice reaches are named, and each one is a load or a
//  store in a reconstructed body.
//============================================================================
#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include "observer.h"

class CSelectionManager : public CObserver
{
public:
    //  0x6F382D40 - GameUI/selectionmanagerctor.cpp.  `retn 4`.
    CSelectionManager(void* owner);

    //  +0x0C - the only argument the constructor keeps.
    void* m_pOwner;                     // +0x0C
    //  +0x10 - 0x1F4, the one non-zero integer the constructor writes before
    //  the geometry block.
    int   m_field10;                    // +0x10
    //  +0x14 - a '+w3u' checked slot (Agent/agenttypedslots.h), built from
    //  null.  Declared as its storage because the constructor runs it in the
    //  middle of the body, where a member constructor would run first.
    char  m_unit[4];                    // +0x14
    int   m_field18;                    // +0x18
    int   m_field1C;                    // +0x1C
    //  +0x20 - which of the two circle-geometry builders to use.  The
    //  constructor leaves it zero, i.e. the fixed-eight-segment one; nothing
    //  in this tree writes it, so what turns it on is not established here.
    int  m_scaledGeometry;              // +0x20
    //  +0x24 .. +0x50 - twelve more words the constructor clears, and the
    //  frame's own unwind trampoline destroys sub-objects at +0x24, +0x34
    //  and +0x44, so at least three of them are not plain fields.  The
    //  constructor writes all twelve as zero stores either way.
    int   m_reserved24[12];             // +0x24 .. +0x54
    float m_float54;                    // +0x54
    float m_float58;                    // +0x58
    char  m_reserved5C[0x04];           // +0x5C
    int   m_reserved60[6];              // +0x60 .. +0x78
    //  +0x78 - a TEXTUREIMAGE (Render/renderdevice.h), 0x18 bytes, built by
    //  its own constructor at 0x6F526F70.
    char  m_image[0x18];                // +0x78
    float m_float90;                    // +0x90
    float m_float94;                    // +0x94
    float m_float98;                    // +0x98
    int   m_field9C;                    // +0x9C
    int   m_fieldA0;                    // +0xA0
    //  +0xA4 .. +0xE0 - sixteen floats zeroed as one `fldz` run.
    float m_geometry[16];               // +0xA4 .. +0xE4
    //  +0xE4 - the segment count the scaled builder multiplies by four.
    int   m_segmentCount;               // +0xE4
    float m_floatE8;                    // +0xE8
    int   m_fieldEC;                    // +0xEC
    int   m_fieldF0;                    // +0xF0
    int   m_fieldF4;                    // +0xF4
    char  m_reservedF8[0x04];           // +0xF8
    //  +0xFC - [SelectionCircle] ColorFriend, read once at construction.
    unsigned int m_colorFriend;         // +0xFC

    //  0x6F381D50 - rebuild the circle geometry with whichever of the two
    //  builders m_scaledGeometry selects.  `retn 0`.
    //  GameUI/selectionmanagergeometry.cpp.
    void RebuildCircleGeometry();

    //  0x6F380B90 - segments from the count at +0xE4, times four.  `retn 0`.
    void BuildScaledCircleGeometry();
    //  0x6F380330 - a fixed eight segments.  `retn 0`.
    void BuildDefaultCircleGeometry();
};

#endif
