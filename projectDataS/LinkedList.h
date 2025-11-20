#ifndef _LINKEDLIST
#define _LINKEDLIST

#include "Node.h"
#include <iostream>
using namespace std;

template <typename T>
class LinkedList
{
private:
	Node<T>* Head;	//Pointer to the head of the list
	//You can add tail pointer too (depending on your problem)
public:


	LinkedList()
	{
		Head = nullptr;
	}

	//List is being desturcted ==> delete all items in the list
	~LinkedList()
	{
		DeleteAll();
	}

	/*
	* Function: PrintList.
	* prints the values of all nodes in a linked list.
	*/
	void PrintList()	const
	{
		cout << "\nprinting list contents:\n\n";
		Node<T>* p = Head;

		while (p)
		{
			cout << "[ " << p->getItem() << " ]";
			cout << "--->";
			p = p->getNext();
		}
		cout << "*\n";
	}

	/*
	* Function: InsertBeg.
	* Creates a new node and adds it to the beginning of a linked list.
	*
	* Parameters:
	*	- data : The value to be stored in the new node.
	*/
	void InsertBeg(const T& data)
	{
		Node<T>* R = new Node<T>(data);
		R->setNext(Head);
		Head = R;
	}

	/*
	* Function: DeleteAll.
	* Deletes all nodes of the list.
	*/
	void DeleteAll()
	{
		Node<T>* P = Head;
		while (Head)
		{
			P = Head->getNext();
			delete Head;
			Head = P;
		}
	}

	////////////////     Requirements   ///////////////////
	//
	// Implement the following member functions


	//[1]InsertEnd 
	//inserts a new node at end if the list
	void InsertEnd(const T& data)
	{
		Node<T>* R = new Node<T>(data);
		Node<T>* p = Head;
		while (p->getNext())
		{
			p = p->getNext();
		}
		p->setNext(R);
		R->setNext() = nullptr;
	}

	//[2]Find 
	//searches for a given value in the list, returns true if found; false otherwise.
	bool find(const T data)
	{
		if (Head->getItem() == data)
		{
			return true;
		}
		Node<T*> p = Head;
		while (p->getNext())
		{
			if (p->getItem() == data)
			{
				return true;
			}
			p = p->getNext();

		}
		return false;
	}
	//[3]CountOccurance
	//returns how many times a certain value appeared in the list
	int CountOccurance(const T data)
	{
		int count = 0;
		Node<T*> p = Head;
		while (p)
		{
			if (p->getItem() == data)
				count++;
			p = p->getNext();
		}

	}
	//[4] DeleteFirst
	//Deletes the first node in the list
	void DeleteFirst()
	{
		Node<T*> p = Head->getNext();
		delete Head;
		Head = p;
		delete p;
		p = nullptr;
	}
	//[5] DeleteLast
	//DelD\etes the last node in the list
	void DeleteLast()
	{
		if (Head == nullptr)
			return;
		if (Head->getNext() == nullptr)
		{
			delete Head;
			Head = nullptr;
			return;

		}

		Node<T>* P = Head;
		while (P->getNext()->getNext() != nullptr)
		{
			P = P->getNext();
		}
		delete P->getNext();
		P->setNext(nullptr);
	}
	//[6] DeleteNode
	//deletes the first node with the given value (if found) and returns true
	//if not found, returns false
	//Note: List is not sorted
	bool DeleteNode(T value)
	{
		if (Head == nullptr)
			return false;
		if (Head->getItem() == value)
		{
			if (Head->getNext() == nullptr)
			{
				delete Head;
				Head = NULL;
				return true;
			}
			Node<T>* a = Head;
			Head = Head->getNext();
			delete a;
			a = nullptr;
			return true;
		}
		Node<T>* ptr = Head;
		while (ptr->getNext() != nullptr)
		{
			if (ptr->getNext()->getItem() == value)
			{
				Node<T>* toDelete = ptr->getNext();
				ptr->setNext(toDelete->getNext());
				delete toDelete;
				toDelete = nullptr;
				return true;
			}

			ptr = ptr->getNext();
		}
		return false;
	}

	//[7] DeleteNodes
	//deletes ALL node with the given value (if found) and returns true
	//if not found, returns false
	//Note: List is not sorted
	bool DeleteNodes(T value)
	{
		bool deleteAtLeastOne = false;

		while (DeleteNode(value))
		{
			deleteAtLeastOne = true;
		}

		return deleteAtLeastOne;

	}

	//[8]Merge
	//Merges the current list to another list L by making the last Node in the current list 
	//point to the first Node in list L
	void Merge(LinkedList& L)
	{
		if (Head == nullptr)
		{
			Head = L.Head;
			L.Head = nullptr;
			return;
		}
		Node<T>* ptr = Head;
		while (ptr->getNext() != nullptr)
		{
			ptr = ptr->getNext();
		}
		ptr->setNext(L.Head);
		L.Head = nullptr;
	}

	//[9] Reverse
	//Reverses the linked list (without allocating any new Nodes)
	void Reverse()
	{
		if (Head == nullptr || Head->getNext() == nullptr)
			return;
		Node<T>* curr = Head;
		Node<T>* prev = nullptr;
		while (curr != nullptr)
		{
			Node<T>* next = curr->getNext();
			curr->getNext() = prev;
			prev = curr;
			curr = next;

		}
		Head = prev;
	}




	LinkedList<T>& operator=(const LinkedList<T>& other)
	{
		if (this == &other)  // Self-assignment check
			return *this;

		// Delete existing list
		DeleteAll();

		if (other.Head == nullptr)
		{
			Head = nullptr;
			return *this;
		}

		// Copy first node
		Head = new Node<T>(other.Head->getItem());

		Node<T>* currentNew = Head;
		Node<T>* currentOther = other.Head->getNext();

		while (currentOther)
		{
			Node<T>* newNode = new Node<T>(currentOther->getItem());
			currentNew->setNext(newNode);
			currentNew = newNode;
			currentOther = currentOther->getNext();
		}

		return *this;
	}
};

#endif	

