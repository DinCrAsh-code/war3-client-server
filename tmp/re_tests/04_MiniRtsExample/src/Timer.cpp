#include "Engine.h"

// Min-heap of pending timers ordered by due time (ties broken by id so
// firing order is deterministic).

static bool TimerLess(float dueA, u32 idA, float dueB, u32 idB)
{
    if (dueA != dueB)
        return dueA < dueB;
    return idA < idB;
}

void CTimerQueue::Init()
{
    m_heap.Clear();
    m_nextId = 1;
    m_now = 0.0f;
}

u32 CTimerQueue::Schedule(float delay, float period, TimerFn fn, void* context, u32 param)
{
    Timer t;
    t.due = m_now + delay;
    t.period = period;
    t.fn = fn;
    t.context = context;
    t.param = param;
    t.id = m_nextId++;
    Push(t);
    return t.id;
}

void CTimerQueue::Advance(float now)
{
    m_now = now;
    int guard = 0;
    while (m_heap.Count() > 0 && m_heap[0].due <= now && guard < 256)
    {
        Timer t = Pop();
        ++guard;
        if (!t.fn)
            continue;

        t.fn(t.context, t.param);

        if (t.period > 0.0f)
        {
            t.due += t.period;
            if (t.due <= now)
                t.due = now + t.period;
            Push(t);
        }
    }
}

void CTimerQueue::Push(const Timer& t)
{
    m_heap.Add(t);
    SiftUp(m_heap.Count() - 1);
}

CTimerQueue::Timer CTimerQueue::Pop()
{
    Timer top = m_heap[0];
    int last = m_heap.Count() - 1;
    m_heap[0] = m_heap[last];
    m_heap.SetCount(last);
    if (last > 0)
        SiftDown(0);
    return top;
}

void CTimerQueue::SiftUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) >> 1;
        if (!TimerLess(m_heap[index].due, m_heap[index].id, m_heap[parent].due, m_heap[parent].id))
            break;
        Timer tmp = m_heap[parent];
        m_heap[parent] = m_heap[index];
        m_heap[index] = tmp;
        index = parent;
    }
}

void CTimerQueue::SiftDown(int index)
{
    int count = m_heap.Count();
    for (;;)
    {
        int left = index * 2 + 1;
        int right = left + 1;
        int best = index;
        if (left < count && TimerLess(m_heap[left].due, m_heap[left].id, m_heap[best].due, m_heap[best].id))
            best = left;
        if (right < count && TimerLess(m_heap[right].due, m_heap[right].id, m_heap[best].due, m_heap[best].id))
            best = right;
        if (best == index)
            break;
        Timer tmp = m_heap[best];
        m_heap[best] = m_heap[index];
        m_heap[index] = tmp;
        index = best;
    }
}
