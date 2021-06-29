#include "SCHEDULE.h"
#include "event.h"
#include "ivtentry.h"
#include "pcb.h"
#include "timer.h"
#include <dos.h>
#include <iostream.h>
#include "semaphor.h"

extern int userMain(int argc, char* argv[]);

class UserThread : public Thread {
  private:
    int retValue;
    int argc;
    char **argv;

  public:
    UserThread(int c, char **v, StackSize s = defaultStackSize,
    Time t = defaultTimeSlice) : Thread(s, t), argc(c), argv(v), retValue(-1) {}
    void run() { retValue = userMain(argc, argv); }
    int getResult() {
        waitToComplete();
        return retValue;
    }

    Thread *clone() const { return new UserThread(argc, argv); }

    ~UserThread() { waitToComplete(); }
};

void createMainPCB() {
    PCB::mainThread = PCB::running = new PCB(1024, 0, 0); // Current thread
    PCB::running->status = MAIN_IDLE;
}

int main(int argc, char* argv[]) {
    initCSTimer();

    interrLock;
    createMainPCB(); 
    UserThread *userMainThread = new UserThread(argc, argv);
    userMainThread->start();
    interrUnlock;

    dispatch();

    int mainRetValue = userMainThread->getResult();
    delete userMainThread;

    restoreStockTimer();
    return mainRetValue;
}
