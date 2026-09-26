#include "Game.h"

CAbility::CAbility(CUnit* owner, int type, int level)
{
    m_owner = owner;
    m_data = GetAbilityType(type);
    m_type = (u8)type;
    m_level = (u8)level;
    m_autoCast = (m_data->flags & ABF_AUTOCAST) != 0;
    m_channeling = false;
    m_cooldown = 0.0f;
    m_timer = 0.0f;
}

CAbility::~CAbility()
{
}

void CAbility::OnAdded()
{
}

void CAbility::OnRemoved()
{
}

void CAbility::Update(float dt)
{
    if (m_cooldown > 0.0f)
        m_cooldown -= dt;
    if (m_timer > 0.0f)
        m_timer -= dt;
}

int CAbility::CheckCast(const CastTarget& target) const
{
    if (m_level == 0)
        return CE_NOT_LEARNED;
    if (IsPassive())
        return CE_PASSIVE;
    if (m_cooldown > 0.0f)
        return CE_COOLDOWN;
    if (m_owner->m_mana < ManaCost())
        return CE_NO_MANA;
    if (m_owner->m_stunCount > 0)
        return CE_DISABLED;

    if (m_data->flags & ABF_TARGET_UNIT)
    {
        if (target.kind != CT_UNIT)
            return CE_BAD_TARGET;
        CUnit* t = g_game->GetUnit(target.unit);
        if (!t || !t->IsTargetable() || t->IsStructure())
            return CE_BAD_TARGET;
        bool friendly = (m_data->flags & ABF_FRIENDLY) != 0;
        if (friendly != m_owner->IsAllyOf(t))
            return CE_BAD_TARGET;
        float reach = m_data->range + m_owner->CollisionRadius() + t->CollisionRadius();
        if (DistSq(m_owner->m_pos, t->m_pos) > reach * reach)
            return CE_OUT_OF_RANGE;
    }
    else if (m_data->flags & ABF_TARGET_POINT)
    {
        if (target.kind != CT_POINT)
            return CE_BAD_TARGET;
        if (Distance(m_owner->m_pos, target.point) > m_data->range)
            return CE_OUT_OF_RANGE;
    }
    return CE_OK;
}

bool CAbility::Cast(const CastTarget& target)
{
    (void)target;
    return false;
}

bool CAbility::UpdateChannel(float dt)
{
    (void)dt;
    return true;
}

void CAbility::EndChannel()
{
    m_channeling = false;
}

void CAbility::OnAttackLaunch(AttackContext& ctx)
{
    (void)ctx;
}

void CAbility::OnAttackHit(AttackContext& ctx)
{
    (void)ctx;
}

bool CAbility::OnIncomingAttack(AttackContext& ctx)
{
    (void)ctx;
    return false;
}

int CAbility::OrderUpdate(COrder& order, float dt)
{
    (void)order;
    (void)dt;
    return OR_FAILED;
}

bool CAbility::TryAutoCast()
{
    return false;
}

void CAbility::StartCooldown()
{
    m_cooldown = m_data->cooldown[LevelIndex()];
}

bool CAbility::PayCost()
{
    float cost = ManaCost();
    if (m_owner->m_mana < cost)
        return false;
    m_owner->m_mana -= cost;
    return true;
}

float CAbility::Duration(const CUnit* target) const
{
    if (target && target->IsHero())
        return m_data->heroDuration[LevelIndex()];
    return m_data->duration[LevelIndex()];
}

