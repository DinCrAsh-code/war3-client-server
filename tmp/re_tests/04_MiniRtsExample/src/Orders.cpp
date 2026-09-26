#include "Game.h"

// Right-click semantics: the concrete order depends on what was clicked
// and who is being ordered.
static COrder ResolveSmartOrder(const CUnit* unit, const COrder& order)
{
    COrder o = order;
    CWidget* w = (o.flags & (OF_TARGET_UNIT | OF_TARGET_ITEM)) ? g_game->GetWidget(o.target) : 0;
    if (!w)
    {
        o.id = ORD_MOVE;
        return o;
    }

    o.point = w->m_pos;
    if (w->m_kind == WK_UNIT)
    {
        const CUnit* t = static_cast<const CUnit*>(w);
        if (unit->IsEnemyOf(t) && unit->CanAttack(t))
            o.id = ORD_ATTACK;
        else if ((t->m_data->flags & UTF_GOLDMINE) && unit->IsWorker())
            o.id = ORD_HARVEST;
        else if (unit->IsWorker() && t->m_owner == unit->m_owner &&
                 (t->m_data->flags & (UTF_TOWNHALL | UTF_LUMBERDROP)))
            o.id = ORD_RETURN;
        else
            o.id = ORD_MOVE;
    }
    else if (w->m_kind == WK_DESTRUCTABLE)
    {
        o.id = unit->IsWorker() ? ORD_HARVEST : (unit->CanAttack(w) ? ORD_ATTACK : ORD_MOVE);
    }
    else
    {
        o.id = unit->IsHero() ? ORD_PICKUP : ORD_MOVE;
    }
    return o;
}

static bool OrderNeedsMovement(int id)
{
    switch (id)
    {
    case ORD_MOVE:
    case ORD_ATTACK_MOVE:
    case ORD_PATROL:
    case ORD_HARVEST:
    case ORD_RETURN:
    case ORD_BUILD:
    case ORD_PICKUP:
        return true;
    default:
        return false;
    }
}

bool CUnit::IssueOrder(const COrder& order, bool queued)
{
    if (!IsAlive() || (m_state & US_CONSTRUCTING))
        return false;
    if (HandleImmediateOrder(order))
        return true;

    COrder o = order;
    if (o.id == ORD_SMART)
        o = ResolveSmartOrder(this, order);
    if (o.id == ORD_NONE || o.id >= ORD_COUNT)
        return false;
    if (m_stats.moveSpeed <= 0.0f && OrderNeedsMovement(o.id))
        return false;
    if (o.id == ORD_BUILD && !FindAbility(AB_BUILD))
        return false;
    if ((o.id == ORD_HARVEST || o.id == ORD_RETURN) && !FindAbility(AB_HARVEST))
        return false;

    if (queued && m_order.id != ORD_NONE)
    {
        if (m_queueCount >= kOrderQueueSize)
            return false;
        m_queue[m_queueCount++] = o;
        return true;
    }

    // Leaving a harvest cycle drops the worker's claim on its mine.
    if ((m_order.id == ORD_HARVEST || m_order.id == ORD_RETURN) && o.id != ORD_HARVEST && o.id != ORD_RETURN)
    {
        CAbility* h = FindAbility(AB_HARVEST);
        if (h)
            static_cast<CAbilHarvest*>(h)->ReleaseClaims();
    }
    else if (o.id == ORD_HARVEST)
    {
        CAbility* h = FindAbility(AB_HARVEST);
        if (h && static_cast<CAbilHarvest*>(h)->m_state != CAbilHarvest::HS_IN_MINE)
            static_cast<CAbilHarvest*>(h)->ReleaseClaims();
    }

    InterruptChannel();
    ClearOrders();
    StopMoving();
    m_order = o;
    m_orderOrigin = m_pos;
    m_attackTarget = (o.id == ORD_ATTACK) ? o.target : 0;
    if (m_attackPhase == 1)
        m_attackPhase = 0;
    return true;
}

void CUnit::ClearOrders()
{
    m_queueCount = 0;
    m_order.id = ORD_NONE;
    m_order.flags = 0;
}

