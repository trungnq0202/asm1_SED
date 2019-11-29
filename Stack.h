//
// Created by trungngo on 26/11/2019.
//

#ifndef ASM1_SED_STACK_H
#define ASM1_SED_STACK_H


#include "Node.h"

class Stack {
public:
    bool isEmpty();
    char* peek();
    void push(char* data);
    void push(char op);
    void pop();
    //for int stack
    void push(int data);
    int peek(bool isInt);
    Stack();
    ~Stack();
private:
    Node* top;
};



#endif //ASM1_SED_STACK_H
