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
  unsigned int ready_queue_size = 0;

  Node block_queue;
  unsigned int block_queue_size = 0;


  Console::puts("Constructed Scheduler.\n");
}


//
void Scheduler::yield() {

  
  	if (ready_queue_size > 0){
  		ready_queue_size--;
  		Thread* target = ready_queue.dequeue();
  		Thread::dispatch_to(target);
  	}

  	Console::puts("Yielding ... \n");
}


//
void Scheduler::resume(Thread * _thread) {

	ready_queue.enqueue(_thread);
	ready_queue_size++;

	if (block_queue_size != 0) {
		// check if the disk is ready
		if ((Machine::inportb(0x1F7) & 0x08) == 0) {
			return;
		}

		Console::puts("blocking queue size:");
		Console::puti(block_queue_size);Console::puts("\n");

		Thread* target = block_queue.dequeue();
		block_queue_size--;

		ready_queue.enqueue(target);
		ready_queue_size++;
	}


	Console::puts("Resuming ... \n");

}

//
void Scheduler::add(Thread * _thread) {
  
  	ready_queue.enqueue(_thread);
	ready_queue_size++;

	Console::puts("Adding a thread ... \n");

}


//
void Scheduler::terminate(Thread * _thread) {
  
	int check_flag = 0;


	for (int i=0; i<ready_queue_size; ++i){
		Thread* temp = ready_queue.dequeue();
		if (temp->ThreadId() == _thread->ThreadId()){
			check_flag = 1;
		}else{
			ready_queue.enqueue(temp);
		}
	}

	if (check_flag != 0){
		ready_queue_size--;
	}


	Console::puts("Terminating a thread ... \n");

}


void Scheduler::blocking(Thread * _thread) {
	block_queue.enqueue(_thread);
	block_queue_size++;
}



