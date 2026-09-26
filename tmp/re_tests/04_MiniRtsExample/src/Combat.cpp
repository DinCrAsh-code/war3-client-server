#include "Game.h"

enum AttackPhase
{
    AP_IDLE = 0,
    AP_WINDUP,
    AP_BACKSWING
};

static const float kHeroXpRange = 1200.0f;

float ArmorDamageFactor(float armor)
{
    if (armor >= 0.0f)
    {
        float r = armor * 0.06f;
        return 1.0f - r / (1.0f + r);
    }
    return 2.0f - powf(0.94f, -armor);
}

float DamageTypeFactor(int attackType, int defenseType)
{
    if (attackType < 0 || attackType >= ATK_COUNT || defenseType < 0 || defenseType >= DEF_COUNT)
        return 1.0f;
    return g_damageTable[attackType][defenseType];
}

bool FilterUnit(CUnit* unit, void* context)
{
    const FilterContext* f = static_cast<const FilterContext*>(context);
    if (!unit->IsAlive() || (unit->m_state & (US_HIDDEN | US_INSIDE)))
        return false;
    if ((f->flags & FF_NOT_SELF) && unit == f->self)
        return false;

    const CPlayer& p = g_game->m_players[f->player];
    if ((f->flags & FF_ENEMY) && !p.IsEnemy(unit->m_owner))
        return false;
    if ((f->flags & FF_ALLY) && !p.IsAlly(unit->m_owner))
        return false;
    if ((f->flags & FF_NO_STRUCT) && unit->IsStructure())
        return false;
    if ((f->flags & FF_STRUCTURE) && !unit->IsStructure())
        return false;
    if ((f->flags & FF_HERO) && !unit->IsHero())
        return false;

    bool wantGround = (f->flags & FF_GROUND) != 0;
    bool wantAir = (f->flags & FF_AIR) != 0;
    if (wantGround && !wantAir && unit->IsFlying())
        return false;
    if (wantAir && !wantGround && !unit->IsFlying())
        return false;

    if ((f->flags & FF_INJURED) && unit->m_life >= unit->m_maxLife * f->minLifeRatio)
        return false;
    if ((f->flags & FF_HAS_MANA) && unit->m_stats.maxMana <= 0.0f)
        return false;
    if ((f->flags & FF_VISIBLE) && !g_game->IsVisibleTo(f->player, unit))
        return false;
    return true;
}

bool FilterAttackable(CUnit* unit, void* context)
{
    const FilterContext* f = static_cast<const FilterContext*>(context);
    if (!FilterUnit(unit, context))
        return false;
    if (!unit->IsTargetable())
        return false;
    return f->self->CanAttack(unit);
}

// ---------------------------------------------------------------------------

bool CUnit::CanAttack(const CWidget* target) const
{
    if (m_data->weaponType == WPN_NONE || !target || !target->IsAlive() || target == this)
        return false;
    if (m_state & US_CONSTRUCTING)
        return false;

    if (target->m_kind == WK_DESTRUCTABLE)
        return m_data->attackType == ATK_SIEGE;
    if (target->m_kind != WK_UNIT)
        return false;

    const CUnit* u = static_cast<const CUnit*>(target);
    if (!u->IsTargetable() || (u->m_state & US_INVISIBLE))
        return false;
    if (!IsEnemyOf(u))
        return false;
    if (u->IsFlying())
        return (m_data->flags & UTF_ATTACKS_AIR) != 0;
    return (m_data->flags & UTF_ATTACKS_GND) != 0;
}

bool CUnit::InAttackRange(const CWidget* target, float slack) const
{
    float reach = m_stats.range + CollisionRadius() + target->CollisionRadius() + slack;
    return DistSq(m_pos, target->m_pos) <= reach * reach;
}

float CUnit::RollDamage()
{
    float damage = m_stats.damageBase + m_stats.damageFlat;
    for (int i = 0; i < m_stats.dice; ++i)
        damage += (float)g_game->m_rng.Range(1, m_stats.sides);
    return damage * m_stats.damageMul;
}

void CUnit::LaunchAttack(CWidget* target)
{
    AttackContext ctx;
    ctx.source = this;
    ctx.target = target;
    ctx.damage = RollDamage();
    ctx.attackType = m_data->attackType;
    ctx.weaponType = m_data->weaponType;
    ctx.flags = (m_data->flags & UTF_RANGED) ? AF_RANGED : 0;
    ctx.sourceOwner = m_owner;
    ctx.impactPoint = target->m_pos;
    ctx.splashRadius = (float)m_data->splashRadius;

    for (int i = 0; i < m_numAbilities; ++i)
        m_abilities[i]->OnAttackLaunch(ctx);

    if (ctx.weaponType == WPN_INSTANT)
        ResolveAttack(ctx);
    else
        g_game->AddMissile(new CAttackMissile(this, m_pos, ctx));
}

