//
// Created by trungngo on 26/11/2019.
//

#ifndef ASM1_SED_QUEUE_H
#define ASM1_SED_QUEUE_H

#include "Node.h"

class Queue {
public:
    bool isEmpty();
    char* peek();
    void enqueue(char* data);
    void dequeue();
    Queue();
    ~Queue();
private:
    Node *head;
    Node *tail;
};



#endif //ASM1_SED_QUEUE_H
