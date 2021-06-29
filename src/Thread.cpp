#include "Thread.h"
#include "pcb.h"
#include "timer.h"
#include <dos.h>
#include <iostream.h>

Thread::Thread(StackSize stackSize, Time timeQuant) {
    lock;
    myPCB = new PCB(stackSize, timeQuant, this);
    unlock;
}
void Thread::start() {
    if (myPCB == nullptr) return;
    myPCB->PCB::start();
}

ID Thread::getId() {
    if (myPCB == nullptr) return -1;
    return myPCB->ID;
}
ID Thread::getRunningId() {
    if (PCB::running == nullptr) return -1;
    return PCB::running->ID;
}
Thread *Thread::getThreadById(ID id) { return PCB::getThreadById(id); }

void dispatch() {
    interrLock;
    csOnDemand = true;
    timer();
    interrUnlock;
}

Thread *Thread::clone() const {}

ID Thread::fork() {
    lock;
    volatile PCB *toFork = (PCB *)PCB::running;
    int futureChildID = PCB::baseID + 1;
    PCB::fork();
    unlock;

    if (toFork == (PCB *)PCB::running) {
    	if (toFork->hasFailedFork == true){
    		toFork->hasFailedFork = false;
    		return -1;
    	}
    	return futureChildID;
    }
    else
    	return 0;
}

void Thread::waitToComplete() {
    if (myPCB == nullptr) return;
    myPCB->waitToComplete();
}

void Thread::waitForForkChildren() { PCB::waitForForkChildren(); }
void Thread::exit() { PCB::terminate(); }

Thread::~Thread() {
    this->waitToComplete();
    lock;
    if (myPCB != nullptr) delete myPCB;
    unlock;
    myPCB = nullptr;
}
