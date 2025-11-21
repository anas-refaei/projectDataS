#include <iostream>
using namespace std;
#ifndef PRIORITY_QUEUE_
#define PRIORITY_QUEUE_
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
			next = nullptr;

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
bool enqueue(const T&newEntry , float priority){
// we will do a new node first 
	PriorityNode* newNode = new PriorityNode(newEntry, priority);
// we have 2 cases , 1st empty queue or the highest pri .
	if (head == nullptr || priority > head->priority) {
		newNode->next = head;
		head = newNode;
	}
//Insert in middle or end according to the priority 
	else {
		PriorityNode* current = head;
		while (current->next != nullptr &&
			current->next->priority >= priority) {
			current = current->next;
		}
// Insert after current
		newNode->next = current->next;
		current->next = newNode;
	}
	count++;
	return true;
}
bool dequeue(T& frontEntry) {
	if (isEmpty()) {
		return false;
	}
	PriorityNode* nodeToDelete = head;
	frontEntry = head->item;
	head = head->next;

	delete nodeToDelete;
	count--;
	return true;
}
bool peek(T& frontEntry) const {
	if (isEmpty()) {
		return false;
	}

	frontEntry = head->item;
	return true;
}
void clear() {
	T temp;
	while (!isEmpty()) {
		dequeue(temp);
	}
}
void printQueue() const {
	if (isEmpty()) {
		cout << "Priority Queue is empty." << endl;
		return;
	}

	cout << "Priority Queue (Front to Back)" << endl;
	PriorityNode* current = head;
	int position = 1;

	while (current != nullptr) {
		cout << "  [" << position << "] Priority: " << current->priority<< ", Item: " << current->item << endl;
		current = current->next;
		position++;
	}
	cout << "Total items: " << count << endl;
}
bool getHighestPriority(float& priority) const {
	if (isEmpty()) {
		return false;
	}
	priority = head->priority;
	return true;
}
};
#endif