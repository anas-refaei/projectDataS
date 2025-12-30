#include "Restaurant.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>     

using namespace std;

Restaurant::Restaurant() {
    currentTime = 0;
    eventCount = 0;
    numNormalCooks = 0;
    numVeganCooks = 0;
    numVIPCooks = 0;
    numQuickCooks = 0;      
    numCateringCooks = 0;   
    speedNormal = 0;
    speedVegan = 0;
    speedVIP = 0;
    speedQuick = 0;         
    speedCatering = 0;      
    BO = 0;
    BN = 0;
    BG = 0;
    BV = 0;
    BQ = 0;                 
    BC = 0;                
    autoPromoteTime = 0;
    totalNormalOrders = 0;
    totalVeganOrders = 0;
    totalVIPOrders = 0;
    totalQuickOrders = 0;   
    totalCateringOrders = 0; 
    autoPromotedCount = 0;
}

Restaurant::~Restaurant() {
    for (Cook* cook : allCooks) {
        delete cook;
    }
    Order* order;
    while (normalWaitingOrders.dequeue(order)) delete order;
    while (veganWaitingOrders.dequeue(order)) delete order;
    while (vipWaitingOrders.dequeue(order)) delete order;
    while (quickWaitingOrders.dequeue(order)) delete order;    
    while (cateringWaitingOrders.dequeue(order)) delete order;

    finishedOrders.DeleteAll();

    Event* event;
    while (events.dequeue(event)) delete event;
}

////Anas

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

    // ? NEW: Set default values for Quick and Catering cooks
    // Quick cooks: fast service, 1 cook per 2 normal cooks
    numQuickCooks = max(1, numNormalCooks / 2);
    speedQuick = (int)(speedNormal * 1.5);  // 50% faster than normal
    BQ = BN;  // Same break duration as normal

    // Catering cooks: bulk orders, 1 cook per 3 normal cooks
    numCateringCooks = max(1, numNormalCooks / 3);
    speedCatering = speedNormal;  // Same speed but handles larger orders
    BC = BN;  // Same break duration

    cout << "=== NEW COOK TYPES ===" << endl;
    cout << "  Quick Cooks: " << numQuickCooks << " (speed: " << speedQuick << ")" << endl;
    cout << "  Catering Cooks: " << numCateringCooks << " (speed: " << speedCatering << ")" << endl;
    cout << "======================" << endl;

    createCooks();

    string line;
    getline(inputFile, line); // Clear newline

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
    cout << "File loaded successfully!" << endl;
    return true;
}

//sayed noman
void Restaurant::createCooks() {
    int cookID = 1;
    auto getVariableSpeed = [](int baseSpeed) {
        int variation = (rand() % 41) - 20;  
        int variedSpeed = baseSpeed + (baseSpeed * variation / 100);
        return (variedSpeed > 0) ? variedSpeed : 1;  
    };

    
    auto getVariableBreak = [](int baseBreak) {
        int variation = (rand() % 31) - 15; 
        int variedBreak = baseBreak + (baseBreak * variation / 100);
        return (variedBreak > 0) ? variedBreak : 1; 
    };
    for (int i = 0; i < numNormalCooks; i++) {
        int individualSpeed = getVariableSpeed(speedNormal);
        int individualBreak = getVariableBreak(BN);
        Cook* cook = new Cook('N', individualSpeed, BO, individualBreak, cookID++);
        allCooks.push_back(cook);
        cout << "  Created Normal Cook " << (cookID-1) << " with speed " << individualSpeed 
             << " (base: " << speedNormal << ")" << endl;
    }
    for (int i = 0; i < numVeganCooks; i++) {
        int individualSpeed = getVariableSpeed(speedVegan);
        int individualBreak = getVariableBreak(BG);
        Cook* cook = new Cook('G', individualSpeed, BO, individualBreak, cookID++);
        allCooks.push_back(cook);
        cout << "  Created Vegan Cook " << (cookID-1) << " with speed " << individualSpeed 
             << " (base: " << speedVegan << ")" << endl;
    }

    for (int i = 0; i < numVIPCooks; i++) {
        int individualSpeed = getVariableSpeed(speedVIP);
        int individualBreak = getVariableBreak(BV);
        Cook* cook = new Cook('V', individualSpeed, BO, individualBreak, cookID++);
        allCooks.push_back(cook);
        cout << "  Created VIP Cook " << (cookID-1) << " with speed " << individualSpeed 
             << " (base: " << speedVIP << ")" << endl;
    }

    for (int i = 0; i < numQuickCooks; i++) {
        int individualSpeed = getVariableSpeed(speedQuick);
        int individualBreak = getVariableBreak(BQ);
        Cook* cook = new Cook('Q', individualSpeed, BO, individualBreak, cookID++);
        allCooks.push_back(cook);
        cout << "  Created Quick Cook " << (cookID-1) << " with speed " << individualSpeed 
             << " (base: " << speedQuick << ")" << endl;
    }

    for (int i = 0; i < numCateringCooks; i++) {
        int individualSpeed = getVariableSpeed(speedCatering);
        int individualBreak = getVariableBreak(BC);
        Cook* cook = new Cook('C', individualSpeed, BO, individualBreak, cookID++);
        allCooks.push_back(cook);
        cout << "  Created Catering Cook " << (cookID-1) << " with speed " << individualSpeed 
             << " (base: " << speedCatering << ")" << endl;
    }
}

