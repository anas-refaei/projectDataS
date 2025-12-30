#ifndef COOK_H
#define COOK_H

#include <iostream>
#include "Definitions.h"
using namespace std;

class Order; 
enum CookState {
    AVAILABLE,
    BUSY,
    ON_BREAK,
    INJURED  
};

class Cook {
private:
    char type; 
    int id; 
    int baseSpeed;           
    double currentSpeed;     
    int breakAfter;          
    int ordersServedStreak;  
    int breakDuration;       
    int breakEndTime;        
    bool injured;            
    int injuryEndTime;      
    int injuryRecoveryTime;  
    bool shouldSkipNextBreak; 
    int breaksSkipped;       
    Order* currentOrder;     
    int remainingDishes;     
    CookState currentState;  
    int totalOrdersServed;
    int totalOrdersServedByType[5]; 
    int totalDishesCooked;
    int totalBusyTime;
    int totalIdleTime;
    int totalBreakTime;
    int totalInjuryTime;     
    int ordersHandled;

public:
    Cook(char t, int s, int breakAfterN, int breakDur, int cookId)
        : type(t), baseSpeed(s), currentSpeed((double)s), 
        breakAfter(breakAfterN), breakDuration(breakDur), id(cookId),
        ordersServedStreak(0), breakEndTime(-1),
        injured(false), injuryEndTime(-1), injuryRecoveryTime(0), 
        shouldSkipNextBreak(false), breaksSkipped(0),              
        currentOrder(nullptr), remainingDishes(0),
        currentState(AVAILABLE),
        totalOrdersServed(0), totalDishesCooked(0),
        totalBusyTime(0), totalIdleTime(0), totalBreakTime(0),
        totalInjuryTime(0)                                       
    {
       
        totalOrdersServedByType[0] = 0;
        totalOrdersServedByType[1] = 0;
        totalOrdersServedByType[2] = 0;
        totalOrdersServedByType[3] = 0;  
        totalOrdersServedByType[4] = 0;  
    }
    char getType() const { return type; }
    int getSpeed() const { return (int)(currentSpeed + 0.5); } 
    int getCurrentSpeedInt() const { return (int)(currentSpeed + 0.5); }
    double getCurrentSpeedDouble() const { return currentSpeed; }
    int getBaseSpeed() const { return baseSpeed; }
    int getBreakAfter() const { return breakAfter; }
    int getOrdersServedStreak() const { return ordersServedStreak; }
    int getBreakEndTime() const { return breakEndTime; }
    int getId() const { return id; }
    Order* getCurrentOrder() const { return currentOrder; }
    int getRemainingDishes() const { return remainingDishes; }

   
    bool isInjured() const { return injured; }
    int getInjuryEndTime() const { return injuryEndTime; }

   
    bool shouldSkipBreak() const { return shouldSkipNextBreak; }
    int getBreaksSkipped() const { return breaksSkipped; }

   
    bool isBusy() const { return currentOrder != nullptr; }
    bool isOnBreak() const { return breakEndTime != -1; }

    bool isAvailable(int currentTime) const {
        return currentOrder == nullptr &&
            (breakEndTime == -1 || currentTime >= breakEndTime) &&
            !injured;
    }


    CookState getState() const { return currentState; }
    void setState(CookState state) { currentState = state; }

    void setCurrentOrder(Order* order) { currentOrder = order; }
    void setRemainingDishes(int dishes) { remainingDishes = dishes; }
    void setStatus(CookState status) { currentState = status; }  


    bool isAvailableForAssignment() const {
        return currentState == AVAILABLE && currentOrder == nullptr && !injured;
    }
    int getTotalOrdersServed() const { return totalOrdersServed; }
    int getTotalOrdersServedByType(int typeIndex) const {
        return totalOrdersServedByType[typeIndex];
    }
    int getTotalDishesCooked() const { return totalDishesCooked; }
    int getTotalBusyTime() const { return totalBusyTime; }
    int getTotalIdleTime() const { return totalIdleTime; }
    int getTotalBreakTime() const { return totalBreakTime; }
    int getTotalInjuryTime() const { return totalInjuryTime; } 
   
    int getOrdersHandled() const {
        return ordersHandled;
    }


    double getUtilization(int totalTime) const {
        if (totalTime == 0) return 0.0;
        int activeTime = totalBusyTime + totalIdleTime;
        if (activeTime == 0) return 0.0;
        return (totalBusyTime * 100.0) / activeTime;
    }

  
    void incrementBusyTime() { totalBusyTime++; }
    void incrementIdleTime() { totalIdleTime++; }
    void incrementBreakTime() { totalBreakTime++; }
    void incrementInjuryTime() { totalInjuryTime++; } 


    void applyFatigue() {
        currentSpeed *= 0.98;

        double minSpeed = baseSpeed * 0.5;
        if (currentSpeed < minSpeed) {
            currentSpeed = minSpeed;
        }
    }

    void restoreSpeed() {
  
        currentSpeed = (double)baseSpeed;
    }

    void causeInjury(int currentTime, int recoveryDuration) {
        injured = true;
        injuryRecoveryTime = recoveryDuration;
        injuryEndTime = currentTime + recoveryDuration;
        currentState = INJURED;


        if (currentOrder != nullptr) {
            currentOrder = nullptr;
            remainingDishes = 0;
        }
    }

    void updateInjuryStatus(int currentTime) {
        if (injured && currentTime >= injuryEndTime) {
            injured = false;
            injuryEndTime = -1;
            currentState = AVAILABLE;
            restoreSpeed();
        }
    }

    void forceWorkWhileInjured() {
        currentSpeed = baseSpeed * 0.5;
    }

    void forceOvertime() {
        shouldSkipNextBreak = true;
        breaksSkipped++;
        currentSpeed *= 0.90; 

        double minSpeed = baseSpeed * 0.4;
        if (currentSpeed < minSpeed) {
            currentSpeed = minSpeed;
        }
    }

    void resetBreakSkip() {
        shouldSkipNextBreak = false;
    }

    bool needsLongerBreak() const {
        return breaksSkipped >= 2;
    }

    int getLongerBreakDuration() const {
        return (int)(breakDuration * 1.5);
    }
    bool canServeOrderType(OrderType orderType) const;
    int getServicePriorityForOrderType(OrderType orderType) const;
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
