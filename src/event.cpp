#include "event.h"
#include "auxil.h"
#include "Thread.h"
#include "kerev.h"

Event::Event(IVTNo ivtNo) {
    lock;
    myImpl = new KernelEv(ivtNo); 
    unlock;
}

Event::~Event() {
    lock;
    delete myImpl;
    unlock;
    myImpl = nullptr;
}

void Event::wait() {
    myImpl->wait();
}

void Event::signal() {
    myImpl->signal();
}