void CUnit::NextOrder()
{
    StopMoving();
    m_attackTarget = 0;
    if (m_queueCount > 0)
    {
        m_order = m_queue[0];
        for (int i = 1; i < m_queueCount; ++i)
            m_queue[i - 1] = m_queue[i];
        --m_queueCount;
    }
    else
    {
        m_order.id = ORD_NONE;
    }
    m_orderOrigin = m_pos;
}

void CUnit::ProcessOrder(float dt)
{
    int result;
    switch (m_order.id)
    {
    case ORD_NONE:
        result = OrderIdle(dt);
        break;
    case ORD_MOVE:
        result = OrderMove(dt);
        break;
    case ORD_ATTACK:
        result = OrderAttack(dt);
        break;
    case ORD_ATTACK_MOVE:
        result = OrderAttackMove(dt);
        break;
    case ORD_PATROL:
        result = OrderPatrol(dt);
        break;
    case ORD_HOLD:
        result = OrderHold(dt);
        break;
    case ORD_HARVEST:
    case ORD_RETURN:
        {
            CAbility* a = FindAbility(AB_HARVEST);
            result = a ? a->OrderUpdate(m_order, dt) : OR_FAILED;
        }
        break;
    case ORD_BUILD:
        {
            CAbility* a = FindAbility(AB_BUILD);
            result = a ? a->OrderUpdate(m_order, dt) : OR_FAILED;
        }
        break;
    case ORD_CAST:
        result = OrderCast(dt);
        break;
    case ORD_PICKUP:
        result = OrderPickup(dt);
        break;
    default:
        result = OR_FAILED;
        break;
    }

    if (result != OR_CONTINUE)
        NextOrder();
}

// Idle: acquire and fight anything that comes near, without wandering far
// from where the unit was left.
int CUnit::OrderIdle(float dt)
{
    if (m_data->weaponType == WPN_NONE || IsWorker())
        return OR_CONTINUE;

    CUnit* target = g_game->GetUnit(m_attackTarget);
    if (target && !target->IsAlive())
    {
        target = 0;
        m_attackTarget = 0;
    }

    m_acquireTimer -= dt;
    if (!target && m_acquireTimer <= 0.0f)
    {
        m_acquireTimer = 0.5f;
        float range = IsStructure() ? m_stats.range : MaxF(m_stats.range + 100.0f, 500.0f);
        if (m_campIndex >= 0)
            range = MinF(range, 400.0f);
        target = AcquireTarget(range);
        if (target)
            m_attackTarget = target->m_handle;
    }

    if (target)
    {
        bool chase = !IsStructure() && Distance(m_pos, m_orderOrigin) < 700.0f;
        if (UpdateAttack(target, dt, chase) != OR_CONTINUE)
        {
            m_attackTarget = 0;
            StopMoving();
        }
    }
    else if (!IsStructure() && DistSq(m_pos, m_orderOrigin) > 200.0f * 200.0f)
    {
        if (!m_moving)
            MoveTo(m_orderOrigin, 32.0f);
        if (UpdateMovement(dt) != 0)
            m_orderOrigin = m_pos;
    }
    return OR_CONTINUE;
}

int CUnit::OrderMove(float dt)
{
    if (!m_moving)
    {
        if (DistSq(m_pos, m_order.point) <= 24.0f * 24.0f)
            return OR_DONE;
        if (!MoveTo(m_order.point, 16.0f))
            return OR_FAILED;
    }
    int r = UpdateMovement(dt);
    if (r > 0)
        return OR_DONE;
    if (r < 0)
        return OR_FAILED;
    return OR_CONTINUE;
}

int CUnit::OrderAttack(float dt)
{
    CWidget* target = g_game->GetWidget(m_order.target);
    if (!target)
        return OR_DONE;
    m_attackTarget = m_order.target;
    return UpdateAttack(target, dt, true);
}

