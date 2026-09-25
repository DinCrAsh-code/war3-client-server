//============================================================================
//  0x6F2B9940 - CreateItemAtPosition: build an item of type `itemId` on the
//  ground at (*x, *y) and hand back the new CAgent*, or null.
//
//  __fastcall: the type in ecx, the X pointer in edx, the Y pointer on the
//  stack (`retn 4`) - JASS_UnitAddItemById's own header comment already
//  established this from the call site.
//
//  Follows the same template every other agile-type factory in this repo
//  does (MakeOrderAgent, unitordermake.cpp; the `Location` native's own
//  0x6F3D03B0) - a scoped, type-checked slot around
//  SubmitWidgetAgentQuery's own made-object holder - plus the one step
//  those do not have: building the ground orientation
//  (BuildItemGroundOrientation, already established at 0x6F2B6270) from the
//  position and the facing this same populate step computed, and handing
//  it to the new item's own sprite through SetItemPlacementOrientation -
//  the same two calls JASS_SetItemPosition's own placement code already
//  makes (jasssetitemposition.cpp).
//
//  PopulateItemCreateParams (0x6F2B90C0) is `static` in this TU on purpose
//  - see its own header for why that is what lets it take `esi`/`edi`
//  rather than real parameters.
//
//  Own translation unit: everything above is a real out-of-line call, most
//  of it shared with other factories that must not be inlined into this
//  one either.
//============================================================================
#include "unitcreateparams.h"
#include "itemtypedassign.h"
#include "itemtypetable.h"
#include "itemplacementdispatch.h"   // SOrientationMatrix
#include "widgetagentquery.h"
#include "game.h"
#include "CFloat.h"
#include "jassnatives.h"        // STStringField
#include "fvec3.h"

double cos(double value);
double sin(double value);
#pragma intrinsic(cos)
#pragma intrinsic(sin)

void __fastcall GetItemGroundOffset(FVec3* out, float worldX, float worldY);

//  0x6F4C4660 - GetHandleOrZeroAlias, already reconstructed
//  (Agent/handleobject.cpp); redeclared locally with its real signature.
unsigned int __fastcall GetHandleOrZeroAlias(const SHandleHolder* self);
extern const CFloat g_CFloatDegToRad;   // dword_6FAAE5E8
extern const CFloat g_unk6FAAE4A0;       // dword_6FAAE4A0
CFloat __fastcall GetBuildingAngle();

//  item_typefields.cpp's own ten - redeclared locally the way every other
//  cross-TU caller in this repo does.
int __fastcall ItemTypeExists(unsigned int fourCC);
int __fastcall ItemTypeGetField58(unsigned int fourCC);
int __fastcall ItemTypeGetField5C(unsigned int fourCC);
int __fastcall ItemTypeGetField60(unsigned int fourCC);
int __fastcall ItemTypeGetField64(unsigned int fourCC);
int __fastcall ItemTypeGetField6C(unsigned int fourCC);
SHandleHolder* __fastcall ItemTypeGetHandleField84(unsigned int fourCC);
SHandleHolder* __fastcall ItemTypeGetHandleField98(unsigned int fourCC);
float __fastcall ItemTypeGetField90(unsigned int fourCC);
CFloat* __fastcall ItemTypeGetFieldA4(CFloat* out, unsigned int fourCC);
unsigned int* __fastcall QueryItemTypeTint(unsigned int* out, unsigned int typeId);

//  itemtypequeries.cpp's own two.
int __fastcall ItemTypeIsPowerup(unsigned int fourCC);
int __fastcall ItemTypeIsPawnable(unsigned int fourCC);

//  0x6F2B90C0 - see item_populate section below.  `static`, one caller,
//  this TU - the calling convention MSVC invents for it depends on both.
static int PopulateItemCreateParams(SUnitCreateParams* params, unsigned int itemId);

//  0x6F2B5AE0 - GetItemTypeFourCC, already reconstructed elsewhere.
unsigned int __fastcall GetItemTypeFourCC();

//  What SubmitWidgetAgentQuery's made-object holder carries - the same
//  +0x54 shape every other factory in this repo names it.
struct SMadeItemAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  0x6F26B7F0 - SField28Owner::GetField28, already reconstructed
//  (Misc/sweep_predicates.cpp); redeclared locally.
struct SField28Owner { int __thiscall GetField28(); };

//  0x6F4D3170 - SetItemPlacementOrientation, already reconstructed
//  (Item/itemplacementdispatch.cpp); redeclared locally.
void __fastcall SetItemPlacementOrientation(void* target, SOrientationMatrix m);

