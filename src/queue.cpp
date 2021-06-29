#include "queue.h"

void Queue::push(PCB *elem) {
    Node *spawn = new Node(elem);
    if (head == nullptr) {
        head = tail = spawn;
        return;
    }
    tail = tail->next = spawn;
}

PCB *Queue::pop() {
    if (head == nullptr) return nullptr;
    Node *toRemove = head;
    PCB *popped = toRemove->data;
    head = head->next;
    if (head == nullptr) tail = nullptr;
    delete toRemove;
    return popped;
}

boolean Queue::isEmpty() { return ((head == nullptr) ? true : false); }

Queue::~Queue() {
    while (head != nullptr) {
        Node *toRemove = head;
        head = head->next;
        delete toRemove;
    }
    tail = nullptr; // redudant
}