int CUnit::OrderAttackMove(float dt)
{
    CUnit* target = g_game->GetUnit(m_attackTarget);
    if (target && target->IsAlive())
    {
        if (UpdateAttack(target, dt, true) == OR_CONTINUE)
            return OR_CONTINUE;
        m_attackTarget = 0;
        StopMoving();
    }

    m_acquireTimer -= dt;
    if (m_acquireTimer <= 0.0f)
    {
        m_acquireTimer = 0.4f;
        target = AcquireTarget(MaxF(m_stats.range + 150.0f, 600.0f));
        if (target)
        {
            m_attackTarget = target->m_handle;
            StopMoving();
            return OR_CONTINUE;
        }
    }

    if (!m_moving)
    {
        if (DistSq(m_pos, m_order.point) <= 64.0f * 64.0f)
            return OR_DONE;
        if (!MoveTo(m_order.point, 48.0f))
            return OR_FAILED;
    }
    int r = UpdateMovement(dt);
    if (r > 0)
        return OR_DONE;
    if (r < 0)
        return OR_FAILED;
    return OR_CONTINUE;
}

int CUnit::OrderPatrol(float dt)
{
    int r = OrderAttackMove(dt);
    if (r == OR_DONE)
    {
        Vec2 tmp = m_order.point;
        m_order.point = m_orderOrigin;
        m_orderOrigin = tmp;
        return OR_CONTINUE;
    }
    return r;
}

int CUnit::OrderHold(float dt)
{
    CUnit* target = g_game->GetUnit(m_attackTarget);
    if (!target || !target->IsAlive() || !InAttackRange(target, 0.0f))
    {
        target = 0;
        m_acquireTimer -= dt;
        if (m_acquireTimer <= 0.0f)
        {
            m_acquireTimer = 0.3f;
            target = AcquireTarget(m_stats.range + CollisionRadius());
        }
        m_attackTarget = target ? target->m_handle : 0;
    }
    if (target && UpdateAttack(target, dt, false) != OR_CONTINUE)
        m_attackTarget = 0;
    return OR_CONTINUE;
}

int CUnit::OrderCast(float dt)
{
    CAbility* a = FindAbility(m_order.param);
    if (!a)
        return OR_FAILED;

    if (m_state & US_CHANNELING)
    {
        if (a->UpdateChannel(dt))
        {
            a->EndChannel();
            m_state &= ~US_CHANNELING;
            return OR_DONE;
        }
        return OR_CONTINUE;
    }

    CastTarget ct;
    ct.unit = m_order.target;
    ct.point = m_order.point;
    if (m_order.flags & OF_TARGET_UNIT)
        ct.kind = CT_UNIT;
    else if (m_order.flags & OF_TARGET_POINT)
        ct.kind = CT_POINT;
    else
        ct.kind = CT_NONE;

    if (ct.kind == CT_UNIT)
    {
        CUnit* t = g_game->GetUnit(ct.unit);
        if (!t || !t->IsAlive())
            return OR_FAILED;
        ct.point = t->m_pos;
    }

    int err = a->CheckCast(ct);
    if (err == CE_OUT_OF_RANGE)
    {
        m_repathTimer -= dt;
        if (!m_moving || m_repathTimer <= 0.0f)
        {
            if (!MoveTo(ct.point, MaxF(a->Data()->range - 32.0f, 32.0f)))
                return OR_FAILED;
        }
        if (UpdateMovement(dt) < 0)
            return OR_FAILED;
        return OR_CONTINUE;
    }
    if (err != CE_OK)
        return OR_FAILED;

    StopMoving();
    if (ct.kind != CT_NONE && !FaceTowards(ct.point, dt))
        return OR_CONTINUE;

    if (a->Cast(ct))
    {
        a->m_channeling = true;
        m_state |= US_CHANNELING;
        return OR_CONTINUE;
    }
    return OR_DONE;
}

int CUnit::OrderPickup(float dt)
{
    CItem* item = g_game->GetItem(m_order.target);
    if (!item || !item->IsAlive() || item->m_carrier || !IsHero())
        return OR_FAILED;

    if (DistSq(m_pos, item->m_pos) > 80.0f * 80.0f)
    {
        if (!m_moving && !MoveTo(item->m_pos, 48.0f))
            return OR_FAILED;
        if (UpdateMovement(dt) < 0)
            return OR_FAILED;
        return OR_CONTINUE;
    }
    StopMoving();
    return static_cast<CHero*>(this)->PickupItem(item) ? OR_DONE : OR_FAILED;
}
