#include "Cook.h"
#include "Order.h"
#include <iostream>
#include <algorithm>
using namespace std;

void Cook::assignOrder(Order* order, int assignTime) {
    if (currentOrder != nullptr) {
        cout << "Warning: Cook " << id << " already has an order!" << endl;
        return;
    }

    currentOrder = order;
    remainingDishes = order->getSize();
    order->setAssignTime(assignTime);
    currentState = BUSY;

    cout << "  -> Cook " << type << id << " assigned to Order "
        << order->getID() << " (" << order->getTypeString() << ")" << endl;
}

void Cook::cookOneStep() {
    if (currentOrder == nullptr) return;

    // Cook dishes based on current speed
    int dishesCooked = std::min<int>(currentSpeed, remainingDishes);
    remainingDishes -= dishesCooked;
    totalDishesCooked += dishesCooked;

    // Update order's completed dishes
    currentOrder->setDishesCompleted(currentOrder->getDishesCompleted() + dishesCooked);
}

bool Cook::finishOrder(int finishTime) {
    // Check if cook has an order to finish
    if (currentOrder == nullptr) return false;
    if (remainingDishes > 0) return false;

    // Set finish time (this is all you need!)
    currentOrder->setFinishTime(finishTime);

    // Count by CURRENT type (for per-cook statistics)
    OrderType orderType = currentOrder->getType();
    if (orderType == TYPE_NRM) totalOrdersServedByType[0]++;
    else if (orderType == TYPE_VEG) totalOrdersServedByType[1]++;
    else if (orderType == TYPE_VIP) totalOrdersServedByType[2]++;

    totalOrdersServed++;
    ordersServedStreak++;

    // ? Get the calculated values for display
    int WT = currentOrder->getWT();  // Calculated automatically!
    int ST = currentOrder->getST();  // Calculated automatically!

    cout << "  -> Cook " << type << id << " completed Order "
        << currentOrder->getID() << " at time " << finishTime
        << " (WT=" << WT << ", ST=" << ST << ")" << endl;

    // Clear current order AFTER all calculations are done
    currentOrder = nullptr;
    remainingDishes = 0;

    // Check if cook needs a break
    if (ordersServedStreak >= breakAfter) {
        startBreak(finishTime);
    }
    else {
        currentState = AVAILABLE;
    }

    return true;
}




void Cook::startBreak(int currentTime) {
    // ? CHECK IF BREAK SHOULD BE SKIPPED (OVERTIME)
    if (shouldSkipNextBreak) {
        cout << "  -> Cook " << type << id << " skipping break (OVERTIME)" << endl;
        resetBreakSkip();
        ordersServedStreak = 0;
        currentState = AVAILABLE;
        return;
    }

    // ? CHECK IF LONGER BREAK NEEDED
    int breakTime = breakDuration;
    if (needsLongerBreak()) {
        breakTime = getLongerBreakDuration();
        cout << "  -> Cook " << type << id << " taking EXTENDED break (duration: "
            << breakTime << ")" << endl;
        breaksSkipped = 0;
    }

    breakEndTime = currentTime + breakTime;
    currentState = ON_BREAK;

    // ? RESTORE SPEED ON BREAK
    restoreSpeed();

    cout << "  -> Cook " << type << id << " starting break until time "
        << breakEndTime << endl;
}

void Cook::updateBreak(int currentTime) {
    if (isOnBreak()) {
        if (currentTime >= breakEndTime) {
            cout << "  -> Cook " << type << id << " returning from break" << endl;
            endBreak();
        }
    }
}

void Cook::endBreak() {
    breakEndTime = -1;
    ordersServedStreak = 0;

    // ? RESTORE SPEED WHEN BREAK ENDS
    restoreSpeed();

    currentState = AVAILABLE;
}

Order* Cook::preemptCurrentOrder() {
    if (currentOrder == nullptr) return nullptr;

    Order* preemptedOrder = currentOrder;

    // Update the order's size to reflect remaining dishes
    preemptedOrder->setSize(remainingDishes);

    cout << "  -> Cook " << type << id << " preempted Order "
        << preemptedOrder->getID() << " (remaining: " << remainingDishes << " dishes)" << endl;

    currentOrder = nullptr;
    remainingDishes = 0;

    return preemptedOrder;
}

// ? THAT'S IT - NO MORE FUNCTIONS NEEDED!
// The other functions are already in Cook.h