//ANAS 

void Restaurant::processArrivalEvent(Event* event) {
    OrderType type;
    char orderChar = event->getOrderTypeChar();

    if (orderChar == 'N') {
        type = TYPE_NRM;
        totalNormalOrders++;
    }
    else if (orderChar == 'G') {
        type = TYPE_VEG;
        totalVeganOrders++;
    }
    else if (orderChar == 'V') {
        type = TYPE_VIP;
        totalVIPOrders++;
    }
    else if (orderChar == 'Q') {  
        type = TYPE_QCK;
        totalQuickOrders++;
    }
    else if (orderChar == 'C') {  
        type = TYPE_CTR;
        totalCateringOrders++;
    }
    else {
       
        type = TYPE_NRM;
        totalNormalOrders++;
    }

    Order* order = new Order(
        event->getOrderID(),
        event->getTimestamp(),
        type,
        event->getOrderSize(),
        event->getOrderMoney()
    );

    cout << "EVENT: Order " << order->getID() << " (" << order->getTypeString()
        << ") arrived" << endl;
    if (type == TYPE_NRM) {
        normalWaitingOrders.enqueue(order);
    }
    else if (type == TYPE_VEG) {
        veganWaitingOrders.enqueue(order);
    }
    else if (type == TYPE_VIP) {
        float priority = order->calculatePriority(currentTime);
        vipWaitingOrders.enqueue(order, priority);
    }
    else if (type == TYPE_QCK) {  
        quickWaitingOrders.enqueue(order);
    }
    else if (type == TYPE_CTR) {  
        cateringWaitingOrders.enqueue(order);
    }
}
//ANAS

void Restaurant::processCancellationEvent(Event* event) {
    int targetID = event->getTargetID();
    cout << "EVENT: Cancellation request for Order " << targetID << endl;

    LinkedQueue<Order*> tempQueue;
    Order* order;
    bool found = false;

    while (normalWaitingOrders.dequeue(order)) {
        if (order->getID() == targetID) {
            cout << "  -> Order " << targetID << " cancelled successfully" << endl;
            delete order;
            found = true;
        }
        else {
            tempQueue.enqueue(order);
        }
    }

    while (tempQueue.dequeue(order)) {
        normalWaitingOrders.enqueue(order);
    }

    if (!found) {
        cout << "  -> Order " << targetID << " not found in waiting queue" << endl;
    }
}
//ANAS

void Restaurant::processPromotionEvent(Event* event) {
    int targetID = event->getTargetID();
    float extraMoney = event->getExtraMoney();

    cout << "EVENT: Promotion request for Order " << targetID
        << " (Extra: " << extraMoney << ")" << endl;

    LinkedQueue<Order*> tempQueue;
    Order* order;
    bool found = false;

    while (normalWaitingOrders.dequeue(order)) {
        if (order->getID() == targetID) {
            order->setType(TYPE_VIP);
            order->addMoney(extraMoney);

            float priority = order->calculatePriority(currentTime);
            vipWaitingOrders.enqueue(order, priority);

            cout << "  -> Order " << targetID << " promoted to VIP" << endl;
            totalNormalOrders--;
            totalVIPOrders++;
            found = true;
        }
        else {
            tempQueue.enqueue(order);
        }
    }

    while (tempQueue.dequeue(order)) {
        normalWaitingOrders.enqueue(order);
    }

    if (!found) {
        cout << "  -> Order " << targetID << " not found in Normal waiting queue" << endl;
    }
}
//ANAS
void Restaurant::checkAutoPromotions() {
    if (autoPromoteTime <= 0) return;

    LinkedQueue<Order*> tempQueue;
    Order* order;

    while (normalWaitingOrders.dequeue(order)) {
        int waitingTime = currentTime - order->getAT();

        if (waitingTime >= autoPromoteTime) {
            order->setType(TYPE_VIP);
            float priority = order->calculatePriority(currentTime);
            vipWaitingOrders.enqueue(order, priority);

            cout << "  -> Order " << order->getID()
                << " auto-promoted to VIP (waited " << waitingTime << " steps)" << endl;

            autoPromotedCount++;
            totalNormalOrders--;
            totalVIPOrders++;
        }
        else {
            tempQueue.enqueue(order);
        }
    }

    while (tempQueue.dequeue(order)) {
        normalWaitingOrders.enqueue(order);
    }
}

