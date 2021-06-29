#include "stqueue.h"
#include "auxil.h"
#include <dos.h>
#include <iostream.h>

void STQueue::push(KernelSem *elem) {
    Node *spawn = new Node(elem);
    if (head == nullptr) {
        head = tail = spawn;
        return;
    }
    tail = tail->next = spawn;
}

boolean STQueue::isEmpty() { return ((head == nullptr) ? true : false); }

STQueue::~STQueue() {
    while (head != nullptr) {
        Node *toRemove = head;
        head = head->next;
        delete toRemove;
    }
    tail = nullptr; 
}