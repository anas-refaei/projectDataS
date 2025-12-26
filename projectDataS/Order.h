#ifndef ORDER_H
#define ORDER_H

#include "Definitions.h"

/*
* Order Class
* Represents a customer order in the restaurant system
*
* Contains all order information: ID, arrival time, type, size, price
* Tracks assignment time, finish time, and calculates priority for VIP orders
* Supports preemption for Phase 2 requirements
*/
class Order {
private:
    // Basic order information (from input file)
    int ID;                    // Unique order identifier
    int arrivalTime;           // AT - Time step when order was placed
    OrderType type;            // TYPE_NRM, TYPE_VEG, or TYPE_VIP
    OrderType originalType;    // ? NEW: Original type at arrival (never changes)
    int size;                  // Number of dishes in the order
    float money;               // Total order price

    // Tracking information (calculated during simulation)
    int assignTime;            // Time step when assigned to cook (-1 if not assigned)
    int finishTime;            // FT - Time step when order completed (-1 if not finished)

    // For preemption support (Phase 2)
    int originalSize;          // Original size before any preemption
    int dishesCompleted;       // Number of dishes already prepared

    // ? DEADLINE & LATENESS (REQUIRED)
    int deadline;              // Deadline time D = AT + f(SIZE, PRICE)
    bool late;                 // Is the order late? (FT > deadline)
    int WT;           // Waiting Time
    int ST;
public:
    // Constructor
    Order(int id, int at, OrderType t, int sz, float m);
    //int getWT() const { return WT; }
    //int getST() const { return ST; }
    // Basic getters
    int getID() const { return ID; }
    int getAT() const { return arrivalTime; }
    OrderType getType() const { return type; }
    OrderType getOriginalType() const { return originalType; }  // ? NEW GETTER
    int getSize() const { return size; }
    float getMoney() const { return money; }
    int getFinishTime() const { return finishTime; }
    int getAssignTime() const { return assignTime; }
    int getOriginalSize() const { return originalSize; }
    int getDishesCompleted() const { return dishesCompleted; }

    // ? DEADLINE GETTERS:
    int getDeadline() const { return deadline; }
    bool getIsLate() const { return late; }

    // Setters
    void setSize(int newSize) { size = newSize; }
    void setAssignTime(int time) { assignTime = time; }
    void setFinishTime(int time) { finishTime = time; }
    void setDishesCompleted(int dishes) { dishesCompleted = dishes; }
    void setType(OrderType t) { type = t; }  // Changes current type, NOT originalType
    void addMoney(float extraMoney) { money += extraMoney; }
    void setWT(int time) { WT = time; }
    void setST(int time) { ST = time; }
    // ? LATENESS SETTER:
    void setIsLate(bool isOrderLate) { late = isOrderLate; }

    // Priority calculation (for VIP orders only)
    float calculatePriority(int currentTime) const;

    // Time calculations (PDF requirements)
    int getWT() const {
        if (assignTime == -1) return 0;
        return assignTime - arrivalTime;
    }

    int getST() const {
        if (finishTime == -1 || assignTime == -1) return 0;
        return finishTime - assignTime;
    }

    // ? DEADLINE CALCULATION (REQUIRED)
    // Formula: D = AT + baseTime + (SIZE * factor) + (PRICE / divisor)
    int calculateDeadline() const {
        int baseTime = 10;                             // Base waiting allowance
        int sizeTime = size * 2;                       // Larger orders get more time
        int priceTime = static_cast<int>(money / 20.0f); // Higher price = more tolerance
        return arrivalTime + baseTime + sizeTime + priceTime;
    }

    // ? CHECK IF ORDER IS LATE (REQUIRED)
    // Called when order finishes to update late status
    void checkLateness() {
        if (finishTime != -1 && finishTime > deadline) {
            late = true;
        }
    }

    // Preemption support
    int getRemainingSize() const {
        return size - dishesCompleted;
    }

    bool hasBeenPreempted() const {
        return size < originalSize;
    }

    // Display
    void display() const;

    const char* getTypeString() const {
        switch (type) {
        case TYPE_NRM: return "Normal";
        case TYPE_VEG: return "Vegan";
        case TYPE_VIP: return "VIP";
        default: return "Unknown";
        }
    }
};

#endif
