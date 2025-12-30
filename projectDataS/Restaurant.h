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
#include "GUI/GUI.h"

class Restaurant {
private:
    // Waiting orders (not yet assigned to cooks)
    LinkedQueue<Order*> normalWaitingOrders;
    LinkedQueue<Order*> veganWaitingOrders;
    PriorityQueue<Order*> vipWaitingOrders;
    LinkedQueue<Order*> quickWaitingOrders;    // Express/Quick orders - FIFO
    LinkedQueue<Order*> cateringWaitingOrders; // Catering orders - FIFO



    // All cooks for statistics
    vector<Cook*> allCooks;

    // Events queue
    LinkedQueue<Event*> events;

    // Configuration
    int numNormalCooks, numVeganCooks, numVIPCooks;
    int numQuickCooks, numCateringCooks;  // NEW: Quick and Catering cook counts
    int speedNormal, speedVegan, speedVIP;
    int speedQuick, speedCatering;        // NEW: Quick and Catering base speeds
    int BO, BN, BG, BV;
    int BQ, BC;                           // NEW: Break durations for Quick and Catering
    int autoPromoteTime;
    int eventCount;
    
    // Statistics for new order types
    int totalQuickOrders;
    int totalCateringOrders;

    // GUI 
    GUI* pGUI;
    PROG_MODE mode;

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
    void runSimulationWithGUI();

    void setGUI(GUI* pGUI);
    void setMode(PROG_MODE mode);

    void displayCurrentState();

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
    bool assignQuickOrders();     // NEW: Assign Express orders
    bool assignCateringOrders();  // NEW: Assign Catering orders
    bool attemptPreemption(Order* vipOrder);


    // Output
    void generateOutputToConsole();
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
