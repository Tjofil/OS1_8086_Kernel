#ifndef _stqueue_h_
#define _stqueue_h_
#include "auxil.h"
class KernelSem;

class STQueue {
  private:
    struct Node {
        KernelSem *data;
        Node *next;
        Node(KernelSem *elem) : data(elem), next(nullptr) {}
    };

  public:
    Node *head, *tail;

  public:
    STQueue() : tail(nullptr), head(nullptr) {}
    void push(KernelSem *elem);
    void removeSem();
    boolean isEmpty();
    virtual ~STQueue();
};

#endif
