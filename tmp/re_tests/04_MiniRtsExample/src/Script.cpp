#include "Game.h"

// Stack-based trigger script VM. The map script is a compiled bytecode
// blob: a function table followed by code. Natives are dispatched through
// g_scriptNatives; triggers registered by the script are driven by the
// game's timer queue and death events.

enum Opcode
{
    OP_NOP    = 0x00,
    OP_PUSHI  = 0x0C,
    OP_PUSHF  = 0x0D,
    OP_LDG    = 0x11,
    OP_STG    = 0x12,
    OP_LDL    = 0x13,
    OP_STL    = 0x14,
    OP_ADD    = 0x20,
    OP_SUB    = 0x21,
    OP_MUL    = 0x22,
    OP_DIV    = 0x23,
    OP_MOD    = 0x24,
    OP_NEG    = 0x25,
    OP_LT     = 0x30,
    OP_LE     = 0x31,
    OP_GT     = 0x32,
    OP_GE     = 0x33,
    OP_EQ     = 0x34,
    OP_NE     = 0x35,
    OP_AND    = 0x36,
    OP_OR     = 0x37,
    OP_NOT    = 0x38,
    OP_JMP    = 0x40,
    OP_JZ     = 0x41,
    OP_JNZ    = 0x42,
    OP_CALL   = 0x50,
    OP_NATIVE = 0x51,
    OP_RET    = 0x52,
    OP_RETV   = 0x53,
    OP_POP    = 0x60,
    OP_DUP    = 0x61,
    OP_I2F    = 0x62,
    OP_F2I    = 0x63,
    OP_HALT   = 0x7F
};

static const int kMaxCallDepth = 16;
static const int kOpLimit      = 30000;

static ScriptValue MakeInt(s32 v)
{
    ScriptValue r;
    r.type = SV_INT;
    r.i = v;
    return r;
}

static ScriptValue MakeReal(float v)
{
    ScriptValue r;
    r.type = SV_REAL;
    r.f = v;
    return r;
}

static ScriptValue MakeBool(bool v)
{
    ScriptValue r;
    r.type = SV_BOOL;
    r.i = v ? 1 : 0;
    return r;
}

static ScriptValue MakeHandle(Handle h)
{
    ScriptValue r;
    r.type = SV_HANDLE;
    r.h = h;
    return r;
}

static float ToReal(const ScriptValue& v)
{
    return v.type == SV_REAL ? v.f : (float)v.i;
}

static s32 ToInt(const ScriptValue& v)
{
    return v.type == SV_REAL ? (s32)v.f : v.i;
}

static bool IsTrue(const ScriptValue& v)
{
    if (v.type == SV_REAL)
        return v.f != 0.0f;
    return v.i != 0;
}

static s32 ReadS32(const u8* p)
{
    return (s32)((u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24));
}

static float ReadF32(const u8* p)
{
    s32 bits = ReadS32(p);
    float f;
    memcpy(&f, &bits, sizeof(f));
    return f;
}

static s16 ReadS16(const u8* p)
{
    return (s16)((u16)p[0] | ((u16)p[1] << 8));
}

static ScriptValue Arithmetic(int op, const ScriptValue& a, const ScriptValue& b)
{
    if (a.type == SV_REAL || b.type == SV_REAL)
    {
        float x = ToReal(a);
        float y = ToReal(b);
        switch (op)
        {
        case OP_ADD: return MakeReal(x + y);
        case OP_SUB: return MakeReal(x - y);
        case OP_MUL: return MakeReal(x * y);
        case OP_DIV: return MakeReal(y != 0.0f ? x / y : 0.0f);
        default:     return MakeReal(y != 0.0f ? fmodf(x, y) : 0.0f);
        }
    }

    s32 x = a.i;
    s32 y = b.i;
    switch (op)
    {
    case OP_ADD: return MakeInt(x + y);
    case OP_SUB: return MakeInt(x - y);
    case OP_MUL: return MakeInt(x * y);
    case OP_DIV: return MakeInt(y != 0 ? x / y : 0);
    default:     return MakeInt(y != 0 ? x % y : 0);
    }
}

