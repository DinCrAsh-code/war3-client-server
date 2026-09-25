//============================================================================
//  0x6F4D8270 - construct a CSprite in place.
//
//  CDataMgr's own default constructor (0x6F4D5xxx-neighbourhood: it also
//  underlies CSpriteUber_'s much larger constructor, sub_6F4D85D0, out of
//  this batch's scope) is inlined first - vtable stamp, m_count=0, the
//  TSFixedArray<CBaseManaged*> property table zeroed
//  (cdatamgrpropertyarray.cpp), the chunk word set to 4, the list
//  terminator poisoned - and then CSprite's own fields (sprite.h) are
//  zeroed and its vtable re-stamped over CDataMgr's.
//
//  The array-clear guard (`if (m_count) { if (m_data) Free(m_data); ...}`)
//  is provably dead by the time it runs here - m_count is the literal 0
//  this same constructor stored three field-writes earlier - and the
//  shipped compiler left it in rather than folding it away.  Reproduced
//  rather than skipped, because the goal is the instruction stream a
//  smarter optimiser would not have emitted, not a smaller one.
//
//  Free function, not `CSprite::CSprite()`: getting the `??_7CSprite@@6B@`
//  symbol needs a real, file-scope, polymorphic class literally named
//  CSprite (the InitProp<Kind> technique cameraconstructor.cpp and
//  Misc/ccamera.cpp's own CBaseManaged stand-in both use) - and the real
//  CSprite (sprite.h) is deliberately non-polymorphic there, with its own
//  vtable pointer as a plain named field, so the two cannot be the same
//  declaration in one translation unit.  This file never includes
//  sprite.h; every CSprite field below is written by raw offset instead,
//  matched against that header's own layout comment for comment.
//============================================================================
void __stdcall SMemFree(void*, const char*, int, unsigned int);

//  Local stand-ins purely to reproduce the two vtable *symbols* the shipped
//  constructor stamps - the real CDataMgr and CSprite classes are
//  unreconstructed/differently-shaped here and live in Game.dll; only the
//  mangled names have to agree.  Deliberately not in an anonymous
//  namespace: that would decorate the mangled symbol.
class CDataMgr
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~CDataMgr() {}
};
class CSprite
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~CSprite() {}
};

void* __fastcall ConstructCSprite(void* rawSelf)
{
    unsigned int* self = (unsigned int*)rawSelf;

    //  CDataMgr::CDataMgr(), inlined.
    CDataMgr dataMgrStamp;
    *(void**)&self[0] = *(void**)&dataMgrStamp;    // +0x00 vtable
    self[1] = 0;                                    // +0x04 m_count
    self[2] = 0;                                    // +0x08 m_propertyTable.m_alloc
    self[3] = 0;                                    // +0x0C m_propertyTable.m_count
    self[4] = 0;                                    // +0x10 m_propertyTable.m_data
    self[5] = 4;                                     // +0x14
    self[6] = (unsigned int)&self[6];                // +0x18 terminator.m_next = &terminator
    self[7] = ~(unsigned int)&self[6];                // +0x1C terminator.m_prevlink

    volatile unsigned int* vself = (volatile unsigned int*)self;
    if (vself[3] != 0)
    {
        if (self[4] != 0)
            SMemFree((void*)self[4], ".PAVCBaseManaged@@", -2, 0);
        self[2] = 0;
        self[3] = 0;
        self[4] = 0;
    }

    //  CSprite's own fields (sprite.h).
    self[8]  = 0;    // +0x20 m_pModel
    self[9]  = 0;    // +0x24
    self[10] = 0;    // +0x28 m_stateBits
    self[12] = 0;    // +0x30 m_pModelData

    CSprite spriteStamp;
    *(void**)&self[0] = *(void**)&spriteStamp;      // +0x00 re-stamp CSprite's own vtable

    ((unsigned short*)self)[0x2C / 2] = 0xFFFE;      // +0x2C
    ((unsigned short*)self)[0x2E / 2] = 0xFFFF;      // +0x2E
    ((unsigned char*)self)[0x34] = 0;                 // m_flag34
    ((unsigned char*)self)[0x35] = 0;                 // m_flag35
    ((unsigned char*)self)[0x36] = 0;                 // m_flag36
    self[0x38 / 4] = 0;
    self[0x3C / 4] = 0;
    self[0x40 / 4] = 0;
    self[0x60 / 4] = 0;    // m_pRef60

    return rawSelf;
}
