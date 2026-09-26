#include "Game.h"

CWidget::CWidget(int kind)
{
    m_kind = (u8)kind;
    m_state = 0;
    m_owner = (u8)kPlayerNeutralPassive;
    m_life = 1.0f;
    m_maxLife = 1.0f;
    m_pos = Vec2(0.0f, 0.0f);
}

CWidget::~CWidget()
{
}

void CWidget::Update(float dt)
{
    (void)dt;
}

float CWidget::ApplyDamage(CUnit* source, float amount, int attackType, int flags)
{
    (void)attackType;
    (void)flags;
    if (!IsAlive() || amount <= 0.0f)
        return 0.0f;
    if (amount > m_life)
        amount = m_life;
    m_life -= amount;
    if (m_life <= 0.0f)
        Kill(source);
    return amount;
}

void CWidget::Kill(CUnit* killer)
{
    (void)killer;
    m_life = 0.0f;
    m_state |= US_DEAD | US_REMOVE;
}

float CWidget::ArmorValue() const
{
    return 0.0f;
}

int CWidget::DefenseType() const
{
    return DEF_NORMAL;
}

float CWidget::CollisionRadius() const
{
    return 16.0f;
}

// ---------------------------------------------------------------------------

CDestructable::CDestructable(int cell) : CWidget(WK_DESTRUCTABLE)
{
    m_cell = (u16)cell;
    m_lumber = 50;
    m_harvesters = 0;
    m_life = 50.0f;
    m_maxLife = 50.0f;
    m_pos = Vec2(CTerrain::CellCenter(cell % kMapCells), CTerrain::CellCenter(cell / kMapCells));
}

CDestructable::~CDestructable()
{
}

// Only siege weapons and spells knock trees down.
float CDestructable::ApplyDamage(CUnit* source, float amount, int attackType, int flags)
{
    if (attackType != ATK_SIEGE && !(flags & AF_SPELL))
        return 0.0f;
    return CWidget::ApplyDamage(source, amount * 0.5f, attackType, flags);
}

void CDestructable::Kill(CUnit* killer)
{
    (void)killer;
    if (!IsAlive())
        return;
    m_life = 0.0f;
    m_lumber = 0;
    m_state |= US_DEAD;
    g_game->m_terrain.ClearFlags(m_cell % kMapCells, m_cell / kMapCells,
                                 CELL_TREE | CELL_NOWALK | CELL_NOBUILD);
}

int CDestructable::DefenseType() const
{
    return DEF_FORTIFIED;
}

int CDestructable::Harvest(int amount)
{
    if (!IsAlive())
        return 0;
    int taken = amount < (int)m_lumber ? amount : (int)m_lumber;
    m_lumber = (u16)(m_lumber - taken);
    if (m_lumber == 0)
        Kill(0);
    return taken;
}

// ---------------------------------------------------------------------------

CItem::CItem(int itemType) : CWidget(WK_ITEM)
{
    m_itemType = (u8)itemType;
    m_charges = GetItemType(itemType)->charges;
    m_carrier = 0;
    m_life = 75.0f;
    m_maxLife = 75.0f;
}

CItem::~CItem()
{
}

void CItem::Kill(CUnit* killer)
{
    (void)killer;
    m_life = 0.0f;
    m_state |= US_DEAD | US_REMOVE;
    m_carrier = 0;
}

int CItem::DefenseType() const
{
    return DEF_LIGHT;
}
