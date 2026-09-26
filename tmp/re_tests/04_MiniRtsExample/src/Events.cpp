#include "Engine.h"

void CEventBus::Register(int type, EventFn fn, void* context)
{
    if (type < 0 || type >= EVT_COUNT)
        return;
    Listener l;
    l.fn = fn;
    l.context = context;
    if (m_listeners[type].Find(l) < 0)
        m_listeners[type].Add(l);
}

void CEventBus::Fire(int type, CUnit* unit, CUnit* other, float value, int param)
{
    EventData e;
    e.type = type;
    e.unit = unit;
    e.other = other;
    e.value = value;
    e.param = param;

    TArray<Listener>& list = m_listeners[type];
    for (int i = 0; i < list.Count(); ++i)
        list[i].fn(e, list[i].context);
}
