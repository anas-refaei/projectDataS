#include "Restaurant.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;


Restaurant::Restaurant() : currentTime(0), eventCount(0) {
    numNormalCooks = 0;
    numVeganCooks = 0;
    numVIPCooks = 0;
    speedNormal = 0;
    speedVegan = 0;
    speedVIP = 0;
    BO = 0;
    BN = 0;
    BG = 0;
    BV = 0;
    autoPromoteTime = 0;

   
    inServiceCount = 0;
    finishedCount = 0;
}

Restaurant::~Restaurant() {

    Cook* cook;
    while (availableNormalCooks.dequeue(cook)) {
        delete cook;
    }
    while (availableVeganCooks.dequeue(cook)) {
        delete cook;
    }
    while (availableVIPCooks.dequeue(cook)) {
        delete cook;
    }

    Order* order;
    while (normalWaitingOrders.dequeue(order)) {
        delete order;
    }
    while (veganWaitingOrders.dequeue(order)) {
        delete order;
    }
    while (vipWaitingOrders.dequeue(order)) {
        delete order;
    }

    
    inServiceOrders.DeleteAll();   
    finishedOrders.DeleteAll();    
   

    Event* event;
    while (events.dequeue(event)) {
        delete event;
    }
}


bool Restaurant::loadInputFile(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    inputFile >> numNormalCooks >> numVeganCooks >> numVIPCooks;
    inputFile >> speedNormal >> speedVegan >> speedVIP;
    inputFile >> BO >> BN >> BG >> BV;
    inputFile >> autoPromoteTime;
    inputFile >> eventCount;

    createCooks();

    string line;
    getline(inputFile, line);

    for (int i = 0; i < eventCount; i++) {
        getline(inputFile, line);
        istringstream iss(line);

        char eventType;
        iss >> eventType;

        if (eventType == 'R') {
            char orderType;
            int ts, id, size;
            float money;
            iss >> orderType >> ts >> id >> size >> money;
            Event* event = new Event(ts, orderType, id, size, money);
            events.enqueue(event);
        }
        else if (eventType == 'X') {
            int ts, id;
            iss >> ts >> id;
            Event* event = new Event(ts, id);
            events.enqueue(event);
        }
        else if (eventType == 'P') {
            int ts, id;
            float exMoney;
            iss >> ts >> id >> exMoney;
            Event* event = new Event(ts, id, exMoney);
            events.enqueue(event);
        }
    }

    inputFile.close();
    cout << "\nFile loaded successfully!" << endl;
    return true;
}

void Restaurant::createCooks() {
    for (int i = 1; i <= numNormalCooks; i++) {
        Cook* cook = new Cook('N', speedNormal, BN, i);
        availableNormalCooks.enqueue(cook);
    }

    for (int i = 1; i <= numVeganCooks; i++) {
        Cook* cook = new Cook('G', speedVegan, BG, i);
        availableVeganCooks.enqueue(cook);
    }

    for (int i = 1; i <= numVIPCooks; i++) {
        Cook* cook = new Cook('V', speedVIP, BV, i);
        availableVIPCooks.enqueue(cook);
    }
}

void Restaurant::processArrivalEvent(Event* event) {
    OrderType type;
    char orderChar = event->getOrderTypeChar();

    if (orderChar == 'N') type = TYPE_NRM;
    else if (orderChar == 'G') type = TYPE_VEG;
    else type = TYPE_VIP;

    Order* order = new Order(
        event->getOrderID(),
        event->getTimestamp(),
        type,
        event->getOrderSize(),
        event->getOrderMoney()
    );

    cout << "  Order ID: " << order->getID() << endl;
    cout << "  Type: " << order->getTypeString() << endl;
    cout << "  Size: " << order->getSize() << " dishes" << endl;
    cout << "  Price: $" << order->getMoney() << endl;

    if (type == TYPE_NRM) {
        normalWaitingOrders.enqueue(order);
        cout << "  Status: Added to Normal waiting queue" << endl;
    }
    else if (type == TYPE_VEG) {
        veganWaitingOrders.enqueue(order);
        cout << "  Status: Added to Vegan waiting queue" << endl;
    }
    else {
        float priority = order->calculatePriority(currentTime);
        vipWaitingOrders.enqueue(order, priority);
        cout << "  Priority: " << priority << endl;
        cout << "  Status: Added to VIP waiting queue" << endl;
    }
}


