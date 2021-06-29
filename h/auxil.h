#ifndef _auxil_h_
#define _auxil_h_

typedef void interrupt (*pInterrupt)(...);
typedef void (*wrapper)();
typedef int boolean;
typedef unsigned id;

#define interrLock asm { pushf; cli; }
#define interrUnlock asm popf

#define TimerRoutine 0x0008
#define pswInit 0x0200

#define invokeInterrupt asm int

#define nullptr 0
#define true 1
#define false 0

#define limitlessTimeQuant -1
extern volatile boolean bufferedCS;
extern volatile boolean csOnDemand;
extern volatile int lockDepth;

#define lock ++lockDepth
#define unlock if (--lockDepth == 0 && bufferedCS) { dispatch(); }
#define locked (lockDepth>0)

#endif