CAbility* CreateAbility(CUnit* owner, int type, int level)
{
    if (type < 0 || type >= AB_COUNT)
        return 0;

    switch (GetAbilityType(type)->abilityClass)
    {
    case ABC_HARVEST:      return new CAbilHarvest(owner, type, level);
    case ABC_BUILD:        return new CAbilBuild(owner, type, level);
    case ABC_MISSILE_STUN: return new CAbilMissileStun(owner, type, level);
    case ABC_NOVA:         return new CAbilNova(owner, type, level);
    case ABC_BASH:         return new CAbilBash(owner, type, level);
    case ABC_TARGET_HEAL:  return new CAbilTargetHeal(owner, type, level);
    case ABC_SELF_BUFF:    return new CAbilSelfBuff(owner, type, level);
    case ABC_AURA:         return new CAbilAura(owner, type, level);
    case ABC_CHANNEL_AREA: return new CAbilChannelArea(owner, type, level);
    case ABC_SUMMON:       return new CAbilSummon(owner, type, level);
    case ABC_CRITICAL:     return new CAbilCritical(owner, type, level);
    case ABC_EVASION:      return new CAbilEvasion(owner, type, level);
    case ABC_BOUNCE:       return new CAbilBounce(owner, type, level);
    case ABC_LINE:         return new CAbilLine(owner, type, level);
    case ABC_TARGET_BUFF:  return new CAbilTargetBuff(owner, type, level);
    case ABC_ENVENOM:      return new CAbilEnvenom(owner, type, level);
    default:               return 0;
    }
}

// ---------------------------------------------------------------------------
// Hero spells

