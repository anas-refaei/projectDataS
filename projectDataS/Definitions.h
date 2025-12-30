#ifndef DEFINITIONS_H
#define DEFINITIONS_H



 // Order Types
enum OrderType {
    TYPE_NRM,    // Normal order - served FIFO (First Come First Served)
    TYPE_VEG,    // Vegan order - served FIFO (First Come First Served)
    TYPE_VIP,    // VIP order - served by priority equation
    TYPE_QCK,    // Express/Quick order - small orders, faster service
    TYPE_CTR,    // Catering order - large bulk orders for events
    TYPE_CNT  
};

// Cook Types
enum CookType {
    TYPE_NRM_COOK,   // Normal cook - can prepare Normal orders only
    TYPE_VEG_COOK,   // Vegan cook - can prepare Vegan orders only
    TYPE_VIP_COOK,   // VIP cook - can prepare VIP orders only
    TYPE_QCK_COOK,   // Quick cook - handles Express orders (fast service)
    TYPE_CTR_COOK    // Catering cook - handles large Catering orders
};

// Event Types (from input file)
enum EventType {
    EVENT_ARRIVAL,       
    EVENT_CANCELLATION,  
    EVENT_PROMOTION      
};
// Cook Status (for Phase 2)

enum ORD_STATUS	//order status
{
	WAIT,	//waiting to be served
	SRV,	//In-service but not finished (assigned to a cook)
	DONE,	//Finished serving
	ORD_STATUS_CNT
};

enum PROG_MODE	//mode of the program interface
{
	MODE_INTR,	//interactive mode
	MODE_STEP,	//Step-by-step mode
	MODE_SLNT,	//Silent mode
	MODE_DEMO,	//Demo mode (for exhibition)
	MODE_CNT	//Total number of modes
};


#define MaxPossibleOrdCnt 999	//max possible order count (arbitrary value)
#define MaxPossibleMcCnt  100	//max possible cook count (arbitrary value)

#endif

