#ifndef _h_queue_
#define _h_queue_
#include "auxil.h"
class PCB;

class Queue {
  private:
    struct Node {
        PCB *data;
        Node *next;
        Node(PCB *elem) : data(elem), next(nullptr) {}
    };

    Node *head, *tail;

  public:
    Queue() : tail(nullptr), head(nullptr) {}
    void push(PCB *elem);
    PCB *pop();
    boolean isEmpty();
    virtual ~Queue();
};

#endif