#include "Game.h"

static const float kMaxMissileTravel = 4000.0f;
static const float kBounceSpeed      = 1600.0f;
static const float kBounceFalloff    = 0.85f;

CMissile::CMissile(CUnit* source, const Vec2& from, float speed)
{
    m_source = source ? source->m_handle : 0;
    m_owner = source ? source->m_owner : (u8)kPlayerNeutralPassive;
    m_target = 0;
    m_pos = from;
    m_targetPoint = from;
    m_speed = speed > 1.0f ? speed : 900.0f;
    m_traveled = 0.0f;
    m_done = false;
}

CMissile::~CMissile()
{
}

bool CMissile::IsHoming() const
{
    return true;
}

void CMissile::SetTarget(CWidget* target)
{
    m_target = target ? target->m_handle : 0;
    if (target)
        m_targetPoint = target->m_pos;
}

bool CMissile::Update(float dt)
{
    if (m_done)
        return true;

    CWidget* target = m_target ? g_game->GetWidget(m_target) : 0;
    if (target && !target->IsAlive())
        target = 0;
    if (target && IsHoming())
        m_targetPoint = target->m_pos;

    Vec2 delta = m_targetPoint - m_pos;
    float dist = Length(delta);
    float step = m_speed * dt;
    if (dist <= step + 1.0f)
    {
        m_pos = m_targetPoint;
        m_done = true;
        OnImpact(target, m_pos);
        return true;
    }

    m_pos += delta * (step / dist);
    m_traveled += step;
    if (m_traveled > kMaxMissileTravel)
        m_done = true;
    return m_done;
}

// ---------------------------------------------------------------------------

CAttackMissile::CAttackMissile(CUnit* source, const Vec2& from, const AttackContext& ctx)
    : CMissile(source, from, source ? (float)source->m_data->missileSpeed : 900.0f)
{
    m_ctx = ctx;
    m_ctx.source = 0;
    SetTarget(ctx.target);
    if (ctx.weaponType == WPN_ARTILLERY)
        m_targetPoint = ctx.impactPoint;
}

bool CAttackMissile::IsHoming() const
{
    return m_ctx.weaponType != WPN_ARTILLERY;
}

void CAttackMissile::OnImpact(CWidget* target, const Vec2& at)
{
    // Artillery lands where the target was at launch; whoever moved away
    // only takes splash.
    if (target && m_ctx.weaponType == WPN_ARTILLERY)
    {
        float r = target->CollisionRadius() + 24.0f;
        if (DistSq(target->m_pos, at) > r * r)
            target = 0;
    }
    if (!target && m_ctx.splashRadius <= 0.0f)
        return;

    m_ctx.source = g_game->GetUnit(m_source);
    m_ctx.target = target;
    m_ctx.impactPoint = at;
    ResolveAttack(m_ctx);
}

// ---------------------------------------------------------------------------

CSpellMissile::CSpellMissile(CUnit* source, const Vec2& from, float speed, float damage,
                             int buffType, int level, float duration)
    : CMissile(source, from, speed)
{
    m_damage = damage;
    m_buffType = buffType;
    m_level = level;
    m_duration = duration;
}

void CSpellMissile::OnImpact(CWidget* target, const Vec2& at)
{
    (void)at;
    if (!target || target->m_kind != WK_UNIT)
        return;

    CUnit* source = g_game->GetUnit(m_source);
    CUnit* unit = static_cast<CUnit*>(target);
    if (!unit->IsTargetable())
        return;

    DealDamage(source, unit, m_damage, ATK_SPELL, AF_SPELL);
    if (unit->IsAlive() && m_buffType != AB_NONE)
        unit->AddBuff(m_buffType, m_level, m_duration, source);
}

// ---------------------------------------------------------------------------

CBounceMissile::CBounceMissile(CUnit* source, const Vec2& from, float damage, int bounces, float range)
    : CMissile(source, from, kBounceSpeed)
{
    m_damage = damage;
    m_bouncesLeft = bounces;
    m_bounceRange = range;
    m_numHit = 0;
}

struct BounceSearch
{
    FilterContext         base;
    const CBounceMissile* missile;
};

static bool BounceFilter(CUnit* unit, void* context)
{
    const BounceSearch* s = static_cast<const BounceSearch*>(context);
    if (!FilterUnit(unit, const_cast<FilterContext*>(&s->base)) || !unit->IsTargetable())
        return false;
    for (int i = 0; i < s->missile->m_numHit; ++i)
    {
        if (s->missile->m_hit[i] == unit->m_handle)
            return false;
    }
    return true;
}

void CBounceMissile::OnImpact(CWidget* target, const Vec2& at)
{
    if (!target || target->m_kind != WK_UNIT)
        return;

    CUnit* source = g_game->GetUnit(m_source);
    CUnit* unit = static_cast<CUnit*>(target);
    DealDamage(source, unit, m_damage, ATK_SPELL, AF_SPELL);
    if (m_numHit < (int)COUNTOF(m_hit))
        m_hit[m_numHit++] = unit->m_handle;

    if (m_bouncesLeft <= 1)
        return;

    BounceSearch search;
    search.base.self = 0;
    search.base.player = m_owner;
    search.base.flags = FF_ENEMY | FF_VISIBLE | FF_NO_STRUCT;
    search.base.minLifeRatio = 0.0f;
    search.missile = this;

    CUnit* next = g_game->m_grid.FindNearest(at, m_bounceRange, BounceFilter, &search);
    if (!next)
        return;

    CBounceMissile* bounce = new CBounceMissile(source, at, m_damage * kBounceFalloff, m_bouncesLeft - 1, m_bounceRange);
    bounce->m_source = m_source;
    bounce->m_owner = m_owner;
    for (int i = 0; i < m_numHit; ++i)
        bounce->m_hit[i] = m_hit[i];
    bounce->m_numHit = m_numHit;
    bounce->SetTarget(next);
    g_game->AddMissile(bounce);
}
