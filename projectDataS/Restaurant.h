#ifndef RESTAURANT_H
#define RESTAURANT_H

#include "Queue.h"
#include "PriorityQueue.h"
#include "LinkedList.h"
#include "Order.h"
#include "Cook.h"
#include "Event.h"
#include <string>

class Restaurant {
private:
    LinkedQueue<Order*> normalWaitingOrders;
    LinkedQueue<Order*> veganWaitingOrders;
    PriorityQueue<Order*> vipWaitingOrders;

    LinkedQueue<Cook*> availableNormalCooks;
    LinkedQueue<Cook*> availableVeganCooks;
    LinkedQueue<Cook*> availableVIPCooks;

    LinkedQueue<Event*> events;

    int numNormalCooks, numVeganCooks, numVIPCooks;
    int speedNormal, speedVegan, speedVIP;
    int BO, BN, BG, BV;
    int autoPromoteTime;
    int eventCount;
    int currentTime;

public:
    Restaurant();
    ~Restaurant();

    bool loadInputFile(const std::string& filename);
    void runPhase1Simulation();
    void displayCurrentState() const;
    void processArrivalEvent(Event* event);
    void processCancellationEvent(Event* event);
    void processPromotionEvent(Event* event);

private:
    void createCooks();
};

#endif

