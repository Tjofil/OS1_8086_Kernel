#ifndef _semaphore_h_
#define _semaphore_h_

typedef unsigned int Time;

class KernelSem;

class Semaphore {
  public:
    Semaphore(int init = 1);
    virtual ~Semaphore();
    virtual int wait(Time maxTimeToWait);
    virtual void signal();
    int val() const; // Returns the current value of the semaphore
  private:
    KernelSem *myImpl;
};
#endif
