#include "kerev.h"
#include "auxil.h"
#include "ivtentry.h"
#include "pcb.h"

KernelEv::KernelEv(IVTNo ivtNo) : myPCB((PCB *)PCB::running), ivtNo(ivtNo), semaphore(0) { IVTEntry::entryTable[ivtNo]->assignEvent(this); }

KernelEv::~KernelEv() { IVTEntry::entryTable[ivtNo]->unassignEvent(); }

void KernelEv::wait() { // wrapping wait and signal
    if (myPCB != (PCB *)PCB::running) return;
    semaphore.wait(0); // unlimited waiting
}

void KernelEv::signal() {
    if (semaphore.val() >= 0) return;
    semaphore.signal();
}
