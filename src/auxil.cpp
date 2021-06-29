#include "auxil.h"

volatile int lockDepth = 0;
volatile boolean csOnDemand = false;
volatile boolean bufferedCS = false;
