//
// Created by trungngo on 26/11/2019.
//

#include "Node.h"

Node::Node(const char* data) {
    // FIXME: probably loop for 100 chars -> not optimize
    // check this
    // strcpy() equivalent:
    this->next = nullptr;
    for (unsigned int i = 0; i < sizeof(data); i++) {
        this->data[i] = *(data + i);
    }
}