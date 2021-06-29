#include "semaphor.h"
#include "auxil.h"
#include "kersem.h"
#include <dos.h>
#include <iostream.h>

Semaphore::Semaphore(int init) {
    lock;
    myImpl = new KernelSem(init);
    unlock;
}

int Semaphore::wait(Time maxTimeToWait) { return myImpl->wait(maxTimeToWait); }
void Semaphore::signal() { myImpl->signal(); }
int Semaphore::val() const { return myImpl->getVal(); }

Semaphore::~Semaphore() {
    if (myImpl != nullptr) {
        lock;
        delete myImpl;
        unlock;
        myImpl = nullptr;
    }
}
