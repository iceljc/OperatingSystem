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

// #include "scheduler.H"
#include "rr.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "simple_timer.H"
#include "interrupts.H"

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

RRScheduler::RRScheduler() {

  Node ready_queue_rr;
  unsigned int queue_size = 0;

  unsigned int freq = 100;
  RRTimer rr_timer(freq);
  InterruptHandler::register_handler(0, &rr_timer);


  Console::puts("Constructed Scheduler.\n");
}


//
void RRScheduler::yield() {

	if (Machine::interrupts_enabled())
		Machine::disable_interrupts();
  
  	if (queue_size > 0){
  		queue_size--;
  		Thread* target = ready_queue_rr.dequeue();
  		Thread::dispatch_to(target);
  	}

  	Machine::enable_interrupts();
}


//
void RRScheduler::resume(Thread * _thread) {

	if (Machine::interrupts_enabled())
		Machine::disable_interrupts();
  
	ready_queue_rr.enqueue(_thread);
	queue_size++;

	Machine::enable_interrupts();

}

//
void RRScheduler::add(Thread * _thread) {
  
  	ready_queue_rr.enqueue(_thread);
	queue_size++;

}


//
void RRScheduler::terminate(Thread * _thread) {
  
	int check_flag = 0;

	if (Machine::interrupts_enabled())
		Machine::disable_interrupts();

	for (int i=0; i<queue_size; ++i){
		Thread* temp = ready_queue_rr.dequeue();
		if (temp->ThreadId() == _thread->ThreadId()){
			check_flag = 1;
		}else{
			ready_queue_rr.enqueue(temp);
		}
	}

	if (check_flag != 0){
		queue_size--;
	}

	Machine::enable_interrupts();

}






