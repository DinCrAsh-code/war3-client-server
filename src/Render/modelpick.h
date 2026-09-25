//============================================================================
//  Picking: is there a model surface between these two points, and how far
//  along the segment is it?
//
//  This is the half of `.\Texture.cpp`'s neighbourhood that has nothing to
//  do with textures - it is reached from the sprite the cursor is over - and
//  it is where the whole-program register conventions in this batch are
//  concentrated.  Four of the seven functions below take an argument in a
//  register no MSVC calling convention reaches (eax, ebx, esi or edi), and
//  each one says so and says what it costs.
//
//  Nothing here carries an RTTI descriptor, so nothing here is given a class
//  name it does not have: the receivers are CModelInstance (Widget/sprite.h)
//  and CSprite, and the rest are `float*` in fmatrix.h's own spelling.
//============================================================================
#ifndef MODELPICK_H
#define MODELPICK_H

#include "material.h"

//: flt_6FA87F24 / flt_6F955508 / flt_6FAA173C - +infinity, FLT_MAX and
//: +infinity again, the three "nothing was hit" distances this module uses.
extern const float g_pickInfinity;      // flt_6FA87F24
extern const float g_pickFarthest;      // flt_6F955508
extern const float g_pickInfinity2;     // flt_6FAA173C
//: flt_6F92E7A4 - 0.001f.  Already a DATA row under the name batch B6's
//: camera pan gave it (`g_cameraTolerance`); one shipped address keeps one
//: C++ name, so this module reuses it rather than adding a second.
extern const float g_cameraTolerance;
//: flt_6FADB40C / flt_6FADB410 - the two world-origin offsets GetPickBounds
//: adds.  Both words are 0xFFFFFFFF in the image, so they are written at run
//: time and are globals rather than constants.
extern const float g_pickOriginX;
extern const float g_pickOriginY;
//: dword_6FACBC68 - the base of the 0x30-byte record table 0x6F509420
//: indexes.
extern char* g_pickRecordTable;

//  0x6F514840 - does this object's +0x44 sub-object select between more than
//  one variant?  Two flag bits and a count, and both bits ask the same
//  question of the same count.
int __fastcall HasVariantSelection_6F514840(const void* self);

//  0x6F509260 - four floats: the selected variant's, when there is one, and
//  the object's own default block otherwise.  **The shipped build takes all
//  three arguments in registers** - the record in edi, the owner in ebx and
//  the destination in esi - and cleans nothing; edi and ebx are out of
//  reach, so the destination is a stack parameter here and this body cleans
//  four where the shipped one cleans none.
void __fastcall QueryVariantVec4_6F509260(const void* record,
                                          const void* owner, float* out);

//  0x6F509420 - the 0x30-byte record at this index, or null for -1.
void* __fastcall PickRecordAt_6F509420(unsigned int index);

//  0x6F5085F0 - a bounding sphere through a 4x3: the centre transformed, the
//  radius scaled.  The matrix arrives in eax in the shipped build.
void __fastcall TransformBoundingSphere(float* sphere, const float* matrix,
                                        float scale);

//  0x6F509910 - the closest approach between the segment `a`->`b` and the
//  point/segment `origin`, written out as two floats.  **Three pointer
//  inputs in eax, edx and ecx and two stack outputs**, which no MSVC
//  spelling reaches: two of the three become ecx and edx here and the third
//  becomes a stack parameter, so this body cleans twelve bytes where the
//  shipped one cleans eight.
void __fastcall ClosestApproach_6F509910(const float* a, const float* b,
                                         const float* origin, float* out0,
                                         float* out1);

//  0x6F50B790 - the recursive walk: every geoset of this model instance and
//  then every child, keeping the nearest hit.
int __fastcall ModelPickSegment(CModelInstance* model, const float* from,
                                const float* to, float* outDistance,
                                int includeChildren, int includeHidden);

//  0x6F50BFC0 - the entry the sprite uses: build the model's own bounding
//  sphere, reject the segment against it, and only then walk the model.
int __fastcall SpriteModelPick(CModelInstance* model, const float* from,
                               float scale, const float* to, float radius,
                               float* outDistance, int includeChildren);

//  0x6F4D2D70 - and the sprite-side entry: wake the animation if it has not
//  been evaluated, ask the sprite's own vtable for the segment parameter,
//  and hand the model the result.
int __fastcall SpriteIntersectSegment(CSprite* sprite, const float* from,
                                      const float* to, float radius,
                                      float* outDistance);

//  0x6F73B180 - the centre and radius of one pickable object's bounds.
void __stdcall GetPickBounds(const void* object, float* outCentre,
                             float* outRadius);

//  0x6F743E60 - one animated collision shape against the ray.
int __stdcall ShapeListPick(void* shape, const float* origin,
                            const float* direction, float* outDistance);

//  0x6F75CE60 - every shape hanging off one node list.
int __stdcall NodeListPick(void* node, const float* origin,
                           const float* direction, float* outDistance);

#endif
