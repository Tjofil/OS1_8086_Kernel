//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    keyevent.h
//   Date:    Maj 2021
//===========================================================================//

#ifndef _OS1_KEYBOARD_EVENT_
#define _OS1_KEYBOARD_EVENT_

#include <thread.h>

class BoundedBuffer;



class KeyboardEvent :public Thread{
public:

	KeyboardEvent (BoundedBuffer*);  
	virtual ~KeyboardEvent () {waitToComplete();}
	Thread* clone() const { return new KeyboardEvent(myBuffer); };
protected:

	virtual void run();

	char znak;

private:
  
	BoundedBuffer* myBuffer;
};




#endif  // _OS1_KEYBOARD_EVENT_