int CUnit::UpdateAttack(CWidget* target, float dt, bool allowChase)
{
    if (!target || !target->IsAlive())
    {
        if (m_attackPhase == AP_WINDUP)
            m_attackPhase = AP_IDLE;
        return OR_DONE;
    }
    if (!CanAttack(target))
    {
        m_attackPhase = AP_IDLE;
        return OR_FAILED;
    }

    switch (m_attackPhase)
    {
    case AP_IDLE:
        if (!InAttackRange(target, 0.0f))
        {
            if (!allowChase || m_stats.moveSpeed <= 0.0f)
                return OR_FAILED;

            m_repathTimer -= dt;
            if (!m_moving || (m_repathTimer <= 0.0f && DistSq(m_moveGoal, target->m_pos) > 64.0f * 64.0f))
            {
                float stop = m_stats.range + CollisionRadius() + target->CollisionRadius() - 8.0f;
                if (!MoveTo(target->m_pos, MaxF(stop, 8.0f)))
                    return OR_FAILED;
            }
            if (UpdateMovement(dt) < 0 && !InAttackRange(target, 16.0f))
                return OR_FAILED;
            return OR_CONTINUE;
        }

        StopMoving();
        if (!FaceTowards(target->m_pos, dt))
            return OR_CONTINUE;
        if (m_cooldown > 0.0f)
            return OR_CONTINUE;

        m_attackPhase = AP_WINDUP;
        m_attackTimer = (float)m_data->damagePointMs * 0.001f / m_stats.attackSpeed;
        return OR_CONTINUE;

    case AP_WINDUP:
        m_attackTimer -= dt;
        if (m_attackTimer > 0.0f)
            return OR_CONTINUE;
        if (!InAttackRange(target, 64.0f))
        {
            m_attackPhase = AP_IDLE;
            return OR_CONTINUE;
        }
        LaunchAttack(target);
        m_cooldown = m_stats.cooldown / m_stats.attackSpeed;
        m_attackPhase = AP_BACKSWING;
        m_attackTimer = (float)m_data->backswingMs * 0.001f / m_stats.attackSpeed;
        return OR_CONTINUE;

    default:
        m_attackTimer -= dt;
        if (m_attackTimer <= 0.0f)
            m_attackPhase = AP_IDLE;
        return OR_CONTINUE;
    }
}

// Picks the most threatening visible enemy in range: units that can fight
// back first, structures last, current attackers and heroes favoured.
CUnit* CUnit::AcquireTarget(float range)
{
    if (m_data->weaponType == WPN_NONE)
        return 0;

    FilterContext ctx;
    ctx.self = this;
    ctx.player = m_owner;
    ctx.flags = FF_ENEMY | FF_VISIBLE | FF_NOT_SELF;
    ctx.minLifeRatio = 0.0f;

    CUnit* candidates[32];
    int n = g_game->m_grid.Collect(m_pos, range, FilterAttackable, &ctx, candidates, 32);

    CUnit* best = 0;
    float bestScore = -1e9f;
    for (int i = 0; i < n; ++i)
    {
        CUnit* c = candidates[i];
        float score = 1000.0f - Distance(m_pos, c->m_pos);
        if (c->m_data->weaponType != WPN_NONE)
            score += 300.0f;
        if (c->IsStructure())
            score -= 500.0f;
        if (c->m_attackTarget == m_handle)
            score += 200.0f;
        if (c->IsHero())
            score += 100.0f;
        if (c->IsWorker())
            score -= 150.0f;
        score += (1.0f - c->m_life / c->m_maxLife) * 150.0f;
        if (score > bestScore)
        {
            bestScore = score;
            best = c;
        }
    }
    return best;
}

// ---------------------------------------------------------------------------

float DealDamage(CUnit* source, CWidget* target, float amount, int attackType, int flags)
{
    if (!target || !target->IsAlive() || amount <= 0.0f)
        return 0.0f;

    float factor = DamageTypeFactor(attackType, target->DefenseType());
    if (!(flags & AF_SPELL))
        factor *= ArmorDamageFactor(target->ArmorValue());
    return target->ApplyDamage(source, amount * factor, attackType, flags);
}