//sayed noman
void Restaurant::assignOrdersPhase2() {
    cout << "\n--- Assignment Phase ---" << endl;
    if (assignVIPOrders()) {
        cout << "  VIP orders assigned" << endl;
    }
    if (assignVeganOrders()) {
        cout << "  Vegan orders assigned" << endl;
    }
    if (assignQuickOrders()) {
        cout << "  Express orders assigned" << endl;
    }
    if (assignNormalOrders()) {
        cout << "  Normal orders assigned" << endl;
    }
    if (assignCateringOrders()) {
        cout << "  Catering orders assigned" << endl;
    }
}
//sayed noman
bool Restaurant::assignVIPOrders() {
    bool anyAssigned = false;

    while (!vipWaitingOrders.isEmpty()) {
        Order* vipOrder;
        if (!vipWaitingOrders.peek(vipOrder)) break;

        Cook* cook = nullptr;
        cook = getAvailableCook('V');
        if (!cook) {
            cook = getAvailableCook('N');
        }
        if (!cook) {
            cook = getAvailableCook('G');
        }
        if (!cook) {
            Order* toPreempt = findOrderToPreempt();

            if (toPreempt != nullptr) {
                Cook* freedCook = nullptr;
                for (Cook* c : allCooks) {
                    if (c->getCurrentOrder() == toPreempt) {
                        freedCook = c;
                        break;
                    }
                }

                if (freedCook != nullptr) {
                    preemptOrder(toPreempt, freedCook);
                    vipWaitingOrders.dequeue(vipOrder);
                    freedCook->assignOrder(vipOrder, currentTime);
                    cout << "  -> Cook " << freedCook->getType() << freedCook->getId()
                        << " assigned to VIP Order " << vipOrder->getID()
                        << " (after preemption)" << endl;

                    anyAssigned = true;
                    continue;
                }
            }
            break;
        }

        if (cook) {
            vipWaitingOrders.dequeue(vipOrder);
            cook->assignOrder(vipOrder, currentTime);
            anyAssigned = true;
        }
        else {
            break;
        }
    }

    return anyAssigned;
}

//sayed noman
bool Restaurant::assignVeganOrders() {
    bool anyAssigned = false;

    while (!veganWaitingOrders.isEmpty()) {
        Cook* cook = getAvailableCook('G');
        if (!cook) break;

        Order* veganOrder;
        veganWaitingOrders.dequeue(veganOrder);
        cook->assignOrder(veganOrder, currentTime);
        anyAssigned = true;
    }

    return anyAssigned;
}
//sayed noman
bool Restaurant::assignNormalOrders() {
    bool anyAssigned = false;

    while (!normalWaitingOrders.isEmpty()) {
        Cook* cook = nullptr;
        cook = getAvailableCook('N');
        if (!cook) {
            cook = getAvailableCook('V');
        }
        if (!cook) break;
        Order* normalOrder;
        normalWaitingOrders.dequeue(normalOrder);
        cook->assignOrder(normalOrder, currentTime);
        anyAssigned = true;
    }
    return anyAssigned;
}
//sayed noman
bool Restaurant::assignQuickOrders() {
    bool anyAssigned = false;

    while (!quickWaitingOrders.isEmpty()) {
        Cook* cook = nullptr;
        cook = getAvailableCook('Q');
        if (!cook) {
            cook = getAvailableCook('N');
        }

        if (!cook) break;

        Order* quickOrder;
        quickWaitingOrders.dequeue(quickOrder);
        cook->assignOrder(quickOrder, currentTime);
        cout << "  -> Quick Cook " << cook->getId() << " (speed: " << cook->getBaseSpeed() 
             << ") assigned to Express Order " << quickOrder->getID() << endl;
        anyAssigned = true;
    }

    return anyAssigned;
}
//sayed noman
bool Restaurant::assignCateringOrders() {
    bool anyAssigned = false;

    while (!cateringWaitingOrders.isEmpty()) {
        Cook* cook = nullptr;
        cook = getAvailableCook('C');
        if (!cook) break;

        Order* cateringOrder;
        cateringWaitingOrders.dequeue(cateringOrder);
        cook->assignOrder(cateringOrder, currentTime);
        cout << "  -> Catering Cook " << cook->getId() << " (speed: " << cook->getBaseSpeed() 
             << ") assigned to Catering Order " << cateringOrder->getID() << endl;
        anyAssigned = true;
    }

    return anyAssigned;
}

