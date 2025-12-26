#pragma once
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <iostream>
#include "Node.h"  // ? Include Node.h instead of defining Node again
using namespace std;

// LinkedList class (Node is defined in Node.h)
template <typename T>
class LinkedList {
private:
    Node<T>* head;
    int count;

public:
    // Constructor
    LinkedList() : head(nullptr), count(0) {}

    // Destructor
    ~LinkedList() {
        DeleteAll();
    }

    // Copy constructor
    LinkedList(const LinkedList<T>& other) : head(nullptr), count(0) {
        Node<T>* current = other.head;
        while (current != nullptr) {
            InsertEnd(current->getData());
            current = current->getNext();
        }
    }

    // Assignment operator
    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            DeleteAll();
            Node<T>* current = other.head;
            while (current != nullptr) {
                InsertEnd(current->getData());
                current = current->getNext();
            }
        }
        return *this;
    }

    // ? GETTER FOR HEAD (NEEDED FOR OUTPUT FILE)
    Node<T>* getHead() const { return head; }

    // Get size
    int size() const { return count; }

    // Check if empty
    bool isEmpty() const { return head == nullptr; }

    // Insert at beginning
    void InsertBeg(T item) {
        Node<T>* newNode = new Node<T>(item);
        newNode->setNext(head);
        head = newNode;
        count++;
    }

    // Insert at end
    void InsertEnd(T item) {
        Node<T>* newNode = new Node<T>(item);

        if (head == nullptr) {
            head = newNode;
        }
        else {
            Node<T>* current = head;
            while (current->getNext() != nullptr) {
                current = current->getNext();
            }
            current->setNext(newNode);
        }
        count++;
    }

    // Insert at specific position (0-indexed)
    bool InsertAt(T item, int position) {
        if (position < 0 || position > count) {
            return false;
        }

        if (position == 0) {
            InsertBeg(item);
            return true;
        }

        Node<T>* newNode = new Node<T>(item);
        Node<T>* current = head;

        for (int i = 0; i < position - 1; i++) {
            current = current->getNext();
        }

        newNode->setNext(current->getNext());
        current->setNext(newNode);
        count++;
        return true;
    }

    // Delete from beginning
    bool DeleteBeg() {
        if (head == nullptr) {
            return false;
        }

        Node<T>* temp = head;
        head = head->getNext();
        delete temp;
        count--;
        return true;
    }

    // Delete from end
    bool DeleteEnd() {
        if (head == nullptr) {
            return false;
        }

        if (head->getNext() == nullptr) {
            delete head;
            head = nullptr;
            count--;
            return true;
        }

        Node<T>* current = head;
        while (current->getNext()->getNext() != nullptr) {
            current = current->getNext();
        }

        delete current->getNext();
        current->setNext(nullptr);
        count--;
        return true;
    }

    // Delete at specific position (0-indexed)
    bool DeleteAt(int position) {
        if (position < 0 || position >= count || head == nullptr) {
            return false;
        }

        if (position == 0) {
            return DeleteBeg();
        }

        Node<T>* current = head;
        for (int i = 0; i < position - 1; i++) {
            current = current->getNext();
        }

        Node<T>* temp = current->getNext();
        current->setNext(temp->getNext());
        delete temp;
        count--;
        return true;
    }

    // Delete all nodes
    void DeleteAll() {
        while (head != nullptr) {
            Node<T>* temp = head;
            head = head->getNext();
            delete temp;
        }
        count = 0;
    }

    // ? GET ENTRY AT POSITION (NEEDED FOR OUTPUT FILE)
    T getEntry(int position) const {
        if (position < 0 || position >= count || head == nullptr) {
            return T(); // Return default value if invalid
        }

        Node<T>* current = head;
        for (int i = 0; i < position; i++) {
            current = current->getNext();
        }
        return current->getData();
    }

    // Find item by value
    bool Find(T item) const {
        Node<T>* current = head;
        while (current != nullptr) {
            if (current->getData() == item) {
                return true;
            }
            current = current->getNext();
        }
        return false;
    }

    // Get first element
    T GetFirst() const {
        if (head == nullptr) {
            return T(); // Return default value
        }
        return head->getData();
    }

    // Get last element
    T GetLast() const {
        if (head == nullptr) {
            return T();
        }

        Node<T>* current = head;
        while (current->getNext() != nullptr) {
            current = current->getNext();
        }
        return current->getData();
    }

    // Peek (like GetFirst but for queue operations)
    bool peek(T& item) const {
        if (head == nullptr) {
            return false;
        }
        item = head->getData();
        return true;
    }

    // Print list (for debugging)
    void Print() const {
        Node<T>* current = head;
        cout << "List [" << count << " items]: ";
        while (current != nullptr) {
            cout << current->getData() << " -> ";
            current = current->getNext();
        }
        cout << "NULL" << endl;
    }

    // ? ITERATOR SUPPORT (OPTIONAL BUT USEFUL)
    class Iterator {
    private:
        Node<T>* current;

    public:
        Iterator(Node<T>* node) : current(node) {}

        T operator*() const {
            return current->getData();
        }

        Iterator& operator++() {
            if (current != nullptr) {
                current = current->getNext();
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }
};

#endif
