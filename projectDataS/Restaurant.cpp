#include "Restaurant.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

Restaurant::Restaurant() : currentTime(0), eventCount(0) {
}

Restaurant::~Restaurant() {
    Cook* cook;
    while (availableNormalCooks.dequeue(cook)) delete cook;
    while (availableVeganCooks.dequeue(cook)) delete cook;
    while (availableVIPCooks.dequeue(cook)) delete cook;

    Order* order;
    while (normalWaitingOrders.dequeue(order)) delete order;
    while (veganWaitingOrders.dequeue(order)) delete order;
    while (vipWaitingOrders.dequeue(order)) delete order;

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



void Restaurant::processCancellationEvent(Event* event) {
    cout << "  Target Order ID: " << event->getTargetID() << endl;
    cout << "  Status: Order cancellation requested" << endl;
    cout << "  Note: Full cancellation logic in Phase 2" << endl;
}

void Restaurant::processPromotionEvent(Event* event) {
    cout << "  Target Order ID: " << event->getTargetID() << endl;
    cout << "  Extra Money: $" << event->getExtraMoney() << endl;
    cout << "  Status: Order promotion requested" << endl;
    cout << "  Note: Full promotion logic in Phase 2" << endl;
}

void Restaurant::displayCurrentState() const {
    cout << "\n--- Current State (Time=" << currentTime << ") ---" << endl;

    // Calculate totals
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

    // Display loaded parameters
    cout << "\nLoaded Parameters:" << endl;
    cout << "Cooks: N=" << numNormalCooks << ", G=" << numVeganCooks
        << ", V=" << numVIPCooks << endl;
    cout << "Speeds: N=" << speedNormal << ", G=" << speedVegan
        << ", V=" << speedVIP << " dishes/timestep" << endl;
    cout << "Auto-Promote Time: " << autoPromoteTime << " timesteps" << endl;

    cout << "\nPress Enter to start simulation...";
    cin.get();

    // Main simulation loop
    while (!events.isEmpty()) {
        cout << "\n========================================" << endl;
        cout << "         Timestep " << currentTime << endl;
        cout << "========================================" << endl;

        // Track if any events happened
        bool hadEvents = false;

        // Process all events at current timestep
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
                break;  // Future event, stop processing
            }
        }

        // Show message if no events
        if (!hadEvents) {
            cout << "\n[No events at this timestep]" << endl;
        }

        // Display current state
        displayCurrentState();

        // Wait for user
        cout << "\nPress Enter to continue...";
        cin.get();

        currentTime++;
    }

    // Simulation complete
    cout << "\n========================================" << endl;
    cout << "      SIMULATION COMPLETE" << endl;
    cout << "========================================" << endl;
    cout << "\nTotal timesteps simulated: " << currentTime << endl;
    cout << "All events processed successfully!" << endl;
}
