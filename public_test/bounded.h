//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    bounded.h
//   Date:    Maj 2021
//   Opis:		Kruzni ograniceni bafer sa sinhronizacijom; 
//						
//===========================================================================//

#ifndef _OS1_BOUNDED_BUFFER_
#define _OS1_BOUNDED_BUFFER_



#include <semaphor.h>



class BoundedBuffer {
public:

	BoundedBuffer (unsigned size);
	virtual ~BoundedBuffer ();
	int append (char);
	char take ();
	int  fullCount(){return itemAvailable.val();};    // potrebno consumeru

private:
	unsigned Size;
	Semaphore mutexa, mutext;
	Semaphore spaceAvailable, itemAvailable;
	char* buffer;
	int head, tail;
   
};


#endif  // _OS1_BOUNDED_BUFFER_
