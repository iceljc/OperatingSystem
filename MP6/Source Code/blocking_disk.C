/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

extern Scheduler * SYSTEM_SCHEDULER;

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::wait_until_ready() {
  
    SYSTEM_SCHEDULER->blocking(Thread::CurrentThread());

    SYSTEM_SCHEDULER->yield();
}


void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {

  SimpleDisk::read(_block_no, _buf);

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {


  SimpleDisk::read(_block_no, _buf);
}