static bool Compare(int op, const ScriptValue& a, const ScriptValue& b)
{
    if (a.type == SV_REAL || b.type == SV_REAL)
    {
        float x = ToReal(a);
        float y = ToReal(b);
        switch (op)
        {
        case OP_LT: return x < y;
        case OP_LE: return x <= y;
        case OP_GT: return x > y;
        case OP_GE: return x >= y;
        case OP_EQ: return x == y;
        default:    return x != y;
        }
    }

    u32 ux = a.h;
    u32 uy = b.h;
    s32 x = a.i;
    s32 y = b.i;
    switch (op)
    {
    case OP_LT: return x < y;
    case OP_LE: return x <= y;
    case OP_GT: return x > y;
    case OP_GE: return x >= y;
    case OP_EQ: return ux == uy;
    default:    return ux != uy;
    }
}

bool CScriptVM::Load(const u8* code, u32 length)
{
    m_code = code;
    m_codeLength = length;
    m_sp = 0;
    m_depth = 0;
    m_numTriggers = 0;
    m_totalOps = 0;
    m_faulted = false;
    m_eventUnit = 0;
    m_eventKiller = 0;

    if (length < 2)
        return false;
    m_numFunctions = code[0];
    m_numGlobals = code[1];
    if (m_numFunctions > (int)COUNTOF(m_functions) || m_numGlobals > (int)COUNTOF(m_globals))
        return false;
    if (2u + (u32)m_numFunctions * 4u > length)
        return false;

    for (int i = 0; i < m_numFunctions; ++i)
    {
        const u8* e = code + 2 + i * 4;
        m_functions[i].offset = (u16)(e[0] | (e[1] << 8));
        m_functions[i].numArgs = e[2];
        m_functions[i].numLocals = e[3];
        if (m_functions[i].offset >= length)
            return false;
    }
    for (int i = 0; i < m_numGlobals; ++i)
        m_globals[i] = MakeInt(0);
    return true;
}

bool CScriptVM::Push(const ScriptValue& v)
{
    if (m_sp >= (int)COUNTOF(m_stack))
    {
        m_faulted = true;
        return false;
    }
    m_stack[m_sp++] = v;
    return true;
}

bool CScriptVM::Pop(ScriptValue& v)
{
    if (m_sp <= 0)
    {
        m_faulted = true;
        v = MakeInt(0);
        return false;
    }
    v = m_stack[--m_sp];
    return true;
}

