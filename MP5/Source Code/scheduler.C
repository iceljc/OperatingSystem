/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "machine.H"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {

  Node ready_queue;
  unsigned int queue_size = 0;


  Console::puts("Constructed Scheduler.\n");
}


//
void Scheduler::yield() {

	if (Machine::interrupts_enabled()){
		Machine::disable_interrupts();
	}
  
  	if (queue_size > 0){
  		queue_size--;
  		Thread* target = ready_queue.dequeue();
  		Thread::dispatch_to(target);
  	}

  	Machine::enable_interrupts();

  	Console::puts("Yielding ... \n");
}


//
void Scheduler::resume(Thread * _thread) {

	if (Machine::interrupts_enabled()){
		Machine::disable_interrupts();
	}
  
	ready_queue.enqueue(_thread);
	queue_size++;

	Machine::enable_interrupts();

	Console::puts("Resuming ... \n");

}

//
void Scheduler::add(Thread * _thread) {
  
  	ready_queue.enqueue(_thread);
	queue_size++;

	Console::puts("Adding a thread ... \n");

}


//
void Scheduler::terminate(Thread * _thread) {
  
	int check_flag = 0;

	if (Machine::interrupts_enabled()){
		Machine::disable_interrupts();
	}

	for (int i=0; i<queue_size; ++i){
		Thread* temp = ready_queue.dequeue();
		if (temp->ThreadId() == _thread->ThreadId()){
			check_flag = 1;
		}else{
			ready_queue.enqueue(temp);
		}
	}

	if (check_flag != 0){
		queue_size--;
	}

	Machine::enable_interrupts(); 

	Console::puts("Terminating a thread ... \n");

}






