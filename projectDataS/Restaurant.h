#ifndef RESTAURANT_H
#define RESTAURANT_H

#include "Queue.h"
#include "PriorityQueue.h"
#include "LinkedList.h"
#include "Order.h"
#include "Cook.h"
#include "Event.h"
#include <string>
#include <vector>

class Restaurant {
private:
    // Waiting orders (not yet assigned to cooks)
    LinkedQueue<Order*> normalWaitingOrders;
    LinkedQueue<Order*> veganWaitingOrders;
    PriorityQueue<Order*> vipWaitingOrders;



    // All cooks for statistics
    vector<Cook*> allCooks;

    // Events queue
    LinkedQueue<Event*> events;

    // Configuration
    int numNormalCooks, numVeganCooks, numVIPCooks;
    int speedNormal, speedVegan, speedVIP;
    int BO, BN, BG, BV;
    int autoPromoteTime;
    int eventCount;

    // Simulation state
    int currentTime;

    // Finished orders
    LinkedList<Order*> finishedOrders;

    // Statistics
    int totalNormalOrders;
    int totalVeganOrders;
    int totalVIPOrders;
    int autoPromotedCount;
    /////////////////////////
    Order* findOrderToPreempt();
    void preemptOrder(Order* order, Cook* cook);
    void checkHealthEmergencies();
    void checkForOvertime();
    void applyFatigueToCooks();
    void finishOrder(Cook* cook);

public:
    Restaurant();
    ~Restaurant();

    bool loadInputFile(const string& filename);
    void runPhase2Simulation();
    void displayCurrentState() ;

    // Event processing
    void processArrivalEvent(Event* event);
    void processCancellationEvent(Event* event);
    void processPromotionEvent(Event* event);

    // Phase 2 core functions
    void assignOrdersPhase2();
    void updateCooksAndOrders();
    void checkAutoPromotions();

    // Assignment helpers
    bool assignVIPOrders();
    bool assignVeganOrders();
    bool assignNormalOrders();
    bool attemptPreemption(Order* vipOrder);


    // Output
    void generateOutputFile(const string& filename);
/// <summary>
/// //////////////////////////////////////////////////////
    Cook* getAvailableCook(char type);
    int countCooksByState(char type, CookState state);
    int countAvailableCooks(char type);
    int countBusyCooks();
    int countBreakCooks();


private:
    void createCooks();
    Cook* getAvailableCookForType(char type);
    void returnCookToPool(Cook* cook);
};

#endif
