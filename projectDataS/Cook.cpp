#include "Cook.h"
#include "Order.h"
#include <iostream>
using namespace std;

void Cook::assignOrder(Order* order, int assignTime) {
    if (currentOrder != nullptr) {
        cout << "Warning: Cook already has an order!" << endl;
        return;
    }

    currentOrder = order;
    remainingDishes = order->getSize();
    order->setAssignTime(assignTime);
}

void Cook::cookOneStep() {
    if (currentOrder == nullptr) return;

    int dishesCooked = min(speed, remainingDishes);
    remainingDishes -= dishesCooked;
    totalDishesCooked += dishesCooked;
}

void Cook::finishOrder(int finishTime) {
    if (currentOrder == nullptr) return;

    currentOrder->setFinishTime(finishTime);
    currentOrder = nullptr;
    remainingDishes = 0;
    totalOrdersServed++;
    ordersServedStreak++;
}

void Cook::startBreak(int currentTime) {
    breakEndTime = currentTime + breakAfter;
}

void Cook::update(int currentTime) {
    // Update break status
    if (isOnBreak()) {
        if (currentTime >= breakEndTime) {
            endBreak();
        }
        else {
            incrementBreakTime();
        }
        return;
    }

    // Update cooking
    if (isBusy()) {
        cookOneStep();
        incrementBusyTime();

        if (remainingDishes == 0) {
            finishOrder(currentTime);
        }
    }
}

void Cook::reset() {
    currentOrder = nullptr;
    remainingDishes = 0;
    ordersServedStreak = 0;
    breakEndTime = -1;
    totalOrdersServed = 0;
    totalDishesCooked = 0;
    totalBusyTime = 0;
    totalBreakTime = 0;
}
