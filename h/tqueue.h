#ifndef _tqueue_h_
#define _tqueue_h_
#include "pcb.h"

class TQueue {

  private:
    struct TNode {
        PCB *data;
        TNode *next;
        Time wait;
        TNode(PCB *elem, Time t) : data(elem), wait(t), next(nullptr) {}
    };

    TNode *head, *tail;

  public:
    TQueue() : tail(nullptr), head(nullptr) {}
    void push(PCB *elem, Time toWait);
    PCB *pop();
    int decRelease();
    boolean isEmpty();
    virtual ~TQueue();
};

#endif