// Cook.h
#ifndef COOK_H
#define COOK_H

class Order;  // Forward declaration

class Cook {
private:
    int     id;                     // Unique ID
    char    type;                   // 'V' = VIP cook, 'G' = Vegan cook, 'N' = Normal cook
    int     speed;                  // Dishes per time step (same for all cooks of same type)
    int     breakAfter;             // Take break after serving this many consecutive orders
    int     ordersServedStreak;     // Current streak of served orders without break

    // Current work
    Order* currentOrder;           // nullptr if idle
    int     remainingDishes;        // Of the current order

    // Break state
    bool    onBreak;
    int     breakEndTime;           // Time when break ends (-1 if not on break)

    // Statistics (Phase 2)
    int     totalOrdersServed;
    int     totalDishesCooked;
    int     totalBusyTime;
    int     totalBreakTime;

public:
    // Constructor
    Cook(int cookId, char cookType, int cookSpeed, int breakAfterN);

    // Getters
    int     getId() const;
    char    getType() const;                   // Returns 'V', 'G', or 'N'
    int     getSpeed() const;
    bool    isAvailable(int currentTime) const;
    bool    isBusy() const;
    bool    isOnBreak() const;
    Order* getCurrentOrder() const;
    int     getRemainingDishes() const;

    // State changers
    void    assignOrder(Order* order, int assignTime);
    void    cookOneStep();                      // Reduces remainingDishes by speed
    void    finishOrder(int finishTime);
    void    startBreak(int currentTime);
    void    endBreak(int currentTime);

    // Called every time step
    void    update(int currentTime);

    // Statistics
    int     getTotalOrdersServed() const;
    int     getTotalDishesCooked() const;
    int     getTotalBusyTime() const;
    int     getTotalBreakTime() const;
    double  getUtilization(int totalTime) const;

    void    reset();  // For multiple simulations if needed
};

#endif 