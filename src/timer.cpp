#include "timer.h"
#include "SCHEDULE.H"
#include "kersem.h"
#include "pcb.h"
#include <dos.h>
#include <iostream.h>

pInterrupt stockTimer;
volatile Time countdown = defaultTimeSlice;
extern void tick();

unsigned tsp;
unsigned tss;
unsigned tbp;

void initCSTimer() {
    interrLock;
    stockTimer = getvect(0x08);
    setvect(0x08, timer);
    setvect(0x60, stockTimer);
    interrUnlock;
}

void restoreStockTimer() {
    interrLock;
    setvect(0x08, stockTimer);
    interrUnlock;
}

void interrupt timer(...) {
    if (csOnDemand == false) {
    	tick();
        KernelSem::decReleaseGlobal();   //decrementing all time blocked semaphores
        invokeInterrupt 60h;
        if (countdown > 0) countdown--;  //so that threads with unlimited timeslice wont get cs'ed by timer
    }
    if (countdown == 0 || csOnDemand == true) {
        if (locked == false) {
            csOnDemand = false; // Can't corrupt the semantics
            bufferedCS = false; // -||-
            asm {
                mov tsp, sp
                mov tss, ss
                mov tbp, bp
            }
            PCB::running->sp = tsp;
            PCB::running->ss = tss;
            PCB::running->bp = tbp;
            if (PCB::running->status == RUNNING && PCB::running->status != MAIN_IDLE) {
                PCB::running->status = READY;
                Scheduler::put((PCB *)PCB::running);
            }
            PCB::running = Scheduler::get();

            if (PCB::running == nullptr)
            	 PCB::running = PCB::getIdlePCB();

            else
                PCB::running->status = RUNNING;

            tsp = PCB::running->sp;
            tss = PCB::running->ss;
            tbp = PCB::running->bp;
            countdown = PCB::running->timeQuant;

            asm {
                mov sp, tsp
                mov ss, tss
                mov bp, tbp
            }
        } else {
            bufferedCS = true;
            csOnDemand = false; // To preserve the stock routine
        }
    }
}