void ResolveAttack(AttackContext& ctx)
{
    CWidget* target = ctx.target;
    if (target && target->m_kind == WK_UNIT)
    {
        CUnit* tu = static_cast<CUnit*>(target);
        for (int i = 0; i < tu->m_numAbilities; ++i)
        {
            if (tu->m_abilities[i]->OnIncomingAttack(ctx))
            {
                ctx.flags |= AF_MISS;
                break;
            }
        }
    }

    if (target && !(ctx.flags & AF_MISS))
    {
        float dealt = DealDamage(ctx.source, target, ctx.damage, ctx.attackType, ctx.flags);
        if (dealt > 0.0f && ctx.source && ctx.source->IsAlive())
        {
            for (int i = 0; i < ctx.source->m_numAbilities; ++i)
                ctx.source->m_abilities[i]->OnAttackHit(ctx);
        }
    }

    if ((ctx.weaponType == WPN_ARTILLERY || ctx.weaponType == WPN_MISSILE_SPLASH) && ctx.splashRadius > 0.0f)
    {
        DealSplashDamage(ctx.source, ctx.sourceOwner, ctx.impactPoint, ctx.splashRadius, ctx.damage,
                         ctx.attackType, ctx.flags | AF_SPLASH, target);
    }
}

// Three damage bands: full inside 40% of the radius, half inside 75%,
// quarter to the edge. Also knocks down trees inside the full band.
int DealSplashDamage(CUnit* source, int owner, const Vec2& center, float radius, float amount,
                     int attackType, int flags, CWidget* primary)
{
    FilterContext ctx;
    ctx.self = source;
    ctx.player = owner;
    ctx.flags = FF_ENEMY;
    ctx.minLifeRatio = 0.0f;

    CUnit* hits[32];
    int n = g_game->m_grid.Collect(center, radius, FilterUnit, &ctx, hits, 32);
    int damaged = 0;
    for (int i = 0; i < n; ++i)
    {
        CUnit* u = hits[i];
        if (u == primary || !u->IsTargetable())
            continue;
        float d = Distance(center, u->m_pos) - u->CollisionRadius();
        float factor = d < radius * 0.4f ? 1.0f : (d < radius * 0.75f ? 0.5f : 0.25f);
        if (DealDamage(source, u, amount * factor, attackType, flags | AF_NOREACT) > 0.0f)
            ++damaged;
    }

    if (attackType == ATK_SIEGE)
    {
        int r = (int)(radius * 0.4f / kCellSize) + 1;
        int cx = CTerrain::WorldToCell(center.x);
        int cy = CTerrain::WorldToCell(center.y);
        for (int y = cy - r; y <= cy + r; ++y)
        {
            for (int x = cx - r; x <= cx + r; ++x)
            {
                if (!g_game->m_terrain.InBounds(x, y))
                    continue;
                CDestructable* tree = g_game->TreeAtCell(y * kMapCells + x);
                if (tree && tree->IsAlive())
                    tree->ApplyDamage(source, amount, attackType, flags);
            }
        }
    }
    return damaged;
}

int ExperienceForKill(const CUnit* victim, int heroLevel)
{
    int level = ClampI(victim->Level(), 0, 10);
    if (victim->IsHero())
        return g_heroKillXpByLevel[level];

    int xp = g_creepXpByLevel[level];
    if (victim->m_owner == kPlayerNeutralHostile)
        xp = xp * g_creepXpFactorByHeroLevel[ClampI(heroLevel, 0, kMaxHeroLevel)] / 100;
    if (victim->m_data->flags & UTF_SUMMONED)
        xp /= 2;
    return xp;
}

static void DistributeExperience(CUnit* killer, CUnit* victim)
{
    if (victim->IsStructure())
        return;

    FilterContext ctx;
    ctx.self = killer;
    ctx.player = killer->m_owner;
    ctx.flags = FF_ALLY | FF_HERO;
    ctx.minLifeRatio = 0.0f;

    CUnit* heroes[8];
    int n = g_game->m_grid.Collect(victim->m_pos, kHeroXpRange, FilterUnit, &ctx, heroes, 8);
    if (n == 0)
        return;

    for (int i = 0; i < n; ++i)
    {
        CHero* hero = static_cast<CHero*>(heroes[i]);
        int xp = ExperienceForKill(victim, hero->m_level) / n;
        if (xp > 0)
            hero->AddExperience(xp);
    }
}

void AwardKill(CUnit* killer, CUnit* victim)
{
    if (!killer)
        return;
    CPlayer& kp = g_game->m_players[killer->m_owner];
    if (!kp.IsEnemy(victim->m_owner))
        return;

    ++kp.m_unitsKilled;
    if (victim->m_owner == kPlayerNeutralHostile && victim->m_data->bounty > 0)
    {
        int gold = victim->m_data->bounty + g_game->m_rng.Range(0, victim->Level() * 2);
        kp.m_gold += gold;
    }
    DistributeExperience(killer, victim);
}