//event is  Event object that contains the cancellation information from your input file ( Type of the event , timestamp , targetID)
void Restaurant::processCancellationEvent(Event* event) {
    int targetID = event->getTargetID();                 //Extract the order ID to cancel from the event object
    cout << "  Target Order ID: " << targetID << endl;   // Tell the user which order we're looking for
    bool found = false;                                  // Create a marker that says 'I haven't found the order yet
    LinkedQueue<Order*> tempQueue;                       // Create a temporary empty queue to hold orders we want to KEEP "Create a temporary holding area for orders we're keeping"
    Order* order;                                        //Declare a pointer variable to hold each order as we check it
    while (normalWaitingOrders.dequeue(order)) {         // Loop that removes orders one-by-one from the Normal waiting queue
        if (order->getID() == targetID) {                //Check if this order is the one we want to cancel
        
            cout << "  Status: Order " << targetID << " cancelled successfully" << endl;
            delete order;                               //Tell the user we found and cancelled their order
            found = true;
        }
        else {
           
            tempQueue.enqueue(order);                   //If this is a different order (not the one to cancel)
        }
    }
    while (tempQueue.dequeue(order)) {                  //Loop to take orders back OUT of temporary queue(Put the kept orders back into the original queue)
        normalWaitingOrders.enqueue(order);             //Put each order back into the Normal waiting queue
    }
    if (!found) {                                       //heck if we DIDN'T find the order (found is still false)
        cout << "  Status: Order " << targetID << " not found in waiting queue" << endl;
    }
}

void Restaurant::processPromotionEvent(Event* event) {
    cout << "  Target Order ID: " << event->getTargetID() << endl;
    cout << "  Extra Money: $" << event->getExtraMoney() << endl;
    cout << "  Status: Order promotion requested" << endl;
    cout << "  Note: Full promotion logic in Phase 2" << endl;
}


void Restaurant::displayCurrentState() const {
    cout << "\n--- Current State (Time=" << currentTime << ") ---" << endl;

    int totalWaiting = normalWaitingOrders.size() +
        veganWaitingOrders.size() +
        vipWaitingOrders.size();

    int totalCooks = availableNormalCooks.size() +
        availableVeganCooks.size() +
        availableVIPCooks.size();

    cout << "\nWaiting Orders:" << endl;
    cout << "  Normal Queue:  " << normalWaitingOrders.size() << " order(s)" << endl;
    cout << "  Vegan Queue:   " << veganWaitingOrders.size() << " order(s)" << endl;
    cout << "  VIP Queue:     " << vipWaitingOrders.size() << " order(s)" << endl;
    cout << "  Total Waiting: " << totalWaiting << " order(s)" << endl;


    cout << "\nOrder Status:" << endl;
    cout << "  In-Service:    " << inServiceCount << " order(s)" << endl;
    cout << "  Finished:      " << finishedCount << " order(s)" << endl;

    cout << "\nAvailable Cooks:" << endl;
    cout << "  Normal Cooks:  " << availableNormalCooks.size() << "/" << numNormalCooks << endl;
    cout << "  Vegan Cooks:   " << availableVeganCooks.size() << "/" << numVeganCooks << endl;
    cout << "  VIP Cooks:     " << availableVIPCooks.size() << "/" << numVIPCooks << endl;
    cout << "  Total Available: " << totalCooks << "/"
        << (numNormalCooks + numVeganCooks + numVIPCooks) << endl;

    cout << "----------------------------------------" << endl;
}




