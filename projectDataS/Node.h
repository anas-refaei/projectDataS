#pragma once
#ifndef _NODE
#define _NODE

// Node class for LinkedList and Queue
template<typename T>
class Node
{
private:
    T item;              // A data item (can be any complex structure)
    Node<T>* next;       // Pointer to next node

public:
    // Default constructor
    Node() : item(T()), next(nullptr) {}

    // Non-default constructor
    Node(T newItem) : item(newItem), next(nullptr) {}

    // Setters
    void setItem(T newItem)
    {
        item = newItem;
    }

    void setNext(Node<T>* nextNodePtr)
    {
        next = nextNodePtr;
    }

    // Getters
    T getItem() const
    {
        return item;
    }

    Node<T>* getNext() const
    {
        return next;
    }

    // ? ALIAS FOR getData() - calls getItem()
    T getData() const
    {
        return item;  // Returns the same as getItem()
    }

    // ? ALIAS FOR setData() - calls setItem()
    void setData(T val)
    {
        item = val;  // Sets the same as setItem()
    }
};

#endif
