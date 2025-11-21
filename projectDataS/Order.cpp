#include "Order.h"
#include <iostream>
using namespace std;

Order::Order(int id, int at, OrderType t, int sz, float m)
    : ID(id), arrivalTime(at), type(t), size(sz), money(m),
    assignTime(-1), finishTime(-1),
    originalSize(sz), dishesCompleted(0)
{
}

float Order::calculatePriority(int currentTime) const {
    int waitingTime = currentTime - arrivalTime;
    return (waitingTime * 0.5f) + (money * 0.3f) + (size * 0.2f);
}

void Order::display() const {
    cout << "Order " << ID << " [" << getTypeString() << "]";
    cout << " - AT:" << arrivalTime;
    cout << ", Size:" << size;
    cout << ", $" << money;

    if (assignTime != -1) {
        cout << ", WT:" << getWT();
    }

    if (finishTime != -1) {
        cout << ", FT:" << finishTime;
        cout << ", ST:" << getST();
    }

    cout << endl;
}
