#ifndef _pcb_h_
#define _pcb_h_
#include "Thread.h"
#include "auxil.h"
#include "queue.h"

const StackSize lowerStackBoundary = 1024;
const StackSize upperStackBoundary = 65536;
const unsigned registerWidth = sizeof(unsigned);

enum Status { CREATED, READY, RUNNING, BLOCKED, FINISHED, MAIN_IDLE };

class PCB {
  public:
    struct Node {
        PCB *data;
        Node *next;
        Node(PCB *elem = nullptr) : data(elem), next(nullptr) {}

        static void append(PCB *d);
        static int allFinished();

        static volatile int numOfActivePCBs;
    };

    static volatile Node *PCBHead, *PCBTail;
    static volatile PCB *running,* mainThread;
    static volatile Queue forkCorpses;
    unsigned sp, ss, bp;
    int timeQuant;
    unsigned *stackPt;
    volatile Status status;
    id ID;
    Thread *myThread;
    PCB *parent;
    Queue waiting;
    int numOfActiveChildren;
    boolean isWaitingForChildren, blockTimeExpired, hasFailedFork, birthByFork;
    StackSize _stackSize; //in bytes

    PCB(StackSize stackSize, Time timeSlice, Thread *corrThread, wrapper fun = PCB::execWrapper, Status s = CREATED);
    static void execWrapper();
    static PCB *getIdlePCB();
    static void idleWrapper();
    void waitToComplete();
    void start();
    boolean unblockedByTime();

    static Thread *getThreadById(id ID);

    static void interrupt fork();
    static void terminate();
    static void waitForForkChildren();
    ~PCB();

    volatile static id baseID;
};

#endif
