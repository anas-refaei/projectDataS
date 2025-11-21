#ifndef DEFINITIONS_H
#define DEFINITIONS_H



 // Order Types
enum OrderType {
    TYPE_NRM,    // Normal order - served FIFO (First Come First Served)
    TYPE_VEG,    // Vegan order - served FIFO (First Come First Served)
    TYPE_VIP     // VIP order - served by priority equation
};

// Cook Types
enum CookType {
    TYPE_NRM_COOK,   // Normal cook - can prepare Normal orders only
    TYPE_VEG_COOK,   // Vegan cook - can prepare Vegan orders only
    TYPE_VIP_COOK    // VIP cook - can prepare VIP orders only
};

// Event Types (from input file)
enum EventType {
    EVENT_ARRIVAL,       
    EVENT_CANCELLATION,  
    EVENT_PROMOTION      
};
// Cook Status (for Phase 2)
enum CookStatus {
    AVAILABLE,   // Cook is free and ready to take orders
    BUSY,        // Cook is currently preparing an order
    ON_BREAK     // Cook is on break (after serving BO orders)
};

#endif