bool Restaurant::attemptPreemption(Order* vipOrder) {
    Cook* victimCook = nullptr;
    int minServiceTime = 999999;
    for (Cook* cook : allCooks) {
        if (cook->getState() == BUSY) {
            Order* currentOrder = cook->getCurrentOrder();
            if (currentOrder && currentOrder->getType() == TYPE_NRM) {
                int serviceTime = currentTime - currentOrder->getAssignTime();
                if (serviceTime < minServiceTime) {
                    minServiceTime = serviceTime;
                    victimCook = cook;
                }
            }
        }
    }
    if (victimCook) {
        Order* preemptedOrder = victimCook->preemptCurrentOrder();
        normalWaitingOrders.enqueue(preemptedOrder);

        cout << "  -> PREEMPTION: Order " << preemptedOrder->getID()
            << " preempted for VIP Order " << vipOrder->getID() << endl;

        victimCook->assignOrder(vipOrder, currentTime);
        return true;
    }

    return false;
}
//sayed noman
void Restaurant::updateCooksAndOrders() {
    cout << "\n--- Update Phase ---" << endl;

    for (Cook* cook : allCooks) {
        if (cook->isInjured()) {
            cook->incrementInjuryTime();
            continue;
        }
        if (cook->getState() == ON_BREAK) {
            cook->updateBreak(currentTime);
            cook->incrementBreakTime();
        }
        else if (cook->isBusy()) {
            cook->cookOneStep();
            cook->incrementBusyTime();

            if (cook->getRemainingDishes() == 0 && cook->getCurrentOrder() != nullptr) {
                Order* finishedOrder = cook->getCurrentOrder();
                cook->finishOrder(currentTime);
                finishedOrder->checkLateness();
                if (finishedOrder->getIsLate()) {
                    cout << "  -> Order " << finishedOrder->getID()
                        << " finished LATE (Deadline: "
                        << finishedOrder->getDeadline() << ")" << endl;
                }
                finishedOrders.InsertEnd(finishedOrder);
            }
        }
        else if (cook->getState() == AVAILABLE) {
            cook->incrementIdleTime();
        }
    }
}