CAbilMissileStun::CAbilMissileStun(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

bool CAbilMissileStun::Cast(const CastTarget& target)
{
    CUnit* t = g_game->GetUnit(target.unit);
    if (!t || !PayCost())
        return false;
    StartCooldown();

    int i = LevelIndex();
    CSpellMissile* m = new CSpellMissile(m_owner, m_owner->m_pos, m_data->valueB[i], m_data->valueA[i],
                                         m_data->buffType, m_level, Duration(t));
    m->SetTarget(t);
    g_game->AddMissile(m);
    return false;
}

CAbilNova::CAbilNova(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

int CAbilNova::CheckCast(const CastTarget& target) const
{
    int err = CAbility::CheckCast(target);
    if (err != CE_OK)
        return err;

    FilterContext ctx;
    ctx.self = m_owner;
    ctx.player = m_owner->m_owner;
    ctx.flags = FF_ENEMY | FF_GROUND | FF_NO_STRUCT;
    ctx.minLifeRatio = 0.0f;
    CUnit* any[1];
    if (g_game->m_grid.Collect(m_owner->m_pos, m_data->area[LevelIndex()], FilterUnit, &ctx, any, 1) == 0)
        return CE_BAD_TARGET;
    return CE_OK;
}

bool CAbilNova::Cast(const CastTarget& target)
{
    (void)target;
    if (!PayCost())
        return false;
    StartCooldown();

    int i = LevelIndex();
    FilterContext ctx;
    ctx.self = m_owner;
    ctx.player = m_owner->m_owner;
    ctx.flags = FF_ENEMY | FF_GROUND | FF_NO_STRUCT;
    ctx.minLifeRatio = 0.0f;

    CUnit* hits[24];
    int n = g_game->m_grid.Collect(m_owner->m_pos, m_data->area[i], FilterUnit, &ctx, hits, 24);
    for (int k = 0; k < n; ++k)
    {
        CUnit* u = hits[k];
        if (!u->IsTargetable())
            continue;
        DealDamage(m_owner, u, m_data->valueA[i], ATK_SPELL, AF_SPELL);
        if (u->IsAlive())
            u->AddBuff(m_data->buffType, m_level, Duration(u), m_owner);
    }
    return false;
}

CAbilBash::CAbilBash(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

void CAbilBash::OnAttackLaunch(AttackContext& ctx)
{
    if (m_level == 0)
        return;
    int i = LevelIndex();
    if (g_game->m_rng.Chance(m_data->valueA[i]))
    {
        ctx.flags |= AF_BASH;
        ctx.damage += m_data->valueB[i];
    }
}

void CAbilBash::OnAttackHit(AttackContext& ctx)
{
    if (!(ctx.flags & AF_BASH) || !ctx.target || ctx.target->m_kind != WK_UNIT)
        return;
    CUnit* u = static_cast<CUnit*>(ctx.target);
    if (u->IsAlive() && !u->IsStructure())
        u->AddBuff(m_data->buffType, m_level, Duration(u), m_owner);
}

CAbilTargetHeal::CAbilTargetHeal(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

int CAbilTargetHeal::CheckCast(const CastTarget& target) const
{
    int err = CAbility::CheckCast(target);
    if (err != CE_OK)
        return err;
    CUnit* t = g_game->GetUnit(target.unit);
    if (!t || t->m_life >= t->m_maxLife)
        return CE_BAD_TARGET;
    return CE_OK;
}

bool CAbilTargetHeal::Cast(const CastTarget& target)
{
    CUnit* t = g_game->GetUnit(target.unit);
    if (!t || !PayCost())
        return false;
    StartCooldown();
    t->Heal(m_data->valueA[LevelIndex()]);
    return false;
}

bool CAbilTargetHeal::TryAutoCast()
{
    if (m_timer > 0.0f || m_cooldown > 0.0f || m_owner->m_mana < ManaCost())
        return false;
    m_timer = 1.0f;

    FilterContext ctx;
    ctx.self = m_owner;
    ctx.player = m_owner->m_owner;
    ctx.flags = FF_ALLY | FF_NO_STRUCT | FF_INJURED;
    ctx.minLifeRatio = 0.9f;

    CUnit* hurt[16];
    int n = g_game->m_grid.Collect(m_owner->m_pos, m_data->range, FilterUnit, &ctx, hurt, 16);
    CUnit* best = 0;
    float bestRatio = 1.0f;
    for (int i = 0; i < n; ++i)
    {
        float ratio = hurt[i]->m_life / hurt[i]->m_maxLife;
        if (ratio < bestRatio)
        {
            bestRatio = ratio;
            best = hurt[i];
        }
    }
    if (!best)
        return false;

    CastTarget t;
    t.kind = CT_UNIT;
    t.unit = best->m_handle;
    t.point = best->m_pos;
    if (CheckCast(t) != CE_OK)
        return false;
    Cast(t);
    return true;
}

CAbilSelfBuff::CAbilSelfBuff(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

bool CAbilSelfBuff::Cast(const CastTarget& target)
{
    (void)target;
    if (!PayCost())
        return false;
    StartCooldown();
    m_owner->AddBuff(m_data->buffType, m_level, Duration(m_owner), m_owner);
    return false;
}

// Attacking out of invisibility adds the bonus damage and breaks it.
void CAbilSelfBuff::OnAttackLaunch(AttackContext& ctx)
{
    if (m_data->buffType != BF_WINDWALK)
        return;
    CBuff* b = m_owner->FindBuff(BF_WINDWALK);
    if (!b)
        return;
    ctx.damage += m_data->valueA[LevelIndex()];
    b->m_expired = true;
}

CAbilAura::CAbilAura(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

void CAbilAura::ApplyToUnit(CUnit* unit, void* context)
{
    CAbilAura* aura = static_cast<CAbilAura*>(context);
    if (!unit->IsAlive() || unit->IsStructure() || !aura->m_owner->IsAllyOf(unit))
        return;
    unit->AddBuff(aura->m_data->buffType, aura->m_level, 1.0f, aura->m_owner);
}

void CAbilAura::Update(float dt)
{
    CAbility::Update(dt);
    if (m_level == 0 || m_timer > 0.0f || !m_owner->IsAlive())
        return;
    m_timer = 0.5f;
    g_game->m_grid.Visit(m_owner->m_pos, m_data->area[LevelIndex()], ApplyToUnit, this);
}

void CAbilAura::OnRemoved()
{
    m_level = 0;
}

CAbilChannelArea::CAbilChannelArea(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
    m_wavesLeft = 0;
}

bool CAbilChannelArea::Cast(const CastTarget& target)
{
    if (!PayCost())
        return false;
    StartCooldown();
    m_center = target.point;
    m_wavesLeft = (int)m_data->valueB[LevelIndex()];
    m_timer = 0.0f;
    return true;
}

void CAbilChannelArea::DropWave()
{
    int i = LevelIndex();
    FilterContext ctx;
    ctx.self = m_owner;
    ctx.player = m_owner->m_owner;
    ctx.flags = FF_ENEMY;
    ctx.minLifeRatio = 0.0f;

    for (int shard = 0; shard < 4; ++shard)
    {
        float angle = g_game->m_rng.RangeF(0.0f, kTwoPi);
        float dist = g_game->m_rng.RangeF(0.0f, m_data->area[i]);
        Vec2 p = PolarOffset(m_center, dist, angle);

        CUnit* hits[16];
        int n = g_game->m_grid.Collect(p, 100.0f, FilterUnit, &ctx, hits, 16);
        for (int k = 0; k < n; ++k)
        {
            float amount = m_data->valueA[i];
            if (hits[k]->IsStructure())
                amount *= 0.5f;
            DealDamage(m_owner, hits[k], amount, ATK_SPELL, AF_SPELL);
        }
    }
}

bool CAbilChannelArea::UpdateChannel(float dt)
{
    (void)dt;
    if (m_timer <= 0.0f && m_wavesLeft > 0)
    {
        DropWave();
        --m_wavesLeft;
        m_timer = m_data->duration[LevelIndex()];
    }
    return m_wavesLeft <= 0;
}

void CAbilChannelArea::EndChannel()
{
    CAbility::EndChannel();
    m_wavesLeft = 0;
}

CAbilSummon::CAbilSummon(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

bool CAbilSummon::Cast(const CastTarget& target)
{
    (void)target;
    if (!PayCost())
        return false;
    StartCooldown();

    int i = LevelIndex();
    int count = (int)m_data->valueA[i];
    for (int k = 0; k < count; ++k)
    {
        float angle = m_owner->m_facing + ((float)k - (float)(count - 1) * 0.5f) * 0.8f;
        Vec2 p = PolarOffset(m_owner->m_pos, 96.0f, angle);
        CUnit* u = g_game->CreateUnit(m_data->summonType, m_owner->m_owner, p, m_owner->m_facing);
        if (!u)
            continue;
        u->m_statScale = m_data->valueB[i];
        u->RecalcStats();
        u->m_life = u->m_maxLife;
        u->AddBuff(m_data->buffType, m_level, m_data->duration[i], m_owner);

        CUnit* enemy = g_game->GetUnit(m_owner->m_attackTarget);
        if (enemy && enemy->IsAlive())
        {
            COrder o;
            o.id = ORD_ATTACK;
            o.flags = OF_TARGET_UNIT;
            o.param = 0;
            o.target = enemy->m_handle;
            o.point = enemy->m_pos;
            u->IssueOrder(o, false);
        }
    }
    return false;
}

CAbilCritical::CAbilCritical(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

void CAbilCritical::OnAttackLaunch(AttackContext& ctx)
{
    if (m_level == 0)
        return;
    int i = LevelIndex();
    if (g_game->m_rng.Chance(m_data->valueA[i]))
    {
        ctx.damage *= m_data->valueB[i];
        ctx.flags |= AF_CRIT;
    }
}

CAbilEvasion::CAbilEvasion(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

bool CAbilEvasion::OnIncomingAttack(AttackContext& ctx)
{
    if (m_level == 0 || (ctx.flags & (AF_SPELL | AF_SPLASH)))
        return false;
    return g_game->m_rng.Chance(m_data->valueA[LevelIndex()]);
}

CAbilBounce::CAbilBounce(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

bool CAbilBounce::Cast(const CastTarget& target)
{
    CUnit* t = g_game->GetUnit(target.unit);
    if (!t || !PayCost())
        return false;
    StartCooldown();

    int i = LevelIndex();
    CBounceMissile* m = new CBounceMissile(m_owner, m_owner->m_pos, m_data->valueA[i],
                                           (int)m_data->valueB[i], m_data->area[i]);
    m->SetTarget(t);
    g_game->AddMissile(m);
    return false;
}

CAbilLine::CAbilLine(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

bool CAbilLine::Cast(const CastTarget& target)
{
    if (!PayCost())
        return false;
    StartCooldown();

    int i = LevelIndex();
    Vec2 start = m_owner->m_pos;
    Vec2 dir = Normalize(target.point - start);
    Vec2 end = start + dir * m_data->range;
    Vec2 mid = (start + end) * 0.5f;
    float width = m_data->area[i];

    FilterContext ctx;
    ctx.self = m_owner;
    ctx.player = m_owner->m_owner;
    ctx.flags = FF_ENEMY | FF_GROUND;
    ctx.minLifeRatio = 0.0f;

    CUnit* hits[32];
    int n = g_game->m_grid.Collect(mid, m_data->range * 0.5f + width, FilterUnit, &ctx, hits, 32);
    for (int k = 0; k < n; ++k)
    {
        CUnit* u = hits[k];
        if (!SegmentHitsCircle(start, end, u->m_pos, width + u->CollisionRadius()))
            continue;
        float amount = m_data->valueA[i];
        if (u->IsStructure())
            amount *= 0.5f;
        DealDamage(m_owner, u, amount, ATK_SPELL, AF_SPELL);
    }
    return false;
}

CAbilTargetBuff::CAbilTargetBuff(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

int CAbilTargetBuff::CheckCast(const CastTarget& target) const
{
    int err = CAbility::CheckCast(target);
    if (err != CE_OK)
        return err;
    CUnit* t = g_game->GetUnit(target.unit);
    if (!t || t->FindBuff(m_data->buffType))
        return CE_BAD_TARGET;
    if ((t->m_data->flags & UTF_MECHANICAL) && m_data->buffType == BF_REJUV)
        return CE_BAD_TARGET;
    return CE_OK;
}

bool CAbilTargetBuff::Cast(const CastTarget& target)
{
    CUnit* t = g_game->GetUnit(target.unit);
    if (!t || !PayCost())
        return false;
    StartCooldown();
    t->AddBuff(m_data->buffType, m_level, Duration(t), m_owner);
    return false;
}

bool CAbilTargetBuff::TryAutoCast()
{
    if (m_timer > 0.0f || m_cooldown > 0.0f || m_owner->m_mana < ManaCost())
        return false;
    m_timer = 1.0f;

    bool friendly = (m_data->flags & ABF_FRIENDLY) != 0;
    FilterContext ctx;
    ctx.self = m_owner;
    ctx.player = m_owner->m_owner;
    ctx.flags = (friendly ? FF_ALLY : (FF_ENEMY | FF_VISIBLE)) | FF_NO_STRUCT;
    ctx.minLifeRatio = 0.6f;
    if (m_data->buffType == BF_REJUV)
        ctx.flags |= FF_INJURED;

    CUnit* cands[16];
    int n = g_game->m_grid.Collect(m_owner->m_pos, m_data->range, FilterUnit, &ctx, cands, 16);
    CUnit* best = 0;
    float bestScore = -1.0f;
    for (int i = 0; i < n; ++i)
    {
        CUnit* u = cands[i];
        if (u->FindBuff(m_data->buffType))
            continue;
        float score;
        if (m_data->buffType == BF_BLOODLUST)
            score = u->m_attackTarget ? (float)u->m_data->damageBase + 10.0f : -1.0f;
        else if (m_data->buffType == BF_REJUV)
            score = 1.0f - u->m_life / u->m_maxLife;
        else
            score = (u->IsHero() ? 50.0f : 0.0f) + (float)u->m_data->damageBase;
        if (score > bestScore)
        {
            bestScore = score;
            best = u;
        }
    }
    if (!best || bestScore < 0.0f)
        return false;

    CastTarget t;
    t.kind = CT_UNIT;
    t.unit = best->m_handle;
    t.point = best->m_pos;
    if (CheckCast(t) != CE_OK)
        return false;
    Cast(t);
    return true;
}

CAbilEnvenom::CAbilEnvenom(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
}

void CAbilEnvenom::OnAttackHit(AttackContext& ctx)
{
    if (!ctx.target || ctx.target->m_kind != WK_UNIT)
        return;
    CUnit* u = static_cast<CUnit*>(ctx.target);
    if (u->IsAlive() && !u->IsStructure())
        u->AddBuff(m_data->buffType, m_level, Duration(u), m_owner);
}
