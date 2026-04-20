#include <stdexcept>
#include <iostream>

#ifndef LIST_H
#define LIST_H

struct Node
{
    float data;
    Node *next;
};

struct LinkedList
{
    Node *head;
    Node *tail;
    int size;
};

class List
{
private:
    LinkedList list;

public:
    List()
    {
        this->list.head = nullptr;
        this->list.tail = nullptr;
        this->list.size = 0;
    };
    int count()
    {
        return this->list.size;
    };
    void add(float data)
    {
        Node *newNode = new Node();
        newNode->data = data;
        newNode->next = nullptr;

        if (this->list.head == nullptr)
        {
            this->list.head = newNode;
            this->list.tail = newNode;
        }
        else
        {
            this->list.tail->next = newNode;
            this->list.tail = newNode;
        }

        this->list.size++;
    };
    void print()
    {
        Node *current = this->list.head;
        while (current != nullptr)
        {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    };
    void deleteElement(int index)
    {
        if (index < 0 || index >= this->list.size)
        {
            throw std::out_of_range("Index out of range");
            return;
        }

        Node *current = this->list.head;
        Node *previous = nullptr;

        for (int i = 0; i < index; i++)
        {
            previous = current;
            current = current->next;
        }

        if (previous == nullptr)
        {
            this->list.head = current->next;
        }
        else
        {
            previous->next = current->next;
        }

        if (current == list.tail)
        {
            this->list.tail = previous;
        }

        delete current;
        this->list.size--;
    };
    float getElement(int index)
    {
        if (index < 0 || index >= this->list.size)
        {
            throw std::out_of_range("Index out of range");
            return -1; // Return an invalid value to indicate error
        }

        Node *current = this->list.head;
        for (int i = 0; i < index; i++)
        {
            current = current->next;
        }

        return current->data;
    };
};

#endif