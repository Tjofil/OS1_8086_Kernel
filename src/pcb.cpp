#include "pcb.h"
#include "SCHEDULE.h"
#include <dos.h>
#include <iostream.h>
#include <stdio.h>

volatile id PCB::baseID = 0;
volatile PCB *PCB::running = nullptr;
volatile PCB *PCB::mainThread = nullptr;
volatile PCB::Node *PCB::PCBHead = nullptr;
volatile PCB::Node *PCB::PCBTail = nullptr;
volatile int PCB::Node::numOfActivePCBs = 0;
volatile Queue PCB::forkCorpses;

extern unsigned tsp;
extern unsigned tss;
extern unsigned tbp;

void PCB::Node::append(PCB *d) {
    volatile PCB::Node *spawn = new Node(d);
    if (PCB::PCBHead == nullptr)
        PCB::PCBHead = PCB::PCBTail = spawn;
    else
        PCB::PCBTail = PCB::PCBTail->next = (PCB::Node *)spawn;

}
int PCB::Node::allFinished() { return numOfActivePCBs == 0; }

PCB::PCB(StackSize stackSize, Time timeSlice, Thread *corrThread, wrapper fun, Status s) {
    if (stackSize < lowerStackBoundary) stackSize = lowerStackBoundary;
    if (stackSize > upperStackBoundary) stackSize = upperStackBoundary;
    unsigned long stackCeil = stackSize / registerWidth;
    stackPt = new unsigned[stackCeil];
    stackPt[stackCeil - 1] = pswInit; // Initialization of PSW, all bits are set to 0 except for I flag
    stackPt[stackCeil - 2] = FP_SEG(fun);
    stackPt[stackCeil - 3] = FP_OFF(fun);
    stackPt[stackCeil - 12] = 0x0000;  // Sentinel for recursive BP hunt (Can't be legal BP_OLD(i) value)
    ss = FP_SEG(stackPt + stackCeil - 12);
    sp = bp = FP_OFF(stackPt + stackCeil - 12);
    // Trash values for all other registers
    status = s;
    myThread = corrThread;
    parent = nullptr;        //
    numOfActiveChildren = 0; // For waitForForkChildren functionality
    isWaitingForChildren = birthByFork =  blockTimeExpired = hasFailedFork = false;
    timeQuant = (timeSlice ? timeSlice : limitlessTimeQuant);
    ID = ++baseID;
    _stackSize = stackSize;
    PCB::Node::append(this);
}

void PCB::execWrapper() {
    running->myThread->run();
    terminate();
}

void PCB::terminate() {
	lock;
    while (running->waiting.isEmpty() == false) { // Releasing threads that have been waiting
        PCB *toRelease = running->waiting.pop();
        toRelease->status = READY;
        Scheduler::put(toRelease);
    }
    PCB *iterator = (PCB *)PCB::running->parent;
    while (iterator != nullptr) {  //recursively notifying all predecessors that successor thread is finished
        iterator->numOfActiveChildren--;
        if (iterator->isWaitingForChildren && iterator->numOfActiveChildren == 0) {
            iterator->isWaitingForChildren = false;
            iterator->status = READY;
            Scheduler::put(iterator);
        }
        iterator = iterator->parent;
    }
    if (running->numOfActiveChildren > 0) {   //reconnecting childs of a terminating parent to his parent thread
    	PCB::Node * toReconnect =(PCB::Node*) PCB::PCBHead;  //null if there is no parent in hierarchy
    	while(toReconnect != nullptr){
    		if (toReconnect->data->parent == running){
    			toReconnect->data->parent = running->parent;
    		}
    		toReconnect = toReconnect->next;
    	}
    }
    running->status = FINISHED;
    Node::numOfActivePCBs--;
#ifdef ForkDeallocate
    if (running->birthByFork == true) forkCorpses.push((PCB*)running);
#endif
    unlock;
    dispatch();
}

void PCB::waitForForkChildren() {
    lock;
    if (PCB::running->numOfActiveChildren > 0) {
        PCB::running->isWaitingForChildren = true;
        PCB::running->status = BLOCKED;
        dispatch();
    }
    unlock;
}

void PCB::idleWrapper() {
    while (1) {
        if (PCB::Node::allFinished()) Scheduler::put((PCB*)PCB::mainThread);
        dispatch();
    }
}

