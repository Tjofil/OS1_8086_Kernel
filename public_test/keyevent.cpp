//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    keyevent.cpp
//   Date:    Maj 2021
//===========================================================================//
#include <iostream.h>
#include <dos.h>

#include "keyevent.h"
#include "bounded.h"
#include "user.h"
#include "intLock.h"
#include <event.h>

PREPAREENTRY(9,0);
  
//---------------------------------------------------------------------------//
//  Podeseno za qwerty tastature
//  Tabela nije potpuna
//---------------------------------------------------------------------------//
char keymap[128] = { 
  0 , 27,'1','2','3','4','5','6','7','8','9','0','-','=', 8 , 9, 
 'q','w','e','r','t','y','u','i','o','p','[',']', 13, 0 ,'a','s',
 'd','f','g','h','j','k','l',';',0,0,'`','\\','z','x','c','v','b',
 'n','m',',','.','/', 0 ,'*', 0 ,' '
};





//---------------------------------------------------------------------------//
KeyboardEvent::KeyboardEvent(BoundedBuffer* bb) : Thread(), myBuffer(bb) 
{
	theEnd = 0;
}



void KeyboardEvent::run()
{
	intLock
	Event event9(9);
	cout<<"KeyebordListener started!"<<endl;
	intUnlock
	char scancode, status, znak; 
	
	while (!theEnd) {
      
		event9.wait();
		do{
			status = inportb(0x64); // ocitava statusni reg. sa 64h


			if (status & 0x01){           // Can I read?
				scancode = inportb(0x60);
	
				if (scancode==-127){
					theEnd = 1;
					myBuffer->append('!'); //finished
				}else {
					if (scancode&0x80) {
						myBuffer->append(keymap[scancode&0x7F]);
					}
				}
            
			};
       //////////////////////
			asm{
				cli
				in      al, 61h         //; Send acknowledgment without
				or      al, 10000000b   //;   modifying the other bits.
				out     61h, al         //;
				and     al, 01111111b   //;
				out     61h, al         //;
				mov     al, 20h         //; Send End-of-Interrupt signal
				out     20h, al         //;
				sti
			}
       
		 }while (!theEnd && status & 0x01); //dok se ocitava takav status da je pritisnut neki taster

	}// while
	
	intLock
		cout<<endl<<"KeyebordListener stopped!"<<endl;
	intUnlock

}
