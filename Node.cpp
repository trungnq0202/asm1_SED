//
// Created by trungngo on 26/11/2019.
//

#include "Node.h"

Node::Node(const char* data) {
    this->next = nullptr;
    int index = 0;
    while (*data){
        this->data[index] = *data;
        index++; data++;
    }
}

Node::Node(char data) {
    this->next = nullptr;
    this->data[0] = data;
}

Node::Node(int data) {
    this->dataInt = data;
}
