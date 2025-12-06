#ifndef EVENT_H
#define EVENT_H

#include "Definitions.h"
//In C++, the compiler processes each .cpp file independently and needs to know
// about types before they're used. Without Definitions.h, you'd face these critical problems:

class Event {
private:
    EventType type;         //Stores which type of event this is (Arrival 'R', Cancellation 'X', or Promotion 'P')
    int timestamp;
    char orderTypeChar;     //For Arrival events only. Stores 'N' (Normal), 'G' (Vegan), or 'V' (VIP) to indicate what type of order is arriving
    int orderID;            //unique identifier 
    int orderSize;
    float orderMoney;  // uesed here , Priority = waitTime × 0.5 + money × 0.3 + size × 0.2
    int targetID;      //For Cancellation and Promotion events. The ID of the existing order to cancel or promote. In input file
	float extraMoney;   //For Promotion events only. The additional money to add to the order when promoting it to VIP.

public:
    Event(int ts, char typ, int id, int size, float money);   //Constructor 1: Arrival Event
	Event(int ts, int id);                                   //Constructor 2: Cancellation Event
	Event(int ts, int id, float exMoney);                    //Constructor 3: Promotion Event
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Getters for all private members
    EventType getType() const { return type; }
    int getTimestamp() const { return timestamp; }
    char getOrderTypeChar() const { return orderTypeChar; }
    int getOrderID() const { return orderID; }
    int getOrderSize() const { return orderSize; }
    float getOrderMoney() const { return orderMoney; }
    int getTargetID() const { return targetID; }
    float getExtraMoney() const { return extraMoney; }
};

#endif