void Restaurant::runPhase1Simulation() {
    cout << "\n========================================" << endl;
    cout << "   PHASE 1 SIMULATION (Interactive)" << endl;
    cout << "========================================" << endl;

    cout << "\nLoaded Parameters:" << endl;
    cout << "Cooks: N=" << numNormalCooks << ", G=" << numVeganCooks
        << ", V=" << numVIPCooks << endl;
    cout << "Speeds: N=" << speedNormal << ", G=" << speedVegan
        << ", V=" << speedVIP << " dishes/timestep" << endl;
    cout << "Auto-Promote Time: " << autoPromoteTime << " timesteps" << endl;

    cout << "\nPress Enter to start simulation...";
    cin.get();

  
    while (!events.isEmpty()) {
        cout << "\n========================================" << endl;
        cout << "         Timestep " << currentTime << endl;
        cout << "========================================" << endl;

       
        bool hadEvents = false;

        while (!events.isEmpty()) {
            Event* event;
            if (events.peek(event) && event->getTimestamp() == currentTime) {
                events.dequeue(event);
                hadEvents = true;

                cout << "\n[EVENT] ";

                if (event->getType() == EVENT_ARRIVAL) {
                    cout << "Order Arrival" << endl;
                    processArrivalEvent(event);
                }
                else if (event->getType() == EVENT_CANCELLATION) {
                    cout << "Order Cancellation" << endl;
                    processCancellationEvent(event);
                }
                else if (event->getType() == EVENT_PROMOTION) {
                    cout << "Order Promotion" << endl;
                    processPromotionEvent(event);
                }

                delete event;
            }
            else {
                break; 
            }
        }

        if (!hadEvents) {
            cout << "\n[No events at this timestep]" << endl;
        }

        if (currentTime > 0) {  
            assignOrdersPhase1();
        }

        completeOrdersPhase1();
       
        displayCurrentState();

        cout << "\nPress Enter to continue...";
        cin.get();

        currentTime++;
    }

    cout << "\n========================================" << endl;
    cout << "      SIMULATION COMPLETE" << endl;
    cout << "========================================" << endl;
    cout << "\nTotal timesteps simulated: " << currentTime << endl;
}

void Restaurant::assignOrdersPhase1() {
    cout << "\n[Assignment Phase]" << endl;
    bool anyAssigned = false;

 
    Order* normalOrder;
    if (normalWaitingOrders.dequeue(normalOrder)) {
        normalOrder->setAssignTime(currentTime);
        inServiceOrders.InsertEnd(normalOrder);
        inServiceCount++; 
        cout << "  -> Assigned Normal order " << normalOrder->getID()
            << " to in-service" << endl;
        anyAssigned = true;
    }

    Order* veganOrder;
    if (veganWaitingOrders.dequeue(veganOrder)) {
        veganOrder->setAssignTime(currentTime);
        inServiceOrders.InsertEnd(veganOrder);
        inServiceCount++; 
        cout << "  -> Assigned Vegan order " << veganOrder->getID()
            << " to in-service" << endl;
        anyAssigned = true;
    }

    Order* vipOrder;
    if (vipWaitingOrders.dequeue(vipOrder)) {
        vipOrder->setAssignTime(currentTime);
        inServiceOrders.InsertEnd(vipOrder);
        inServiceCount++; 
        cout << "  -> Assigned VIP order " << vipOrder->getID()
            << " to in-service" << endl;
        anyAssigned = true;
    }

    if (!anyAssigned) {
        cout << "  -> No orders available to assign" << endl;
    }
}

void Restaurant::completeOrdersPhase1() {
    
    if (currentTime % 5 != 0 || currentTime == 0) {
        return;  
    }

    cout << "\n[Completion Phase - Timestep " << currentTime << "]" << endl;

    if (inServiceCount == 0) {
        cout << "  -> No orders in service to complete" << endl;
        return;
    }

    int completedCount = 0;
    int maxToComplete = (inServiceCount < 3) ? inServiceCount : 3;

    for (int i = 0; i < maxToComplete; i++) {
        
        inServiceCount--;
        finishedCount++;
        completedCount++;
    }

    cout << "  -> Completed " << completedCount << " order(s)" << endl;
    cout << "  -> Moved from in-service to finished" << endl;
}
