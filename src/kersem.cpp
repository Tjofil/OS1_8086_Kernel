#include "kersem.h"
#include "SCHEDULE.H"
#include "Thread.h"
#include "pcb.h"
#include <dos.h>
#include <iostream.h>

STQueue KernelSem::semaphoreEvidence;

KernelSem::KernelSem(int init) {
    val = ((init > 0) ? init : 0);
    lock;
    semaphoreEvidence.push(this); // keeping evidence of all semaphores in order to perform
    unlock;                       // decrement of timeslice for all threads that are time blocked
}

int KernelSem::getVal() { return val; }

int KernelSem::wait(Time maxTimeToWait) {
    lock;
    boolean unblockedBySignal = true;
    if (--val < 0) {
        PCB::running->status = BLOCKED;
        if (maxTimeToWait == 0)
            blocked.push((PCB *)PCB::running);
        else {
            timeBlocked.push((PCB *)PCB::running, maxTimeToWait);
        }
        unlock;
        dispatch();
        lock;
        if (PCB::running->unblockedByTime()) { // Could be done without getter
            PCB::running->blockTimeExpired = false;
            unblockedBySignal = false;
        }
    }
    unlock;
    return unblockedBySignal;
}

void KernelSem::signal() {
    lock;
    boolean threadUnblocked = true;
    PCB *toRelease;
    if (blocked.isEmpty() == false)          // Giving advantage to threads that don't have time limits
        toRelease = blocked.pop();           // for full fair approach, timeBlocked and blocked could be merged into
    else if (timeBlocked.isEmpty() == false) // one timeBlocked list with special timeSlice value for blocked ones
        toRelease = timeBlocked.pop();
    else
        threadUnblocked = false;

    if (threadUnblocked == true) {
        toRelease->status = READY;
        Scheduler::put(toRelease);
    }
    val++;
    unlock;
}

void KernelSem::decReleaseGlobal() {
    STQueue::Node *iterator = semaphoreEvidence.head;
    while (iterator != nullptr) {
        if (iterator->data->timeBlocked.isEmpty() == false) {
            iterator->data->timeBlocked.decRelease();
        }
        iterator = iterator->next;
    }
#ifdef ForkDeallocate // In case programmer wants Kernel to take care of finished child threads
    Thread *toDelete; // Disabled by default
    while (PCB::forkCorpses.isEmpty() == false) {
        toDelete = PCB::forkCorpses.pop()->myThread;
        delete toDelete;
    }
#endif
}

KernelSem::~KernelSem() {
    lock;
    STQueue::Node *iterator = semaphoreEvidence.head;
    STQueue::Node *previous = nullptr;

    while (!blocked.isEmpty() || !timeBlocked.isEmpty()) signal();

    while (iterator != nullptr && iterator->data == this) {
        previous = iterator;
        iterator = iterator->next;
    }
    if (iterator == nullptr) {
        unlock;
        return;
    }
    if (previous != nullptr) {
        previous->next = iterator->next;
        delete iterator;
    } else {
        semaphoreEvidence.head = iterator->next;
        delete iterator;
        if (semaphoreEvidence.head == nullptr) semaphoreEvidence.tail = nullptr;
    }

    unlock;
}
