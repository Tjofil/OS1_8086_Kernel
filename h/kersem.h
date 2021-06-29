#ifndef _kersem_h_
#define _kersem_h_
#include "queue.h"
#include "auxil.h"
#include "semaphor.h"
#include "tqueue.h"
#include "stqueue.h"

class KernelSem {
  public:
    static STQueue semaphoreEvidence;
    int val;
    Queue blocked;
    TQueue timeBlocked;

    KernelSem(int init);
    int getVal();
    virtual ~KernelSem();
    virtual int wait(Time maxTimeToWait);
    virtual void signal();
    static void decReleaseGlobal();

};

#endif
