#ifndef _ivtentry_h_
#define _ivtentry_h_
#include "auxil.h"

#define PREPAREENTRY(entryNum, invokeStock) \
    void interrupt interruptEvent##entryNum(...); \
    IVTEntry ivtEntry##entryNum((entryNum), interruptEvent##entryNum); \
    void interrupt interruptEvent##entryNum(...) { \
        ivtEntry##entryNum.signal(); \
        if (invokeStock == true) ivtEntry##entryNum.stockRoutine(); \
        dispatch(); \
    }

class Event;
typedef unsigned char IVTNo;
class KernelEv;

#define NUM_OF_IVT_ENTRIES 256
class IVTEntry {

    IVTNo entryNum;
    KernelEv *event;
    pInterrupt actualRoutine;

  public:
    void signal();
    static IVTEntry *entryTable[NUM_OF_IVT_ENTRIES];
    pInterrupt stockRoutine;
    IVTEntry(IVTNo en, pInterrupt ar);

    void assignEvent(KernelEv *ev);
    void unassignEvent();
    ~IVTEntry();
};

#endif