//  0x6F2B6270 - BuildItemGroundOrientation.  The real reconstruction (and
//  the reasoning behind every line of it) lives in Jass/jasssetitemposition.cpp,
//  but it is `static` there - internal linkage is exactly what buys it the
//  MSVC-invented esi-return calling convention its own header explains
//  (docs/msvc-vc8-idioms.md, "A `static` function in its one caller's TU
//  gets a made-up calling convention"), and that trick only fires when the
//  compiler can see every call site *in that one TU*.  0x6F2B6270 is a real,
//  second call site here too, so this file needs its own `static` instance
//  to get the same shipped ABI rather than an ordinary external declaration
//  reaching across a TU boundary it cannot cross (a `static` symbol has no
//  external name to link against) - see item_populate's own header for why
//  PopulateItemCreateParams needs the identical `static`-in-this-TU shape.
//  Kept byte-for-byte identical to the other copy; if one changes, so must
//  this one.
static SOrientationMatrix __stdcall
BuildItemGroundOrientation(float worldX, float worldY, float facingAngle)
{
    SOrientationMatrix matrix;
    matrix.m[0] = 1.0f;  matrix.m[1] = 0.0f;  matrix.m[2] = 0.0f;
    matrix.m[3] = 0.0f;  matrix.m[4] = 1.0f;  matrix.m[5] = 0.0f;
    matrix.m[6] = 0.0f;  matrix.m[7] = 0.0f;  matrix.m[8] = 1.0f;

    FVec3 up;
    up.m_x = 0.0f;
    up.m_y = 0.0f;
    up.m_z = 0.0f;
    GetItemGroundOffset(&up, worldX, worldY);

    float cosA = (float)cos(facingAngle);
    float sinA = (float)sin(facingAngle);

    FVec3 right;
    right.m_x = up.m_y * 0.0f - sinA * up.m_z;
    right.m_y = cosA * up.m_z - up.m_x * 0.0f;
    right.m_z = sinA * up.m_x - cosA * up.m_y;

    FVec3 forward;
    forward.m_x = right.m_y * up.m_z - right.m_z * up.m_y;
    forward.m_y = right.m_z * up.m_x - right.m_x * up.m_z;
    forward.m_z = right.m_x * up.m_y - right.m_y * up.m_x;

    forward.Normalize();
    right.Normalize();

    matrix.m[0] = forward.m_x;
    matrix.m[1] = forward.m_y;
    matrix.m[2] = forward.m_z;
    matrix.m[3] = right.m_x;
    matrix.m[4] = right.m_y;
    matrix.m[5] = right.m_z;
    matrix.m[6] = up.m_x;
    matrix.m[7] = up.m_y;
    matrix.m[8] = up.m_z;
    return matrix;
}

void* __fastcall CreateItemAtPosition(int itemId, const CFloat* x, const CFloat* y)
{
    SUnitCreateParams params;

    if (!PopulateItemCreateParams(&params, (unsigned int)itemId))
        return 0;

    SCheckedItemSlot held(0);

    unsigned int typeId = GetItemTypeFourCC();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, type->m_allocator);
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeItemAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    if (held.m_value)
    {
        //  Slot 107 (0x1ACh/4) - populate the new item from `params`.
        typedef void (__thiscall *ApplyParamsFn)(void*, int, SUnitCreateParams*);
        ApplyParamsFn apply = *(ApplyParamsFn*)(*(void***)held.m_value + 107);
        apply(held.m_value, itemId, &params);

        CFloat facing(*(int*)&params.m_facingRadiansBits);
        SOrientationMatrix orientation = BuildItemGroundOrientation(
            *(const float*)x, *(const float*)y, *(const float*)&facing);
        SetItemPlacementOrientation((void*)((SField28Owner*)held.m_value)->GetField28(),
                                     orientation);
    }

    return held.m_value;
}

//  0x6F2B90C0 - fill in the per-type fields of `params` for `itemId`, or
//  refuse outright if the type does not exist.  See the file header above
//  for why this stays `static` in CreateItemAtPosition's own TU.
static int PopulateItemCreateParams(SUnitCreateParams* params, unsigned int itemId)
{
    if (!ItemTypeExists(itemId))
        return 0;

    unsigned int handle84 = GetHandleOrZeroAlias(ItemTypeGetHandleField84(itemId));
    ((STStringField*)&params->m_sub2C)->Assign((const char*)handle84);

    params->m_int24 |= 4;
    params->m_int28 = 0xF;

    CFloat facing = GetBuildingAngle() * g_CFloatDegToRad;
    params->m_facingRadiansBits = *(int*)&facing;

    params->m_float10 = ItemTypeGetField90(itemId);

    unsigned int tint;
    params->m_int14 = (int)*QueryItemTypeTint(&tint, itemId);

    int zero = 0;
    params->m_int18 = zero;
    params->m_int20 = zero;

    CFloat fieldA4(kCFloatNoInit);
    int fieldA4Bits = *(int*)ItemTypeGetFieldA4(&fieldA4, itemId);
    params->m_int3C = fieldA4Bits;
    params->m_int38 = fieldA4Bits;

    params->m_int40 = g_CFloatZero.m_bits;
    params->m_int44 = g_unk6FAAE4A0.m_bits;
    params->m_int48 = zero;
    params->m_int4C = zero;
    params->m_int58 = 0xF;

    params->m_int5C = ItemTypeGetField6C(itemId);
    params->m_int60 = ItemTypeGetField58(itemId);
    params->m_int64 = ItemTypeGetField5C(itemId);
    params->m_int68 = ItemTypeGetField60(itemId);
    params->m_int6C = ItemTypeGetField64(itemId);
    params->m_int70 = ItemTypeIsPowerup(itemId);
    params->m_int74 = ItemTypeIsPawnable(itemId);
    params->m_int78 = GetHandleOrZeroAlias(ItemTypeGetHandleField98(itemId));

    return zero + 1;
}