// Runs one function whose arguments already sit at m_stack[base..].
// Leaves exactly one return value at m_stack[base]. Returns ops executed,
// or -1 on a fault (the thread is abandoned, like an op-limit crash).
int CScriptVM::Execute(int function, int base)
{
    if (function < 0 || function >= m_numFunctions || m_depth >= kMaxCallDepth)
        return -1;

    const Function& fn = m_functions[function];
    for (int i = 0; i < fn.numLocals; ++i)
    {
        if (!Push(MakeInt(0)))
            return -1;
    }

    ++m_depth;
    u32 pc = fn.offset;
    int ops = 0;
    ScriptValue a;
    ScriptValue b;

    for (;;)
    {
        if (pc >= m_codeLength || ++ops + m_totalOps > kOpLimit || m_faulted)
        {
            --m_depth;
            return -1;
        }

        u8 op = m_code[pc++];
        switch (op)
        {
        case OP_NOP:
            break;
        case OP_PUSHI:
            Push(MakeInt(ReadS32(m_code + pc)));
            pc += 4;
            break;
        case OP_PUSHF:
            Push(MakeReal(ReadF32(m_code + pc)));
            pc += 4;
            break;
        case OP_LDG:
            Push(m_globals[m_code[pc++] % COUNTOF(m_globals)]);
            break;
        case OP_STG:
            Pop(m_globals[m_code[pc++] % COUNTOF(m_globals)]);
            break;
        case OP_LDL:
            Push(m_stack[base + m_code[pc++]]);
            break;
        case OP_STL:
            Pop(a);
            m_stack[base + m_code[pc++]] = a;
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
            Pop(b);
            Pop(a);
            Push(Arithmetic(op, a, b));
            break;
        case OP_NEG:
            Pop(a);
            Push(a.type == SV_REAL ? MakeReal(-a.f) : MakeInt(-a.i));
            break;
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
        case OP_EQ:
        case OP_NE:
            Pop(b);
            Pop(a);
            Push(MakeBool(Compare(op, a, b)));
            break;
        case OP_AND:
            Pop(b);
            Pop(a);
            Push(MakeBool(IsTrue(a) && IsTrue(b)));
            break;
        case OP_OR:
            Pop(b);
            Pop(a);
            Push(MakeBool(IsTrue(a) || IsTrue(b)));
            break;
        case OP_NOT:
            Pop(a);
            Push(MakeBool(!IsTrue(a)));
            break;
        case OP_JMP:
            pc = (u32)((s32)pc + 2 + ReadS16(m_code + pc));
            break;
        case OP_JZ:
        case OP_JNZ:
            Pop(a);
            if (IsTrue(a) == (op == OP_JNZ))
                pc = (u32)((s32)pc + 2 + ReadS16(m_code + pc));
            else
                pc += 2;
            break;
        case OP_CALL:
            {
                int callee = m_code[pc++];
                if (callee >= m_numFunctions)
                {
                    m_faulted = true;
                    break;
                }
                int calleeBase = m_sp - m_functions[callee].numArgs;
                if (calleeBase < base)
                {
                    m_faulted = true;
                    break;
                }
                m_totalOps += ops;
                ops = 0;
                if (Execute(callee, calleeBase) < 0)
                {
                    --m_depth;
                    return -1;
                }
            }
            break;
        case OP_NATIVE:
            {
                int index = m_code[pc];
                int argc = m_code[pc + 1];
                pc += 2;
                if (index >= g_numScriptNatives || argc > 8 || m_sp - argc < base)
                {
                    m_faulted = true;
                    break;
                }
                ScriptValue args[8];
                for (int i = argc - 1; i >= 0; --i)
                    Pop(args[i]);
                ScriptValue result;
                result.type = SV_NULL;
                result.i = 0;
                g_scriptNatives[index](this, args, argc, &result);
                Push(result);
            }
            break;
        case OP_RET:
        case OP_RETV:
            {
                ScriptValue result = MakeInt(0);
                if (op == OP_RETV)
                    Pop(result);
                m_sp = base;
                Push(result);
                --m_depth;
                m_totalOps += ops;
                return ops;
            }
        case OP_POP:
            Pop(a);
            break;
        case OP_DUP:
            Pop(a);
            Push(a);
            Push(a);
            break;
        case OP_I2F:
            Pop(a);
            Push(MakeReal(ToReal(a)));
            break;
        case OP_F2I:
            Pop(a);
            Push(MakeInt(ToInt(a)));
            break;
        case OP_HALT:
        default:
            m_faulted = true;
            break;
        }
    }
}

bool CScriptVM::Call(int function, const ScriptValue* args, int argc, ScriptValue* result)
{
    if (!m_code || function < 0 || function >= m_numFunctions)
        return false;

    int savedSp = m_sp;
    int savedDepth = m_depth;
    int savedOps = m_totalOps;
    m_faulted = false;
    m_totalOps = 0;

    int base = m_sp;
    for (int i = 0; i < m_functions[function].numArgs; ++i)
        Push(i < argc ? args[i] : MakeInt(0));

    bool ok = Execute(function, base) >= 0 && !m_faulted;
    if (ok && result)
        *result = m_stack[base];

    m_sp = savedSp;
    m_depth = savedDepth;
    m_totalOps += savedOps;
    return ok;
}

void CScriptVM::Run()
{
    Call(0, 0, 0, 0);
}

void CScriptVM::TimerCallback(void* context, u32 param)
{
    CScriptVM* vm = static_cast<CScriptVM*>(context);
    if (param >= (u32)vm->m_numTriggers)
        return;
    const Trigger& t = vm->m_triggers[param];
    if (t.enabled)
        vm->Call(t.function, 0, 0, 0);
}

int CScriptVM::RegisterTrigger(int kind, int function, float period)
{
    if (m_numTriggers >= (int)COUNTOF(m_triggers) || function < 0 || function >= m_numFunctions)
        return -1;

    int index = m_numTriggers++;
    Trigger& t = m_triggers[index];
    t.kind = (u8)kind;
    t.function = (u8)function;
    t.enabled = 1;
    t.pad = 0;
    t.period = period;
    t.timerId = 0;
    if (kind == TRG_TIMER)
        t.timerId = g_game->m_timers.Schedule(period, period, TimerCallback, this, (u32)index);
    return index;
}

