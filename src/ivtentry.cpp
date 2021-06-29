#include "ivtentry.h"
#include "kerev.h"
#include <dos.h>

IVTEntry *IVTEntry::entryTable[256] = {nullptr};

IVTEntry::IVTEntry(IVTNo en, pInterrupt ar) : entryNum(en), actualRoutine(ar), event(nullptr) { entryTable[entryNum] = this; }

void IVTEntry::signal() { event->signal(); }

void IVTEntry::assignEvent(KernelEv *ev) { 
    event = ev;
    interrLock;
    stockRoutine = getvect(entryNum);
    setvect(entryNum, actualRoutine);
    interrUnlock;
}
void IVTEntry::unassignEvent() {
    interrLock;
    setvect(entryNum, stockRoutine);
    interrUnlock;
    entryTable[entryNum] = nullptr;
    event = nullptr;
}

IVTEntry::~IVTEntry() { unassignEvent(); }
