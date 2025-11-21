#ifndef EVENT_H
#define EVENT_H

#include "Definitions.h"

class Event {
private:
    EventType type;
    int timestamp;

    char orderTypeChar;
    int orderID;
    int orderSize;
    float orderMoney;

    int targetID;
    float extraMoney;

public:
    Event(int ts, char typ, int id, int size, float money);
    Event(int ts, int id);
    Event(int ts, int id, float exMoney);

    EventType getType() const { return type; }
    int getTimestamp() const { return timestamp; }
    char getOrderTypeChar() const { return orderTypeChar; }
    int getOrderID() const { return orderID; }
    int getOrderSize() const { return orderSize; }
    float getOrderMoney() const { return orderMoney; }
    int getTargetID() const { return targetID; }
    float getExtraMoney() const { return extraMoney; }
};

#endif