void CScriptVM::FireDeathTriggers(CUnit* dying, CUnit* killer)
{
    CUnit* savedUnit = m_eventUnit;
    CUnit* savedKiller = m_eventKiller;
    m_eventUnit = dying;
    m_eventKiller = killer;
    for (int i = 0; i < m_numTriggers; ++i)
    {
        if (m_triggers[i].kind == TRG_DEATH && m_triggers[i].enabled)
            Call(m_triggers[i].function, 0, 0, 0);
    }
    m_eventUnit = savedUnit;
    m_eventKiller = savedKiller;
}

// ---------------------------------------------------------------------------
// Natives

static CUnit* ArgUnit(const ScriptValue& v)
{
    return v.type == SV_HANDLE ? g_game->GetUnit(v.h) : 0;
}

static bool ValidPlayer(int p)
{
    return p >= 0 && p < kMaxPlayers;
}

static void N_GetGameTime(CScriptVM*, ScriptValue*, int, ScriptValue* r)
{
    *r = MakeReal(g_game->Time());
}

static void N_GetRandomInt(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    *r = MakeInt(g_game->m_rng.Range(ToInt(a[0]), ToInt(a[1])));
}

static void N_GetRandomReal(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    *r = MakeReal(g_game->m_rng.RangeF(ToReal(a[0]), ToReal(a[1])));
}

static void N_CreateUnit(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int player = ToInt(a[0]);
    int type = ToInt(a[1]);
    if (!ValidPlayer(player) || type < 0 || type >= UT_COUNT || (g_unitTypes[type].flags & UTF_STRUCTURE))
    {
        *r = MakeHandle(0);
        return;
    }
    Vec2 p(ToReal(a[2]), ToReal(a[3]));
    int cx = CTerrain::WorldToCell(p.x);
    int cy = CTerrain::WorldToCell(p.y);
    if (g_game->m_terrain.FindNearestWalkable(cx, cy, 8))
        p = Vec2(CTerrain::CellCenter(cx), CTerrain::CellCenter(cy));
    CUnit* u = g_game->CreateUnit(type, player, p, 0.0f);
    *r = MakeHandle(u ? u->m_handle : 0);
}

static void N_KillUnit(CScriptVM*, ScriptValue* a, int, ScriptValue*)
{
    CUnit* u = ArgUnit(a[0]);
    if (u)
        u->Kill(0);
}

static void N_GetUnitLife(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeReal(u ? u->m_life : 0.0f);
}

static void N_SetUnitLife(CScriptVM*, ScriptValue* a, int, ScriptValue*)
{
    CUnit* u = ArgUnit(a[0]);
    if (u && u->IsAlive())
        u->m_life = ClampF(ToReal(a[1]), 1.0f, u->m_maxLife);
}

static void N_GetTriggerUnit(CScriptVM* vm, ScriptValue*, int, ScriptValue* r)
{
    *r = MakeHandle(vm->m_eventUnit ? vm->m_eventUnit->m_handle : 0);
}

static void N_GetKillingUnit(CScriptVM* vm, ScriptValue*, int, ScriptValue* r)
{
    *r = MakeHandle(vm->m_eventKiller ? vm->m_eventKiller->m_handle : 0);
}

static void N_GetOwningPlayer(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeInt(u ? u->m_owner : -1);
}

static void N_AddPlayerGold(CScriptVM*, ScriptValue* a, int, ScriptValue*)
{
    int p = ToInt(a[0]);
    if (ValidPlayer(p))
        g_game->m_players[p].m_gold += ToInt(a[1]);
}

static void N_AddPlayerLumber(CScriptVM*, ScriptValue* a, int, ScriptValue*)
{
    int p = ToInt(a[0]);
    if (ValidPlayer(p))
        g_game->m_players[p].m_lumber += ToInt(a[1]);
}

static void N_GetPlayerGold(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int p = ToInt(a[0]);
    *r = MakeInt(ValidPlayer(p) ? g_game->m_players[p].m_gold : 0);
}

static void N_GetUnitTypeId(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeInt(u ? u->m_type : -1);
}

static void N_GetUnitX(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeReal(u ? u->m_pos.x : 0.0f);
}

