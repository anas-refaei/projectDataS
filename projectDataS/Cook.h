#ifndef COOK_H
#define COOK_H

#include <iostream>
using namespace std;

class Order; // Forward declaration

// Enum for cook states
enum CookState {
    AVAILABLE,
    BUSY,
    ON_BREAK,
    INJURED  // ? NEW: For health emergencies
};

class Cook {
private:
    char type; // 'V' VIP, 'G' Vegan, 'N' Normal
    int id; // Unique ID for printing and debugging

    // ? SPEED AND FATIGUE SYSTEM
    int baseSpeed;           // Base dishes per time step (from input file)
    double currentSpeed;     // ? CHANGED TO DOUBLE for fatigue calculations

    // Break management
    int breakAfter;          // Break after serving this many consecutive orders
    int ordersServedStreak;  // Current streak without break
    int breakDuration;       // How long breaks last
    int breakEndTime;        // Time when current break ends (-1 if not on break)

    // ? HEALTH EMERGENCY SYSTEM
    bool injured;            // Is cook currently injured?
    int injuryEndTime;       // Time when injury recovery ends (-1 if not injured)
    int injuryRecoveryTime;  // Duration of current injury

    // ? OVERTIME SYSTEM
    bool shouldSkipNextBreak; // Should skip next scheduled break due to overload
    int breaksSkipped;       // Count of breaks skipped (for statistics)

    // Current assignment
    Order* currentOrder;     // nullptr if idle
    int remainingDishes;     // Dishes left in current order
    CookState currentState;  // Current state of the cook

    // Statistics for Phase 2
    int totalOrdersServed;
    int totalOrdersServedByType[3]; // [0]=Normal, [1]=Vegan, [2]=VIP
    int totalDishesCooked;
    int totalBusyTime;
    int totalIdleTime;
    int totalBreakTime;
    int totalInjuryTime;     // ? NEW: Track injury time

public:
    // Constructor
    Cook(char t, int s, int breakAfterN, int breakDur, int cookId)
        : type(t), baseSpeed(s), currentSpeed((double)s), // ? Initialize as double
        breakAfter(breakAfterN), breakDuration(breakDur), id(cookId),
        ordersServedStreak(0), breakEndTime(-1),
        injured(false), injuryEndTime(-1), injuryRecoveryTime(0), // ? Initialize injury vars
        shouldSkipNextBreak(false), breaksSkipped(0),              // ? Initialize overtime vars
        currentOrder(nullptr), remainingDishes(0),
        currentState(AVAILABLE),
        totalOrdersServed(0), totalDishesCooked(0),
        totalBusyTime(0), totalIdleTime(0), totalBreakTime(0),
        totalInjuryTime(0)                                        // ? Initialize injury time
    {
        totalOrdersServedByType[0] = 0;
        totalOrdersServedByType[1] = 0;
        totalOrdersServedByType[2] = 0;
    }

    // Basic Getters
    char getType() const { return type; }
    int getSpeed() const { return (int)(currentSpeed + 0.5); } // ? Round to nearest int
    int getCurrentSpeedInt() const { return (int)(currentSpeed + 0.5); }
    double getCurrentSpeedDouble() const { return currentSpeed; }
    int getBaseSpeed() const { return baseSpeed; }
    int getBreakAfter() const { return breakAfter; }
    int getOrdersServedStreak() const { return ordersServedStreak; }
    int getBreakEndTime() const { return breakEndTime; }
    int getId() const { return id; }
    Order* getCurrentOrder() const { return currentOrder; }
    int getRemainingDishes() const { return remainingDishes; }

    // ? INJURY GETTERS
    bool isInjured() const { return injured; }
    int getInjuryEndTime() const { return injuryEndTime; }

    // ? OVERTIME GETTERS
    bool shouldSkipBreak() const { return shouldSkipNextBreak; }
    int getBreaksSkipped() const { return breaksSkipped; }

    // State Getters
    bool isBusy() const { return currentOrder != nullptr; }
    bool isOnBreak() const { return breakEndTime != -1; }

    // ? UPDATED: Check if available (not busy, not on break, not injured)
    bool isAvailable(int currentTime) const {
        return currentOrder == nullptr &&
            (breakEndTime == -1 || currentTime >= breakEndTime) &&
            !injured;
    }

    // State Management
    CookState getState() const { return currentState; }
    void setState(CookState state) { currentState = state; }

    // ??? ADDED: MISSING SETTERS REQUIRED BY RESTAURANT.CPP ???
    void setCurrentOrder(Order* order) { currentOrder = order; }
    void setRemainingDishes(int dishes) { remainingDishes = dishes; }
    void setStatus(CookState status) { currentState = status; }  // Alias for setState
    // ??? END OF ADDED SETTERS ???