Thread *PCB::getThreadById(id ID) {
    lock;
    Thread *retThread = nullptr;
    PCB::Node *iterator = (PCB::Node *)PCB::PCBHead;
    while (iterator->data->ID != ID && iterator != nullptr) iterator = iterator->next;
    if (iterator != nullptr) retThread = iterator->data->myThread;
    unlock;
    return retThread;
}

boolean PCB::unblockedByTime() { return blockTimeExpired; }

void PCB::start() {
    lock;
    if (status == CREATED) {
        status = READY;
        Scheduler::put(this);
        if (this->status != MAIN_IDLE) PCB::Node::numOfActivePCBs++;
    }
    unlock;
}

PCB *PCB::getIdlePCB() {
    static PCB idlePCB(lowerStackBoundary, 1, nullptr, PCB::idleWrapper, MAIN_IDLE);
    return &idlePCB;
}

void PCB::waitToComplete() {
    lock;
    if (PCB::running != this && status != FINISHED && status != CREATED && status != MAIN_IDLE) {
        PCB::running->status = BLOCKED;
        waiting.push((PCB *)PCB::running);
        dispatch();
    }
    unlock;
}

volatile unsigned actualbp, actualss, actualsp, gap;
volatile unsigned oldbp, bpgap, oldchildbp, childbp;
volatile unsigned segmentDif, offsetDif, esrestore;
volatile PCB *parentPCB;
volatile PCB *childPCB;
volatile Thread* childThread;
volatile unsigned stackCeil;

void interrupt PCB::fork() {

    lock;
    asm {
    	mov actualss, ss      
	mov actualbp, bp
    }
    // Should always be bp == sp in this context (inside routine marked as interrupt)
    parentPCB = (PCB *)PCB::running;
    childThread = parentPCB->myThread->clone();


    if (childThread == nullptr || childThread->myPCB == nullptr || childThread->myPCB->stackPt == nullptr){
        if (childThread != nullptr) delete childThread;
    	parentPCB->hasFailedFork = true;
    	unlock;
    	return;
    }

    childPCB = childThread->myPCB;

    stackCeil = parentPCB->_stackSize / registerWidth;
    memcpy(childPCB->stackPt, parentPCB->stackPt, parentPCB->_stackSize);

    segmentDif = FP_SEG(parentPCB->stackPt + stackCeil - 1) - actualss;
    offsetDif = FP_OFF(parentPCB->stackPt + stackCeil - 1) - actualbp;
    childPCB->ss = FP_SEG(childPCB->stackPt + stackCeil - 1) - segmentDif;
    childPCB->sp = childPCB->bp = FP_OFF(childPCB->stackPt + stackCeil - 1) - offsetDif;

    childPCB->parent = (PCB *)parentPCB;
    childPCB->birthByFork = true;
    while (parentPCB != nullptr) {        // recursively assigning successors to predecessor threads
        parentPCB->numOfActiveChildren++;
        parentPCB = parentPCB->parent; 
    }

    childbp = childPCB->bp;
    actualss = childPCB->ss;
    
    asm {
        mov esrestore, es       // saving es, planning to use it for direct memory access
        mov es, actualss
        mov actualbp, bp
    }
    loop:
    asm {
        mov ax, word ptr [bp]   // "actual" BP always points to his previous BP
        mov oldbp, ax   		
	sub ax, bp				
	mov gap, ax				// finding difference between oldbp(i+1) and oldbp(i)

        mov bx, childbp  		// will use the childbp(i) to find childbp(i+1)
        mov ax, childbp 		
        add ax, gap  			
	mov childbp, ax			// passing onto next iteration of childbp before i use ax again
        mov word ptr es:bx, ax  
	mov bp, oldbp 			
        mov ax, bp
	add ax, 0 				//
	jnz loop	            // checking if i hit sentinel
    }

    asm {
        mov bp, actualbp   //restoring essentials thay i've been using in bp hunt
        mov es, esrestore
    }

    childPCB->start();      
    unlock;
}

PCB::~PCB() {
    lock;
    if (stackPt != nullptr) {
        delete[] stackPt;
        stackPt = nullptr;
    }
    PCB::Node *previous = nullptr;
    PCB::Node *iterator = (PCB::Node *)PCB::PCBHead;
    while (iterator != this && iterator != nullptr) {
        previous = iterator;
        iterator = iterator->next;
    }
    if (iterator == nullptr) {
        unlock;
        return;
    }
    if (previous != nullptr)
        previous->next = iterator->next;
    else
        PCB::PCBHead = iterator->next;
    if (iterator->next == nullptr) PCB::PCBTail = previous;
    iterator->next = nullptr; 
    delete iterator;
    unlock;
}
