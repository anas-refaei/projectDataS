#include "Event.h"

Event::Event(int ts, char typ, int id, int size, float money)
    : type(EVENT_ARRIVAL), timestamp(ts), orderTypeChar(typ),
    orderID(id), orderSize(size), orderMoney(money),
    targetID(0), extraMoney(0.0f)
{
}

Event::Event(int ts, int id)
    : type(EVENT_CANCELLATION), timestamp(ts), targetID(id),
    orderTypeChar(' '), orderID(0), orderSize(0),
    orderMoney(0.0f), extraMoney(0.0f)
{
}

Event::Event(int ts, int id, float exMoney)
    : type(EVENT_PROMOTION), timestamp(ts), targetID(id),
    extraMoney(exMoney), orderTypeChar(' '), orderID(0),
    orderSize(0), orderMoney(0.0f)
{
}
