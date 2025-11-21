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
    int ID;                  // Unique order identifier
    int arrivalTime;         // AT - Time step when order was placed
    OrderType type;          // TYPE_NRM, TYPE_VEG, or TYPE_VIP
    int size;                // Number of dishes in the order
    float money;             // Total order price

    // Tracking information (calculated during simulation)
    int assignTime;          // Time step when assigned to cook (-1 if not assigned)
    int finishTime;          // FT - Time step when order completed (-1 if not finished)

    // For preemption support (Phase 2)
    int originalSize;        // Original size before any preemption
    int dishesCompleted;     // Number of dishes already prepared

public:
    // Constructor
    Order(int id, int at, OrderType t, int sz, float m);

    // Basic getters
    int getID() const { return ID; }
    int getAT() const { return arrivalTime; }
    OrderType getType() const { return type; }
    int getSize() const { return size; }
    float getMoney() const { return money; }
    int getFinishTime() const { return finishTime; }
    int getAssignTime() const { return assignTime; }
    int getOriginalSize() const { return originalSize; }
    int getDishesCompleted() const { return dishesCompleted; }

    // Setters
    void setSize(int newSize) { size = newSize; }
    void setAssignTime(int time) { assignTime = time; }
    void setFinishTime(int time) { finishTime = time; }
    void setDishesCompleted(int dishes) { dishesCompleted = dishes; }

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

    // Deadline calculation
    int calculateDeadline() const {
        int baseTime = 10;
        int sizeTime = size * 2;
        int priceTime = static_cast<int>(money / 20.0f);
        return arrivalTime + baseTime + sizeTime + priceTime;
    }

    // Check if order is late
    bool isLate() const {
        if (finishTime == -1) return false;
        return finishTime > calculateDeadline();
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


