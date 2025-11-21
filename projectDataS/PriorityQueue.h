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
		PriorityNode() {                       //CONSTRUCTOR of the struct
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
public:                           //CONSTRUCTOR of the class
	PriorityQueue() {

		head = nullptr;
		count=0;
	}
	~PriorityQueue() {
		clear();                  // Delete all nodes (DESRUCTOR)
	}
	PriorityQueue(const PriorityQueue<T>& other) : head(nullptr), count(0) {
		PriorityNode* current = other.head;                              //Copy Constructor (Prevents shallow copy issues)
		while (current != nullptr) {
			enqueue(current->item, current->priority);
			current = current->next;
		}
	}

	PriorityQueue<T>& operator=(const PriorityQueue<T>& other) {
		if (this == &other) return *this;                          // Assignment Operator
		clear();   // prevent the memory leak  
		PriorityNode* current = other.head;
		while (current != nullptr) {
			enqueue(current->item, current->priority);
			current = current->next;
		}
	}
	bool isEmpty() const {
		if (head == nullptr)
			return true;
		else
			return false;
	}
	int size() const {
		return count;
	}
	// Items are inserted in priority order (descending - highest first)
	//Returns true if successful, false otherwise
	// Complexity O(n) where n is the number of items in queue
};