#include "tqueue.h"
#include "auxil.h"
#include "SCHEDULE.H"
#include <iostream.h>
#include <dos.h>

void TQueue::push(PCB *elem, Time toWait) {
    TNode *spawn = new TNode(elem, toWait);
    if (head == nullptr) {
        head = spawn;
        tail = head;
        return;
    }
    if (toWait < head->wait) {
        spawn->next = head;
        head->wait -= toWait;
        head = spawn;
    } else {
        TNode *iterator = head->next;
        TNode *previous = head;
        while (iterator != nullptr && toWait < iterator->wait) {
            toWait -= iterator->wait;
            previous = iterator;
            iterator = iterator->next;
        }
        previous->next = spawn;
        spawn->next = iterator;
        if (iterator != nullptr)
            iterator->wait -= toWait;
        else
            tail = spawn;
    }
}

PCB *TQueue::pop() {
    if (head == nullptr) return nullptr;
    TNode *toRemove = head;
    PCB *popped = toRemove->data;
    head = head->next;
    head->wait += toRemove->wait;
    if (head == nullptr) tail = nullptr;
    delete toRemove;
    return popped;
}

int TQueue::decRelease() {
    if (head != nullptr) head->wait--;
    int numOfReleased = 0;
    while (head != nullptr && head->wait <= 0) {
        TNode *iterator = head;
        iterator->data->status = READY;
        iterator->data->blockTimeExpired = true;
        numOfReleased++;
        Scheduler::put(iterator->data);
        head = head->next;
        head->wait += iterator->wait;
        delete iterator;
    }
    return numOfReleased;
}

boolean TQueue::isEmpty() { return ((head == nullptr) ? true : false); }

TQueue::~TQueue() {
    while (head != nullptr) {
        TNode *toRemove = head;
        head = head->next;
        delete toRemove;
    }
    tail = nullptr; 
}
