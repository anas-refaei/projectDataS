
#ifndef COOK_H
#define COOK_H
#pragma once
#include <iostream>
using namespace std;

class Order;  // Forward declaration

class Cook
{
    char type;              // 'V' = VIP, 'G' = Vegan, 'N' = Normal
    int speed;              // Dishes per time step
    int breakAfter;         // Break after serving this many consecutive orders
    int ordersServedStreak; // Current streak without break
    int breakEndTime;       // Time when current break ends (-1 if not on break)

    Order* currentOrder;    // nullptr if idle
    int remainingDishes;    // Dishes left in current order

    // Statistics
    int totalOrdersServed;
    int totalDishesCooked;
    int totalBusyTime;
    int totalBreakTime;

    int id;                 // Unique ID (for printing and debugging)

public:
    // Constructor
    Cook(char t, int s, int breakAfterN, int cookId)
        : type(t), speed(s), breakAfter(breakAfterN), id(cookId),
        ordersServedStreak(0), breakEndTime(-1), currentOrder(nullptr), remainingDishes(0),
        totalOrdersServed(0), totalDishesCooked(0), totalBusyTime(0), totalBreakTime(0)
    {
    }

    // ==================== Getters ====================
    char    getType() const { return type; }
    int     getSpeed() const { return speed; }
    int     getBreakAfter() const { return breakAfter; }
    int     getOrdersServedStreak() const { return ordersServedStreak; }
    int     getBreakEndTime() const { return breakEndTime; }
    int     getId() const { return id; }

    Order* getCurrentOrder() const { return currentOrder; }
    int     getRemainingDishes() const { return remainingDishes; }

    bool    isBusy() const { return currentOrder != nullptr; }
    bool    isOnBreak() const { return breakEndTime != -1; }
    bool    isAvailable(int currentTime) const {
        return (currentOrder == nullptr) && (breakEndTime == -1 || currentTime >= breakEndTime);
    }

    // Phase 2 Statistics
    int     getTotalOrdersServed() const { return totalOrdersServed; }
    int     getTotalDishesCooked() const { return totalDishesCooked; }
    int     getTotalBusyTime() const { return totalBusyTime; }
    int     getTotalBreakTime() const { return totalBreakTime; }
    double  getUtilization(int totalTime) const {
        return totalTime > 0 ? (double)totalBusyTime / totalTime : 0.0;
    }

    // ==================== Setters & Actions ====================
    void    assignOrder(Order* order, int assignTime);
    void    cookOneStep();
    void    finishOrder(int finishTime);
    void    startBreak(int currentTime);
    void    endBreak() { breakEndTime = -1; ordersServedStreak = 0; }
    void    update(int currentTime);
    void    incrementBusyTime() { totalBusyTime++; }
    void    incrementBreakTime() { totalBreakTime++; }
    void    incrementOrdersStreak() { ordersServedStreak++; }
    void    resetStreak() { ordersServedStreak = 0; }
    void    reset();  // Reset all stats
    // For debugging / GUI
    void    print() const {
        cout << "Cook " << id << " (" << type << ") Speed:" << speed
            << " Busy:" << (isBusy() ? "Yes" : "No")
            << " OnBreak:" << (isOnBreak() ? "Yes" : "No")
            << " OrdersServed:" << totalOrdersServed << endl;
    }
};
#endif