////Mohamed salah
void Restaurant::displayCurrentState() {
    cout << "\n======================================" << endl;
    cout << "Time Step: " << currentTime << endl;
    cout << "======================================" << endl;

    cout << "Waiting Orders:" << endl;
    cout << "  Normal: " << normalWaitingOrders.size() << endl;
    cout << "  Vegan: " << veganWaitingOrders.size() << endl;
    cout << "  VIP: " << vipWaitingOrders.size() << endl;
    cout << "  Express: " << quickWaitingOrders.size() << endl;    
    cout << "  Catering: " << cateringWaitingOrders.size() << endl; 

    cout << "Available Cooks:" << endl;
    cout << "  Normal: " << countAvailableCooks('N') << "/" << numNormalCooks << endl;
    cout << "  Vegan: " << countAvailableCooks('G') << "/" << numVeganCooks << endl;
    cout << "  VIP: " << countAvailableCooks('V') << "/" << numVIPCooks << endl;
    cout << "  Quick: " << countAvailableCooks('Q') << "/" << numQuickCooks << endl;    
    cout << "  Catering: " << countAvailableCooks('C') << "/" << numCateringCooks << endl; 

    cout << "  Busy: " << countBusyCooks() << endl;
    cout << "  On Break: " << countBreakCooks() << endl;

    cout << "Finished Orders: " << finishedOrders.size() << endl;
    cout << "======================================\n" << endl;
}
////Mohamed salah 
void Restaurant::runPhase2Simulation() {
    cout << "\n========================================" << endl;
    cout << "   PHASE 2 SIMULATION - INTERACTIVE    " << endl;
    cout << "========================================\n" << endl;

    cout << "Configuration:" << endl;
    cout << "  Cooks: N=" << numNormalCooks << " G=" << numVeganCooks
        << " V=" << numVIPCooks << endl;
    cout << "  Speeds: N=" << speedNormal << " G=" << speedVegan
        << " V=" << speedVIP << " dishes/timestep" << endl;
    cout << "  Auto-Promote: " << autoPromoteTime << " timesteps" << endl;
    cout << "\nPress Enter to start simulation..." << endl;
    cin.get();

    while (!events.isEmpty() || normalWaitingOrders.size() > 0 ||
        veganWaitingOrders.size() > 0 || vipWaitingOrders.size() > 0 ||
        quickWaitingOrders.size() > 0 || cateringWaitingOrders.size() > 0 ||
        countBusyCooks() > 0) {

        cout << "\n\n";
        cout << "############################################" << endl;
        cout << "          TIMESTEP " << currentTime << endl;
        cout << "############################################" << endl;
        bool hadEvents = false;
        while (!events.isEmpty()) {
            Event* event;
            if (events.peek(event) && event->getTimestamp() == currentTime) {
                events.dequeue(event);
                hadEvents = true;

                if (event->getType() == EVENT_ARRIVAL) {
                    processArrivalEvent(event);
                }
                else if (event->getType() == EVENT_CANCELLATION) {
                    processCancellationEvent(event);
                }
                else if (event->getType() == EVENT_PROMOTION) {
                    processPromotionEvent(event);
                }

                delete event;
            }
            else {
                break;
            }
        }

        if (!hadEvents) {
            cout << "No events at this timestep" << endl;
        }
        checkHealthEmergencies();
        checkForOvertime();
        updateCooksAndOrders();
        checkAutoPromotions();
        assignOrdersPhase2();
        applyFatigueToCooks();
        displayCurrentState();
        cout << "Press Enter to continue..." << endl;
        cin.get();

        currentTime++;
    }
    cout << "\n--- Final Sweep: Clearing remaining orders ---" << endl;
    for (int sweep = 0; sweep < 500; sweep++) {  

        if (normalWaitingOrders.size() == 0 &&
            veganWaitingOrders.size() == 0 &&
            vipWaitingOrders.size() == 0 &&
            quickWaitingOrders.size() == 0 &&
            cateringWaitingOrders.size() == 0 &&
            countBusyCooks() == 0) {
            cout << "All queues cleared after " << sweep << " sweep iterations." << endl;
            break;
        }   
        assignOrdersPhase2();       
        updateCooksAndOrders();     
        checkAutoPromotions();       

        currentTime++;
    }
    cout << endl;
    cout << "SIMULATION COMPLETE" << endl;
    cout << endl;
    cout << "Total timesteps: " << currentTime << endl;
    cout << "\n=== QUEUE STATUS AT SIMULATION END ===" << endl;
    cout << "Normal waiting: " << normalWaitingOrders.size() << endl;
    cout << "Vegan waiting: " << veganWaitingOrders.size() << endl;
    cout << "VIP waiting: " << vipWaitingOrders.size() << endl;
    cout << "Quick waiting: " << quickWaitingOrders.size() << endl;
    cout << "Catering waiting: " << cateringWaitingOrders.size() << endl;
    cout << "Busy cooks: " << countBusyCooks() << endl;
    cout << "Finished orders: " << finishedOrders.size() << endl;
    cout << "===================================\n" << endl;
    generateOutputToConsole();

}

///anas
Order* Restaurant::findOrderToPreempt() {
    Order* bestToPreempt = nullptr;
    int minServiceTime = 999999;
    for (Cook* cook : allCooks) {
        if (cook->getState() == BUSY && cook->getCurrentOrder() != nullptr) {
            Order* order = cook->getCurrentOrder();
            if (order->getType() == TYPE_NRM) {
                int serviceTime = currentTime - order->getAssignTime();
                if (serviceTime < minServiceTime) {
                    minServiceTime = serviceTime;
                    bestToPreempt = order;
                }
            }
        }
    }

    return bestToPreempt;
}
//anas
void Restaurant::preemptOrder(Order* order, Cook* cook) {
    cout << "  -> PREEMPTION: Order " << order->getID()
        << " interrupted, Cook " << cook->getId()
        << " reassigned" << endl;

    // Calculate how many dishes were completed
    int timeWorked = currentTime - order->getAssignTime();
    int dishesCompleted = timeWorked * cook->getSpeed();
    int remaining = order->getSize() - dishesCompleted;

    if (remaining < 0) remaining = 0;

    // Update order with remaining dishes
    order->setDishesCompleted(order->getDishesCompleted() + dishesCompleted);
    order->setSize(remaining);
    cook->setCurrentOrder(nullptr);
    cook->setRemainingDishes(0);
    cook->setStatus(AVAILABLE);
    normalWaitingOrders.enqueue(order);

    cout << "     (Returned " << remaining << " dishes to queue)" << endl;
}

