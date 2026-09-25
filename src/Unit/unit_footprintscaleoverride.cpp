//============================================================================
//  0x6F27A620 - CUnit's own vtable slot 113 (+0x1C4), CItem's Method_0x108
//  sibling (CWidget's own slot 66) one class up: both start from
//  GetFootprintScaleFactor(m_footprintType) * "SelectionCircle"/"ScaleFactor",
//  but a unit also folds in every currently-attached ability/aura's own
//  scale modifier before the multiply.
//
//  `a == -1` means "use the unit's own default ability, if any" - resolved
//  the same way Unit/unit_haschanneled.cpp's HasChanneledAbility and
//  Unit/unit_findability.cpp's FindAbility already are (both reach through
//  `this` reinterpreted as the shared SUnitAbilityHost, exactly like
//  Unit/unit_maxlevel.cpp does): if the unit is channeling something, look
//  up the 'Bply' ability and take its own +0x114 footprint-type override;
//  otherwise (or if `a` is anything else) `a` itself - or, for -1 with no
//  channeled ability, the unit's own m_footprintType - is the id asked for.
//
//  Then the same ability list (Unit/unit_findability.cpp's {handle,typeTag}
//  chain rooted at +0x1DC, each candidate's own +0x24/+0x28 pair for
//  "next") is walked a second time, independent of the lookup above: every
//  candidate's own vtable+0x2B8 (CAbility's own slot 174 - user_knowledge's
//  ??_7CAbility@@6B@ dump confirms sub_6F052900, "hand back g_CFloatZero",
//  as the base answer) is asked for a CFloat modifier, and any whose
//  magnitude clears the module's own tolerance (g_CFloatEqualityTolerance,
//  flt_6FAAE614 - the exact `(x - g_CFloatZero).m_bits & 0x7FFFFFFF` /
//  `*(float*)&magnitude < tolerance` idiom Unit/floatpropmodifierinit.cpp
//  and Math/cfloatnearlyequal.cpp both already use) gets added into the
//  scale factor as a plain hardware float - the CFloat's own bit pattern,
//  reinterpreted, exactly like unit_attachmentplacement.cpp's own
//  "hardware-float copy" of a CFloat.
//============================================================================
#include "unit.h"
#include "footprinttype.h"      // GetFootprintScaleFactor (0x6F32D070)
#include "itemhandleresolve.h" // QueryHandleField0x54 (0x6F4786B0)

//  0x6F009E30 - Config/configgates.cpp.
float __fastcall GetConfigFloatHW(const char* section, const char* key, int index);

//  0x6F113CC0 - Misc/misc_trivial_getters.cpp's 'Bply'.
unsigned int GetFourCC_6F113CC0();

//  flt_6FAAE614 - Math/cfloatnearlyequal.cpp's own tolerance, reached here
//  the same bare-`fld` way (never through a CFloat operation).
extern const float g_CFloatEqualityTolerance;

//  Both members are real, out-of-line functions in other translation units
//  (Unit/unit_haschanneled.cpp, Unit/unit_findability.cpp) - this empty
//  local redeclaration only has to agree on the mangled signature, the same
//  trick Unit/unit_maxlevel.cpp already uses for FindAbility alone.
struct SUnitAbilityHost
{
    int HasChanneledAbility();
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  A second, independent local view of the same ability-list node
//  Unit/unit_findability.cpp's own SUnitAbility carries - only the fields
//  this function actually reads get names.
struct SUnitAbility
{
    char               m_reserved000[0x24];
    SOptionalHandleRef m_next;              // +0x24/+0x28 - next in the list
    char               m_reserved02C[0x114 - 0x2C];
    unsigned int       m_footprintOverride; // +0x114
};

typedef CFloat* (__thiscall *AbilityScaleModifierFn)(void* self, CFloat* out);

float CUnit::Method_0x1C4(int a)
{
    unsigned int footprintId;

    if (a == -1)
    {
        footprintId = m_footprintType;

        if (((SUnitAbilityHost*)this)->HasChanneledAbility())
        {
            void* ability = ((SUnitAbilityHost*)this)
                ->FindAbility(GetFourCC_6F113CC0(), 1, 0, 1, 1);
            if (ability)
                footprintId = ((SUnitAbility*)ability)->m_footprintOverride;
        }
    }
    else
    {
        footprintId = (unsigned int)a;
    }

    float scale = GetFootprintScaleFactor(footprintId);
    float configScale = GetConfigFloatHW("SelectionCircle", "ScaleFactor", 0);

    SUnitAbility* candidate =
        (m_ref1DC.m_t & m_ref1DC.m_b) != 0xFFFFFFFF
            ? (SUnitAbility*)QueryHandleField0x54((SOptionalHandleRef*)&m_ref1DC)
            : 0;

    while (candidate)
    {
        SUnitAbility* next =
            (candidate->m_next.m_handle & (unsigned int)candidate->m_next.m_typeTag) != 0xFFFFFFFF
                ? (SUnitAbility*)QueryHandleField0x54(&candidate->m_next)
                : 0;

        CFloat modifier;
        ((AbilityScaleModifierFn)(*(void***)candidate)[0x2B8 / 4])(candidate, &modifier);

        int magnitude = (modifier - g_CFloatZero).m_bits & 0x7FFFFFFF;
        if (*(const float*)&magnitude < g_CFloatEqualityTolerance)
        {
        }
        else
        {
            scale += *(const float*)&modifier.m_bits;
        }

        candidate = next;
    }

    return scale * configScale;
}
