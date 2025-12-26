#include "Restaurant.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>     // ? For setprecision(), fixed

using namespace std;

Restaurant::Restaurant() {
    currentTime = 0;
    eventCount = 0;
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
    totalNormalOrders = 0;
    totalVeganOrders = 0;
    totalVIPOrders = 0;
    autoPromotedCount = 0;
}

Restaurant::~Restaurant() {
    // Clean up all allocated cooks
    for (Cook* cook : allCooks) {
        delete cook;
    }

    // Clean up remaining orders
    Order* order;
    while (normalWaitingOrders.dequeue(order)) delete order;
    while (veganWaitingOrders.dequeue(order)) delete order;
    while (vipWaitingOrders.dequeue(order)) delete order;

    finishedOrders.DeleteAll();

    Event* event;
    while (events.dequeue(event)) delete event;
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

void Restaurant::createCooks() {
    int cookID = 1;

    // Create Normal cooks
    for (int i = 0; i < numNormalCooks; i++) {
        Cook* cook = new Cook('N', speedNormal, BO, BN, cookID++);
        allCooks.push_back(cook);
    }

    // Create Vegan cooks
    for (int i = 0; i < numVeganCooks; i++) {
        Cook* cook = new Cook('G', speedVegan, BO, BG, cookID++);
        allCooks.push_back(cook);
    }

    // Create VIP cooks
    for (int i = 0; i < numVIPCooks; i++) {
        Cook* cook = new Cook('V', speedVIP, BO, BV, cookID++);
        allCooks.push_back(cook);
    }
}

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
    else {
        type = TYPE_VIP;
        totalVIPOrders++;
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
    else {
        float priority = order->calculatePriority(currentTime);
        vipWaitingOrders.enqueue(order, priority);
    }
}

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
            // Promote to VIP
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

void Restaurant::checkAutoPromotions() {
    if (autoPromoteTime <= 0) return;

    LinkedQueue<Order*> tempQueue;
    Order* order;

    while (normalWaitingOrders.dequeue(order)) {
        int waitingTime = currentTime - order->getAT();

        if (waitingTime >= autoPromoteTime) {
            // Auto-promote to VIP
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

void Restaurant::assignOrdersPhase2() {
    cout << "\n--- Assignment Phase ---" << endl;

    // Priority 1: Assign VIP orders
    if (assignVIPOrders()) {
        cout << "  VIP orders assigned" << endl;
    }

    // Priority 2: Assign Vegan orders
    if (assignVeganOrders()) {
        cout << "  Vegan orders assigned" << endl;
    }

    // Priority 3: Assign Normal orders
    if (assignNormalOrders()) {
        cout << "  Normal orders assigned" << endl;
    }
}

bool Restaurant::assignVIPOrders() {
    bool anyAssigned = false;

    while (!vipWaitingOrders.isEmpty()) {
        Order* vipOrder;
        if (!vipWaitingOrders.peek(vipOrder)) break;

        Cook* cook = nullptr;

        // Try VIP cooks first
        cook = getAvailableCook('V');

        // Then Normal cooks
        if (!cook) {
            cook = getAvailableCook('N');
        }

        // Finally Vegan cooks
        if (!cook) {
            cook = getAvailableCook('G');
        }

        // ? IF NO COOK AVAILABLE, TRY PREEMPTION:
        if (!cook) {
            Order* toPreempt = findOrderToPreempt();

            if (toPreempt != nullptr) {
                // Find the cook working on this order
                Cook* freedCook = nullptr;
                for (Cook* c : allCooks) {
                    if (c->getCurrentOrder() == toPreempt) {
                        freedCook = c;
                        break;
                    }
                }

                if (freedCook != nullptr) {
                    preemptOrder(toPreempt, freedCook);

                    // Now assign VIP order to freed cook
                    vipWaitingOrders.dequeue(vipOrder);
                    freedCook->assignOrder(vipOrder, currentTime);

                    cout << "  -> Cook " << freedCook->getType() << freedCook->getId()
                        << " assigned to VIP Order " << vipOrder->getID()
                        << " (after preemption)" << endl;

                    anyAssigned = true;
                    continue;
                }
            }

            // No preemption possible, VIP waits
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

bool Restaurant::assignNormalOrders() {
    bool anyAssigned = false;

    while (!normalWaitingOrders.isEmpty()) {
        Cook* cook = nullptr;

        // Try Normal cooks first
        cook = getAvailableCook('N');

        // Then VIP cooks
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

bool Restaurant::attemptPreemption(Order* vipOrder) {
    Cook* victimCook = nullptr;
    int minServiceTime = 999999;

    // Search through all busy cooks for Normal orders
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

    // Perform preemption if victim found
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

void Restaurant::updateCooksAndOrders() {
    cout << "\n--- Update Phase ---" << endl;

    for (Cook* cook : allCooks) {
        // Handle injured cooks
        if (cook->isInjured()) {
            cook->incrementInjuryTime();
            continue;
        }

        // Handle cooks on break
        if (cook->getState() == ON_BREAK) {
            cook->updateBreak(currentTime);
            cook->incrementBreakTime();
        }
        // Handle busy cooks
        else if (cook->isBusy()) {
            cook->cookOneStep();
            cook->incrementBusyTime();

            // ? ONLY finish if ALL dishes are done
            if (cook->getRemainingDishes() == 0 && cook->getCurrentOrder() != nullptr) {
                Order* finishedOrder = cook->getCurrentOrder();

                // ? Finish the order (this clears cook's currentOrder)
                cook->finishOrder(currentTime);

                // Check if order is late
                finishedOrder->checkLateness();
                if (finishedOrder->getIsLate()) {
                    cout << "  -> Order " << finishedOrder->getID()
                        << " finished LATE (Deadline: "
                        << finishedOrder->getDeadline() << ")" << endl;
                }

                // ? Add to finished orders (ONLY once, when truly finished)
                finishedOrders.InsertEnd(finishedOrder);
            }
        }
        // Handle available cooks (track idle time)
        else if (cook->getState() == AVAILABLE) {
            cook->incrementIdleTime();
        }
    }
}


void Restaurant::displayCurrentState() {
    cout << "\n======================================" << endl;
    cout << "Time Step: " << currentTime << endl;
    cout << "======================================" << endl;

    cout << "Waiting Orders:" << endl;
    cout << "  Normal: " << normalWaitingOrders.size() << endl;
    cout << "  Vegan: " << veganWaitingOrders.size() << endl;
    cout << "  VIP: " << vipWaitingOrders.size() << endl;

    cout << "Available Cooks:" << endl;
    cout << "  Normal: " << countAvailableCooks('N') << "/" << numNormalCooks << endl;
    cout << "  Vegan: " << countAvailableCooks('G') << "/" << numVeganCooks << endl;
    cout << "  VIP: " << countAvailableCooks('V') << "/" << numVIPCooks << endl;

    cout << "  Busy: " << countBusyCooks() << endl;
    cout << "  On Break: " << countBreakCooks() << endl;

    cout << "Finished Orders: " << finishedOrders.size() << endl;
    cout << "======================================\n" << endl;
}

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
        countBusyCooks() > 0) {

        cout << "\n\n";
        cout << "############################################" << endl;
        cout << "          TIMESTEP " << currentTime << endl;
        cout << "############################################" << endl;

        // Process events at current time
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

        // ? 1. CHECK FOR HEALTH EMERGENCIES (at start of timestep)
        checkHealthEmergencies();

        // ? 2. CHECK FOR OVERTIME (before assignments)
        checkForOvertime();

        // 3. Update all cooks and finish completed orders
        updateCooksAndOrders();

        // 4. Check for auto-promotions
        checkAutoPromotions();

        // 5. Assign waiting orders to available cooks
        assignOrdersPhase2();

        // ? 6. APPLY FATIGUE TO BUSY COOKS
        applyFatigueToCooks();

        // 7. Display current state
        displayCurrentState();

        cout << "Press Enter to continue..." << endl;
        cin.get();

        currentTime++;
    }

    cout << "\n========================================" << endl;
    cout << "      SIMULATION COMPLETE              " << endl;
    cout << "========================================" << endl;
    cout << "Total timesteps: " << currentTime << endl;

    // Generate output file
    generateOutputFile("output.txt");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ? STEP 3: FIND NORMAL ORDER TO PREEMPT
Order* Restaurant::findOrderToPreempt() {
    Order* bestToPreempt = nullptr;
    int minServiceTime = 999999;

    // Search through all busy cooks for Normal orders
    for (Cook* cook : allCooks) {
        if (cook->getState() == BUSY && cook->getCurrentOrder() != nullptr) {
            Order* order = cook->getCurrentOrder();

            // Only preempt Normal orders
            if (order->getType() == TYPE_NRM) {
                int serviceTime = currentTime - order->getAssignTime();

                // Choose order with LEAST service time
                if (serviceTime < minServiceTime) {
                    minServiceTime = serviceTime;
                    bestToPreempt = order;
                }
            }
        }
    }

    return bestToPreempt;
}

// ? STEP 3: PREEMPT ORDER (This replaces your attemptPreemption)
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

    // ? CRITICAL: Manually clear the cook (DON'T call finishOrder!)
    cook->setCurrentOrder(nullptr);
    cook->setRemainingDishes(0);
    cook->setStatus(AVAILABLE);

    // Return to Normal waiting queue
    normalWaitingOrders.enqueue(order);

    cout << "     (Returned " << remaining << " dishes to queue)" << endl;
}


// ? STEP 4: CHECK FOR HEALTH EMERGENCIES
void Restaurant::checkHealthEmergencies() {
    // 1% chance per cook per timestep
    for (Cook* cook : allCooks) {
        if (!cook->isInjured() && cook->getState() != ON_BREAK) {
            int randomChance = rand() % 100;

            if (randomChance < 1) {  // 1% chance
                int recoveryTime = 3 + (rand() % 5);  // 3-7 timesteps
                cook->causeInjury(currentTime, recoveryTime);

                cout << "  -> HEALTH EMERGENCY: Cook " << cook->getType()
                    << cook->getId() << " injured until time "
                    << (currentTime + recoveryTime) << endl;

                // If cook was working, return order to queue
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

        // Update injury status each timestep
        cook->updateInjuryStatus(currentTime);
    }
}

// ? STEP 5: CHECK FOR OVERTIME
void Restaurant::checkForOvertime() {
    // If VIP queue has > 5 orders, force some cooks into overtime
    if (vipWaitingOrders.size() > 5) {
        cout << "  -> SYSTEM OVERLOAD: Forcing overtime (VIP queue: "
            << vipWaitingOrders.size() << ")" << endl;

        // Force 2 Normal cooks to skip their next break
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

// ? STEP 7: APPLY FATIGUE TO BUSY COOKS
void Restaurant::applyFatigueToCooks() {
    for (Cook* cook : allCooks) {
        // Apply fatigue to busy cooks
        if (cook->isBusy()) {
            cook->applyFatigue();
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////


// ===============================================================================================
// Helper Functions
// ===============================================================================================

Cook* Restaurant::getAvailableCook(char type) {
    for (Cook* cook : allCooks) {
        if (cook->getType() == type && cook->isAvailableForAssignment()) {
            return cook;
        }
    }
    return nullptr;
}

int Restaurant::countAvailableCooks(char type) {
    int count = 0;
    for (Cook* cook : allCooks) {
        if (cook->getType() == type && cook->getState() == AVAILABLE) {
            count++;
        }
    }
    return count;
}

int Restaurant::countBusyCooks() {
    int count = 0;
    for (Cook* cook : allCooks) {
        if (cook->getState() == BUSY) {
            count++;
        }
    }
    return count;
}

int Restaurant::countBreakCooks() {
    int count = 0;
    for (Cook* cook : allCooks) {
        if (cook->getState() == ON_BREAK) {
            count++;
        }
    }
    return count;
}
////////////////////============================================================================================



void Restaurant::generateOutputFile(const string& filename) {
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        cerr << "Error: Cannot create output file " << filename << endl;
        return;
    }

    cout << "\nGenerating output file..." << endl;

    // ? EXTRACT ORDERS FROM LINKEDLIST TO VECTOR
    vector<Order*> ordersVector;

    // Method 1: Using getHead() to traverse the linked list
    Node<Order*>* current = finishedOrders.getHead();
    while (current != nullptr) {
        ordersVector.push_back(current->getData());
        current = current->getNext();
    }

    cout << "Extracted " << ordersVector.size() << " finished orders" << endl;

    // Sort orders by FT (Finish Time), then by ST (Service Time)
    sort(ordersVector.begin(), ordersVector.end(), [](Order* a, Order* b) {
        if (a->getFinishTime() != b->getFinishTime()) {
            return a->getFinishTime() < b->getFinishTime();
        }
        return a->getST() < b->getST();
        });

    // ? WRITE ORDER DETAILS (FT ID AT WT ST)
    for (Order* order : ordersVector) {
        outFile << order->getFinishTime() << " "
            << order->getID() << " "
            << order->getAT() << " "
            << order->getWT() << " "
            << order->getST() << endl;
    }

    // ? CALCULATE STATISTICS FROM ACTUAL FINISHED ORDERS
    int totalOrders = ordersVector.size();  // Use actual finished orders count
    int totalCooks = numNormalCooks + numVeganCooks + numVIPCooks;

    // Calculate average waiting time and service time
    double avgWT = 0.0, avgST = 0.0;
    for (Order* order : ordersVector) {
        avgWT += order->getWT();
        avgST += order->getST();
    }
    if (totalOrders > 0) {
        avgWT /= totalOrders;
        avgST /= totalOrders;
    }

    // ? COUNT ORDERS BY ORIGINAL TYPE (prevents double-counting promoted orders)
    int countNormal = 0, countVegan = 0, countVIP = 0;
    for (Order* order : ordersVector) {
        switch (order->getOriginalType()) {  // ? Use ORIGINAL type!
        case TYPE_NRM: countNormal++; break;
        case TYPE_VEG: countVegan++; break;
        case TYPE_VIP: countVIP++; break;
        }
    }

    // ? WRITE OVERALL STATISTICS
    outFile << "\nOrders: " << totalOrders
        << " [Norm:" << countNormal      // ? Use counted values!
        << ", Veg:" << countVegan
        << ", VIP:" << countVIP << "]" << endl;

    outFile << "Cooks: " << totalCooks
        << " [Norm:" << numNormalCooks
        << ", Veg:" << numVeganCooks
        << ", VIP:" << numVIPCooks << "]" << endl;

    outFile << "Avg Wait: " << fixed << setprecision(2) << avgWT
        << ", Avg Serv: " << avgST << endl;

    outFile << "Auto-promoted: " << autoPromotedCount << endl;

    // ??? ADD LATE ORDERS TRACKING ???
    int lateOrdersCount = 0;
    for (Order* order : ordersVector) {
        if (order->getIsLate()) {
            lateOrdersCount++;
        }
    }

    double latePercentage = 0.0;
    if (totalOrders > 0) {
        latePercentage = (lateOrdersCount * 100.0) / totalOrders;
    }

    outFile << "Late Orders: " << lateOrdersCount
        << " (" << fixed << setprecision(1) << latePercentage << "%)" << endl;
    // ??? END OF LATE ORDERS TRACKING ???

    // ? WRITE PER-COOK STATISTICS
    for (Cook* cook : allCooks) {
        outFile << "\nCook " << cook->getType() << cook->getId()
            << " [Orders: Norm:" << cook->getTotalOrdersServedByType(0)
            << ", Veg:" << cook->getTotalOrdersServedByType(1)
            << ", VIP:" << cook->getTotalOrdersServedByType(2)
            << ", Busy:" << cook->getTotalBusyTime()
            << ", Idle:" << cook->getTotalIdleTime()
            << ", Break:" << cook->getTotalBreakTime()
            << ", Util:" << fixed << setprecision(4) << cook->getUtilization(currentTime) << "%]" << endl;
    }

    outFile.close();
    cout << "Output file '" << filename << "' created successfully!" << endl;
    cout << "Total orders written: " << totalOrders << endl;
}
