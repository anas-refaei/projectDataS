#include <iostream>
using namespace std;
//?????? ??? ?? ?????   
// We can't use the Queue.h instead of this priorityQueue.h as there are diffrent structure Like (float priority.)


template <typename T>
class PriorityQueue {
private:
	struct PriorityNode {
		T item;
		float priority;
		PriorityNode* next;
		PriorityNode() {                       //CONSTRUCTOR
			item = 0;
			priority = 0.0;
			next= nullptr

		}
		PriorityNode(T data, float p) {                  //CONSTRUCTOR
			this->item = data;
			this->priority = p;
			this->next = nullptr;
		}

	};
	PriorityNode* head=nullptr;  
	int count=0;
public:                           //CONSTRUCTOR
	PriorityQueue() {

		head = nullptr;
		count=0;
	}
	~PriorityQueue() {
		clear();                  // Delete all nodes (DESRUCTOR)
	}


};