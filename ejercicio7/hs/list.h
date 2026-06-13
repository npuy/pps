#include <stdexcept>
#include <iostream>
#include <fstream>

#ifndef LIST_H
#define LIST_H

template <typename T>
class List
{
private:
    struct Node
    {
        T data;
        Node *next;
    };
    Node *head;
    Node *tail;
    int size;

public:
    List()
    {
        this->head = nullptr;
        this->tail = nullptr;
        this->size = 0;
    };

    ~List()
    {
        Node *current = this->head;
        while (current)
        {
            Node *next = current->next;
            delete current;
            current = next;
        }
    };

    int count()
    {
        return this->size;
    };

    void add(const T &data)
    {
        Node *newNode = new Node();
        newNode->data = data;
        newNode->next = nullptr;

        if (this->head == nullptr)
        {
            this->head = newNode;
            this->tail = newNode;
        }
        else
        {
            this->tail->next = newNode;
            this->tail = newNode;
        }

        this->size++;
    };

    void print()
    {
        Node *current = this->head;
        while (current != nullptr)
        {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    };

    void deleteElement(int index)
    {
        if (index < 0 || index >= this->size)
        {
            throw std::out_of_range("Index out of range");
            return;
        }

        Node *current = this->head;
        Node *previous = nullptr;

        for (int i = 0; i < index; i++)
        {
            previous = current;
            current = current->next;
        }

        if (previous == nullptr)
        {
            this->head = current->next;
        }
        else
        {
            previous->next = current->next;
        }

        if (current == this->tail)
        {
            this->tail = previous;
        }

        delete current;
        this->size--;
    };

    T getElement(int index)
    {
        if (index < 0 || index >= this->size)
        {
            throw std::out_of_range("Index out of range");
            return -1; // Return an invalid value to indicate error
        }

        Node *current = this->head;
        for (int i = 0; i < index; i++)
        {
            current = current->next;
        }

        return current->data;
    };

    void loadFromFile(const char *filename, T (*parseFunc)(const std::string &))
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open file");
        }
        std::string line;
        while (std::getline(file, line))
        {
            T data = parseFunc(line);
            this->add(data);
        }
        file.close();
    };

    template <typename U>
    U reduce(U (*reduceFunc)(const U &, const T &), U initialValue)
    {
        U result = initialValue;
        Node *current = this->head;
        while (current)
        {
            result = reduceFunc(result, current->data);
            current = current->next;
        }
        return result;
    }
};

#endif