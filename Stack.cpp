//
// Created by trungngo on 26/11/2019.
//

#include "Stack.h"

Stack::Stack() {
    this->top = nullptr;
}

bool Stack::isEmpty() {
    return this->top == nullptr;
}

char* Stack::peek() {
    return this->top->data;
}

void Stack::push(char *data) {
    Node* node = new Node(data);
    node->next = this->top;
    this->top = node;
}

void Stack::pop() {
    Node* detachedNode = this->top;
    this->top = this->top->next;
    delete detachedNode;
}

void Stack::push(char op) {
//    char tempOp[] = {};
//    tempOp[0] = op;
    Node* node = new Node(op);
    node->next = this->top;
    this->top = node;
}