    bool isAvailableForAssignment() const {
        return currentState == AVAILABLE && currentOrder == nullptr && !injured;
    }

    // Phase 2 Statistics Getters
    int getTotalOrdersServed() const { return totalOrdersServed; }
    int getTotalOrdersServedByType(int typeIndex) const {
        return totalOrdersServedByType[typeIndex];
    }
    int getTotalDishesCooked() const { return totalDishesCooked; }
    int getTotalBusyTime() const { return totalBusyTime; }
    int getTotalIdleTime() const { return totalIdleTime; }
    int getTotalBreakTime() const { return totalBreakTime; }
    int getTotalInjuryTime() const { return totalInjuryTime; } // ? NEW

    // ? UPDATED UTILIZATION: Include injury time
    double getUtilization(int totalTime) const {
        if (totalTime == 0) return 0.0;
        int activeTime = totalBusyTime + totalIdleTime;
        if (activeTime == 0) return 0.0;
        return (totalBusyTime * 100.0) / activeTime;
    }

    // Statistics Incrementers
    void incrementBusyTime() { totalBusyTime++; }
    void incrementIdleTime() { totalIdleTime++; }
    void incrementBreakTime() { totalBreakTime++; }
    void incrementInjuryTime() { totalInjuryTime++; } // ? NEW

    // ? FATIGUE SYSTEM
    void applyFatigue() {
        // Reduce speed by 2% after each order
        currentSpeed *= 0.98;

        // Don't go below 50% of base speed
        double minSpeed = baseSpeed * 0.5;
        if (currentSpeed < minSpeed) {
            currentSpeed = minSpeed;
        }
    }

    void restoreSpeed() {
        // Restore to full base speed (after break)
        currentSpeed = (double)baseSpeed;
    }

    // ? HEALTH EMERGENCY SYSTEM
    void causeInjury(int currentTime, int recoveryDuration) {
        injured = true;
        injuryRecoveryTime = recoveryDuration;
        injuryEndTime = currentTime + recoveryDuration;
        currentState = INJURED;

        // If working, interrupt the order
        if (currentOrder != nullptr) {
            // Order will be returned to queue by Restaurant
            currentOrder = nullptr;
            remainingDishes = 0;
        }
    }

    void updateInjuryStatus(int currentTime) {
        if (injured && currentTime >= injuryEndTime) {
            // Recovery complete
            injured = false;
            injuryEndTime = -1;
            currentState = AVAILABLE;

            // Restore full speed after recovery
            restoreSpeed();
        }
    }

    void forceWorkWhileInjured() {
        // Working while injured: speed reduced to 50% of base
        currentSpeed = baseSpeed * 0.5;
    }

    // ? OVERTIME SYSTEM
    void forceOvertime() {
        shouldSkipNextBreak = true;
        breaksSkipped++;

        // Apply extra fatigue penalty for overtime
        currentSpeed *= 0.90; // 10% speed reduction

        // Minimum speed: 40% of base
        double minSpeed = baseSpeed * 0.4;
        if (currentSpeed < minSpeed) {
            currentSpeed = minSpeed;
        }
    }

    void resetBreakSkip() {
        shouldSkipNextBreak = false;
    }

    bool needsLongerBreak() const {
        // After skipping 2+ breaks, need longer break
        return breaksSkipped >= 2;
    }

    int getLongerBreakDuration() const {
        // 1.5x normal break duration
        return (int)(breakDuration * 1.5);
    }

    // Core Action Methods (implementations in Cook.cpp)
    void assignOrder(Order* order, int assignTime);
    void cookOneStep();
    bool finishOrder(int finishTime);
    void startBreak(int currentTime);
    void updateBreak(int currentTime);
    void endBreak();
    Order* preemptCurrentOrder();

    // For debugging/GUI
    void print() const {
        cout << "Cook " << id << " [" << type << "] "
            << "Speed:" << getCurrentSpeedInt() << "/" << baseSpeed;

        if (isBusy()) {
            cout << " [BUSY]";
        }
        else if (isOnBreak()) {
            cout << " [BREAK until T=" << breakEndTime << "]";
        }
        else if (injured) {
            cout << " [INJURED until T=" << injuryEndTime << "]";
        }
        else {
            cout << " [AVAILABLE]";
        }

        cout << " Orders:" << totalOrdersServed;

        if (breaksSkipped > 0) {
            cout << " BreaksSkipped:" << breaksSkipped;
        }

        cout << endl;
    }
};

#endif