static void N_GetUnitY(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeReal(u ? u->m_pos.y : 0.0f);
}

static void N_IssuePointOrder(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    if (!u)
    {
        *r = MakeBool(false);
        return;
    }
    COrder o;
    o.id = (u8)ToInt(a[1]);
    o.flags = OF_TARGET_POINT;
    o.param = 0;
    o.target = 0;
    o.point = Vec2(ToReal(a[2]), ToReal(a[3]));
    *r = MakeBool(u->IssueOrder(o, false));
}

static void N_CountPlayerStructures(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int p = ToInt(a[0]);
    *r = MakeInt(ValidPlayer(p) ? g_game->m_players[p].StructureCount() : 0);
}

static void N_SetPlayerDefeated(CScriptVM*, ScriptValue* a, int, ScriptValue*)
{
    int p = ToInt(a[0]);
    if (ValidPlayer(p))
        g_game->SetDefeated(p);
}

static void N_IsPlayerDefeated(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int p = ToInt(a[0]);
    *r = MakeBool(!ValidPlayer(p) || g_game->m_players[p].IsDefeated());
}

static void N_CreateItem(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int type = ToInt(a[0]);
    if (type < 0 || type >= IT_COUNT)
    {
        *r = MakeHandle(0);
        return;
    }
    CItem* item = g_game->CreateItem(type, Vec2(ToReal(a[1]), ToReal(a[2])));
    *r = MakeHandle(item ? item->m_handle : 0);
}

static void N_RegisterTimerTrigger(CScriptVM* vm, ScriptValue* a, int, ScriptValue* r)
{
    *r = MakeInt(vm->RegisterTrigger(TRG_TIMER, ToInt(a[1]), MaxF(ToReal(a[0]), 0.1f)));
}

static void N_RegisterDeathTrigger(CScriptVM* vm, ScriptValue* a, int, ScriptValue* r)
{
    *r = MakeInt(vm->RegisterTrigger(TRG_DEATH, ToInt(a[0]), 0.0f));
}

static void N_GetStartX(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int p = ToInt(a[0]);
    *r = MakeReal(ValidPlayer(p) ? g_game->m_players[p].m_start.x : kMapSize * 0.5f);
}

static void N_GetStartY(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    int p = ToInt(a[0]);
    *r = MakeReal(ValidPlayer(p) ? g_game->m_players[p].m_start.y : kMapSize * 0.5f);
}

static void N_GetUnitLevel(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeInt(u ? u->Level() : 0);
}

static void N_IsUnitHero(CScriptVM*, ScriptValue* a, int, ScriptValue* r)
{
    CUnit* u = ArgUnit(a[0]);
    *r = MakeBool(u && u->IsHero());
}

static void N_GetTimeOfDay(CScriptVM*, ScriptValue*, int, ScriptValue* r)
{
    *r = MakeReal(g_game->m_timeOfDay);
}

static void N_GetPlayerCount(CScriptVM*, ScriptValue*, int, ScriptValue* r)
{
    *r = MakeInt(g_game->m_numPlayers);
}

static void N_EndGame(CScriptVM*, ScriptValue* a, int, ScriptValue*)
{
    g_game->EndGame(ToInt(a[0]));
}

const ScriptNative g_scriptNatives[] =
{
    N_GetGameTime,
    N_GetRandomInt,
    N_GetRandomReal,
    N_CreateUnit,
    N_KillUnit,
    N_GetUnitLife,
    N_SetUnitLife,
    N_GetTriggerUnit,
    N_GetKillingUnit,
    N_GetOwningPlayer,
    N_AddPlayerGold,
    N_AddPlayerLumber,
    N_GetPlayerGold,
    N_GetUnitTypeId,
    N_GetUnitX,
    N_GetUnitY,
    N_IssuePointOrder,
    N_CountPlayerStructures,
    N_SetPlayerDefeated,
    N_IsPlayerDefeated,
    N_CreateItem,
    N_RegisterTimerTrigger,
    N_RegisterDeathTrigger,
    N_GetStartX,
    N_GetStartY,
    N_GetUnitLevel,
    N_IsUnitHero,
    N_GetTimeOfDay,
    N_GetPlayerCount,
    N_EndGame,
};

const int g_numScriptNatives = COUNTOF(g_scriptNatives);
