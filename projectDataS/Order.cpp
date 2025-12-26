#include "Order.h"
#include <iostream>

using namespace std;

// Constructor
Order::Order(int id, int at, OrderType t, int sz, float m)
    : ID(id), arrivalTime(at), type(t), originalType(t),  // ? SAVE ORIGINAL TYPE!
    size(sz), money(m),
    assignTime(-1), finishTime(-1),
    originalSize(sz), dishesCompleted(0)
{
    // ? CALCULATE DEADLINE AT CREATION:
    deadline = calculateDeadline();
    // ? INITIALIZE LATENESS:
    late = false;  // Not late initially
}

// Priority calculation for VIP orders
float Order::calculatePriority(int currentTime) const {
    // Weighted formula:
    // Priority = w1*waitTime + w2*price + w3*size
    int waitingTime = currentTime - arrivalTime;

    // Weights (you can adjust these)
    float w1 = 0.5f;  // Wait time weight
    float w2 = 0.3f;  // Price weight
    float w3 = 0.2f;  // Size weight

    return (waitingTime * w1) + (money * w2) + (size * w3);
}

// Display order information
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

        // ? SHOW DEADLINE INFO:
        cout << ", Deadline:" << deadline;
        if (late) {
            int lateness = finishTime - deadline;
            cout << " [LATE by " << lateness << " steps]";
        }
        else {
            cout << " [ON TIME]";
        }
    }

    // ? SHOW PREEMPTION INFO:
    if (hasBeenPreempted()) {
        cout << " [PREEMPTED: " << dishesCompleted
            << "/" << originalSize << " dishes completed]";
    }

    cout << endl;
}