//sayed noman
void Restaurant::checkHealthEmergencies() {
    for (Cook* cook : allCooks) {
        if (!cook->isInjured() && cook->getState() != ON_BREAK) {
            int randomChance = rand() % 100;

            if (randomChance < 1) { 
                int recoveryTime = 3 + (rand() % 5);  
                cook->causeInjury(currentTime, recoveryTime);

                cout << "  -> HEALTH EMERGENCY: Cook " << cook->getType()
                    << cook->getId() << " injured until time "
                    << (currentTime + recoveryTime) << endl;
                if (cook->getCurrentOrder() != nullptr) {
                    Order* order = cook->getCurrentOrder();

                    cout << "     Order " << order->getID()
                        << " returned to waiting queue" << endl;

                    if (order->getType() == TYPE_NRM)
                        normalWaitingOrders.enqueue(order);
                    else if (order->getType() == TYPE_VEG)
                        veganWaitingOrders.enqueue(order);
                    else {
                        float priority = order->calculatePriority(currentTime);
                        vipWaitingOrders.enqueue(order, priority);
                    }
                }
            }
        }
        cook->updateInjuryStatus(currentTime);
    }
}
//sayed noman
void Restaurant::checkForOvertime() {
    if (vipWaitingOrders.size() > 5) {
        cout << "  -> SYSTEM OVERLOAD: Forcing overtime (VIP queue: "
            << vipWaitingOrders.size() << ")" << endl;
        int forced = 0;
        for (Cook* cook : allCooks) {
            if (cook->getType() == 'N' && !cook->shouldSkipBreak() && forced < 2) {
                cook->forceOvertime();
                forced++;
                cout << "     Cook N" << cook->getId()
                    << " will skip next break (speed reduced)" << endl;
            }
        }
    }
}
//sayed noman
void Restaurant::applyFatigueToCooks() {
    for (Cook* cook : allCooks) {
        if (cook->isBusy()) {
            cook->applyFatigue();
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
//sayed noman
Cook* Restaurant::getAvailableCook(char type) {
    Cook* selectedCook = nullptr;
    int minOrders = INT_MAX;
    int maxSpeed = -1;

    for (Cook* cook : allCooks) {
        if (cook->getType() == type &&
            cook->isAvailableForAssignment()) {
            int orders = cook->getTotalOrdersServed();
            int speed = cook->getBaseSpeed();
            if (orders < minOrders ||
                (orders == minOrders && speed > maxSpeed)) {

                minOrders = orders;
                maxSpeed = speed;
                selectedCook = cook;
            }
        }
    }

    return selectedCook;
}


////Mohamed salah
int Restaurant::countAvailableCooks(char type) {
    int count = 0;
    for (Cook* cook : allCooks) {
        if (cook->getType() == type && cook->getState() == AVAILABLE) {
            count++;
        }
    }
    return count;
}
////Mohamed salah
int Restaurant::countBusyCooks() {
    int count = 0;
    for (Cook* cook : allCooks) {
        if (cook->getState() == BUSY) {
            count++;
        }
    }
    return count;
}
////Mohamed salah
int Restaurant::countBreakCooks() {
    int count = 0;
    for (Cook* cook : allCooks) {
        if (cook->getState() == ON_BREAK) {
            count++;
        }
    }
    return count;
}
////Mohamed salah
void Restaurant::generateOutputToConsole() {

    ofstream outFile("output.txt");
    
    if (!outFile.is_open()) {
        cerr << "Warning: Cannot create output file 'output.txt'. Output will only appear in console." << endl;
    }
    auto writeBoth = [&](const string& line) {
        cout << line;
        if (outFile.is_open()) {
            outFile << line;
        }
    };
    vector<Order*> ordersVector;

    Node<Order*>* current = finishedOrders.getHead();
    while (current != nullptr) {
        ordersVector.push_back(current->getData());
        current = current->getNext();
    }
    sort(ordersVector.begin(), ordersVector.end(), [](Order* a, Order* b) {
        if (a->getFinishTime() != b->getFinishTime()) {
            return a->getFinishTime() < b->getFinishTime();
        }
        return a->getST() < b->getST();
        });
    stringstream header;
    header << "FT   ID   AT   WT   ST" << endl;
    writeBoth(header.str());
    for (Order* order : ordersVector) {
        stringstream line;
        line << setw(5) << right << order->getFinishTime()
            << "   "  
            << setw(5) << right << order->getID()
            << "   "  
            << setw(5) << right << order->getAT()
            << "   "  
            << setw(5) << right << order->getWT()
            << "   "  
            << setw(5) << right << order->getST() << endl;
        writeBoth(line.str());
    }

    int totalOrders = ordersVector.size(); 
    int totalCooks = numNormalCooks + numVeganCooks + numVIPCooks + numQuickCooks + numCateringCooks; 
    double avgWT = 0.0, avgST = 0.0;
    for (Order* order : ordersVector) {
        avgWT += order->getWT();
        avgST += order->getST();
    }
    if (totalOrders > 0) {
        avgWT /= totalOrders;
        avgST /= totalOrders;
    }

    int countNormal = 0, countVegan = 0, countVIP = 0, countQuick = 0, countCatering = 0;
    for (Order* order : ordersVector) {
        switch (order->getOriginalType()) {  
        case TYPE_NRM: countNormal++; break;
        case TYPE_VEG: countVegan++; break;
        case TYPE_VIP: countVIP++; break;
        case TYPE_QCK: countQuick++; break;   
        case TYPE_CTR: countCatering++; break; 
        }
    }

    stringstream stats1;
    stats1 << "Orders: " << totalOrders
        << " [Norm:" << countNormal
        << ", Veg:" << countVegan
        << ", VIP:" << countVIP
        << ", Expr:" << countQuick     
        << ", Cater:" << countCatering 
        << "]" << endl;
    writeBoth(stats1.str());

    stringstream stats2;
    stats2 << "Cooks: " << totalCooks
        << " [Norm:" << numNormalCooks
        << ", Veg:" << numVeganCooks
        << ", VIP:" << numVIPCooks
        << ", Quick:" << numQuickCooks        
        << ", Cater:" << numCateringCooks     
        << "]" << endl;
    writeBoth(stats2.str());

    stringstream stats3;
    stats3 << "Avg Wait = " << fixed << setprecision(1) << avgWT
        << ", Avg Serv = " << setprecision(2) << avgST << endl;
    writeBoth(stats3.str());
    int totalNormalOrdersArrived = totalNormalOrders + autoPromotedCount;
    stringstream stats4;
    stats4 << "Auto-promoted: " << autoPromotedCount << endl;
    writeBoth(stats4.str());
    for (Cook* cook : allCooks) {
        int busyTime = cook->getTotalBusyTime();
        int idleTime = cook->getTotalIdleTime();
        int breakTime = cook->getTotalBreakTime();
        int injuryTime = cook->getTotalInjuryTime();
        int breakInjuryTime = breakTime + injuryTime;

        double utilization = 0.0;
        int totalTime = busyTime + idleTime + breakInjuryTime;
        if (totalTime > 0) {
            utilization = (busyTime * 100.0) / totalTime;
        }

        stringstream cookLine;
        cookLine << "Cook " << cook->getType() << cook->getId()
            << ": Orders [Norm:" << cook->getTotalOrdersServedByType(0)
            << ", Veg:" << cook->getTotalOrdersServedByType(1)
            << ", VIP:" << cook->getTotalOrdersServedByType(2)
            << ", Qck:" << cook->getTotalOrdersServedByType(3)  
            << ", Ctr:" << cook->getTotalOrdersServedByType(4)  
            << "], Busy: " << busyTime
            << ", Idle: " << idleTime
            << ", Break/Injury: " << breakInjuryTime
            << ", Utilization: " << fixed << setprecision(1) << utilization << "%" << endl;
        writeBoth(cookLine.str());
    }

    if (outFile.is_open()) {
        outFile.close();
        cout << "\nOutput saved to 'output.txt'" << endl;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
////Mohamed salah
void Restaurant::setGUI(GUI* pGUI) {
    this->pGUI = pGUI;
}
////Mohamed salah
void Restaurant::setMode(PROG_MODE mode) {
    this->mode = mode;
}
////Mohamed salah
void Restaurant::runSimulationWithGUI() {
    pGUI->PrintMessage("Simulation Started... Click to continue");
    pGUI->waitForClick();

    // Loop until all events processed and all queues empty and all cooks free
    while (!events.isEmpty() || normalWaitingOrders.size() > 0 ||
        veganWaitingOrders.size() > 0 || vipWaitingOrders.size() > 0 ||
        quickWaitingOrders.size() > 0 || cateringWaitingOrders.size() > 0 ||  // NEW: Include new queues
        countBusyCooks() > 0) {

        cout << "\n\n";
        cout << "############################################" << endl;
        cout << "          TIMESTEP " << currentTime << endl;
        cout << "############################################" << endl;

        // Process events at current time
        Event* event; 
        while (events.peek(event)) {
             if (event->getTimestamp() == currentTime) {
                events.dequeue(event);
                if (event->getType() == EVENT_ARRIVAL) processArrivalEvent(event);
                else if (event->getType() == EVENT_CANCELLATION) processCancellationEvent(event);
                else if (event->getType() == EVENT_PROMOTION) processPromotionEvent(event);
                delete event;
             } else {
                 break; 
             }
        }

        checkHealthEmergencies();
        checkForOvertime();
        updateCooksAndOrders();
        checkAutoPromotions();
        assignOrdersPhase2();
        applyFatigueToCooks();

        if (mode != MODE_SLNT) {
            pGUI->ResetDrawingList();

            // Add waiting orders (Normal)
            Order* pOrd;
            LinkedQueue<Order*> tempQ;
            while (normalWaitingOrders.dequeue(pOrd)) {
                pGUI->AddToDrawingList(pOrd);
                tempQ.enqueue(pOrd);
            }
            while (tempQ.dequeue(pOrd)) normalWaitingOrders.enqueue(pOrd);

            // Add waiting orders (Vegan)
            while (veganWaitingOrders.dequeue(pOrd)) {
                pGUI->AddToDrawingList(pOrd);
                tempQ.enqueue(pOrd);
            }
            while (tempQ.dequeue(pOrd)) veganWaitingOrders.enqueue(pOrd);

            // Add waiting orders (VIP) - Use copy constructor of PriorityQueue
            PriorityQueue<Order*> tempPQ = vipWaitingOrders;
            while(tempPQ.dequeue(pOrd)) {
                 pGUI->AddToDrawingList(pOrd);
            }

            // NEW: Add waiting orders (Quick/Express)
            while (quickWaitingOrders.dequeue(pOrd)) {
                pGUI->AddToDrawingList(pOrd);
                tempQ.enqueue(pOrd);
            }
            while (tempQ.dequeue(pOrd)) quickWaitingOrders.enqueue(pOrd);

            // NEW: Add waiting orders (Catering)
            while (cateringWaitingOrders.dequeue(pOrd)) {
                pGUI->AddToDrawingList(pOrd);
                tempQ.enqueue(pOrd);
            }
            while (tempQ.dequeue(pOrd)) cateringWaitingOrders.enqueue(pOrd);

            // Add Cooks and their current orders (Service)
            for (Cook* cook : allCooks) {
                pGUI->AddToDrawingList(cook);
                if (cook->getState() == BUSY && cook->getCurrentOrder()) {
                     pGUI->AddToDrawingList(cook->getCurrentOrder());
                }
            }

            // Add Finished Orders
            Node<Order*>* pNode = finishedOrders.getHead();
            while(pNode) {
                 pGUI->AddToDrawingList(pNode->getData());
                 pNode = pNode->getNext();
            }

            pGUI->UpdateInterface();
            
            // Calculate Served Counts for Status Bar (all types)
            int sN=0, sG=0, sV=0, sQ=0, sC=0;
            pNode = finishedOrders.getHead();
            while(pNode) {
                 Order* ord = pNode->getData();
                 if(ord->getType() == TYPE_NRM) sN++;
                 else if(ord->getType() == TYPE_VEG) sG++; 
                 else if(ord->getType() == TYPE_VIP) sV++;
                 else if(ord->getType() == TYPE_QCK) sQ++;  // NEW
                 else if(ord->getType() == TYPE_CTR) sC++;  // NEW
                 pNode = pNode->getNext();
            }

            vector<string> assignList; 

            pGUI->PrintStatusBar(currentTime, 
                normalWaitingOrders.size(), veganWaitingOrders.size(), vipWaitingOrders.size(),
                quickWaitingOrders.size(), cateringWaitingOrders.size(),  // NEW: Quick and Catering waiting
                countAvailableCooks('N'), countAvailableCooks('G'), countAvailableCooks('V'),
                countAvailableCooks('Q'), countAvailableCooks('C'),  // NEW: Quick and Catering available
                 assignList, 
                 sN, sG, sV, sQ, sC);  // NEW: Quick and Catering served

            if (mode == MODE_INTR) pGUI->waitForClick();
            else if (mode == MODE_STEP) pGUI->Sleep(1000);
        }
        
        displayCurrentState(); 
        currentTime++;
    }
    
    pGUI->PrintMessage("Simulation Finished. Click to exit.");
    pGUI->waitForClick();
    generateOutputToConsole();
}
