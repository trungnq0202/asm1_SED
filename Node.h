//
// Created by trungngo on 26/11/2019.
//

#ifndef ASM1_SED_NODE_H
#define ASM1_SED_NODE_H


#define MAX_DATA_SIZE 100

class Node {
public:
    char data[MAX_DATA_SIZE] = {0};
    int dataInt = 0;
    explicit Node(const char* data);
    explicit Node(char data);
    explicit Node(int data);
    Node* next;
private:
};


#endif //ASM1_SED_NODE_H
