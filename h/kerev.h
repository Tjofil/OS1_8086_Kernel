#ifndef _kerevent_h_
#define _kerevent_h_
#include "event.h"
#include "semaphor.h"

class PCB;

class KernelEv {
    IVTNo ivtNo;
    PCB *myPCB;
    Semaphore semaphore;

  public:
    KernelEv(IVTNo ivtNo);
    ~KernelEv();
    void wait();
    void signal();
};
#endif