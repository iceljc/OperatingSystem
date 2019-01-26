/*
 File: vm_pool.C
 
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

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"

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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    
    base_address = _base_address;
    size = _size;
    frame_pool = _frame_pool;
    page_table = _page_table;

    region_num = 0;

    // how many region management info can be stored in a frame at most
    region_max_num = PageTable::PAGE_SIZE / sizeof(allocator_info);

    // get a frame to store the allocate_list
    allocate_list = (allocator_info *)(frame_pool->get_frames(1)*PageTable::PAGE_SIZE);
    
    // register current vm pool
    page_table->register_pool(this);



    Console::puts("Constructed VMPool object.\n");
}

//
unsigned long VMPool::allocate(unsigned long _size) {
    if (_size == 0){
      Console::puts("Unable to allocate because size is zero.\n");
      return 0;
    }

    unsigned long start_addr = 0;
    allocator_info temp;
    unsigned long hole = 0;

    for (int i=0; i<region_num; ++i){
      for (int j=i+1; j<region_num; ++j){
        if (allocate_list[j].base_address < allocate_list[i].base_address){
          temp = allocate_list[i];
          allocate_list[i] = allocate_list[j];
          allocate_list[j] = temp;
        }
      }
    }

    //Console::puti(allocate_list[0].base_address); Console::puts("\n");
    //Console::puti(allocate_list[0].region_size); Console::puts("\n");

    // get start_addr 
    if (region_num < region_max_num)
    {
      if (region_num == 0)
      {
        start_addr = base_address;
      }
      else if (region_num == 1)
      {
        hole = allocate_list[0].base_address - base_address;
        if (_size <= hole){
          start_addr = base_address;
        }else{
          start_addr = allocate_list[region_num-1].base_address + allocate_list[region_num-1].region_size;
        }
      }
      else
      {
        hole = allocate_list[0].base_address - base_address;
        if (_size <= hole){
          start_addr = base_address;
        }else{
          for (int i=0; i<region_num-1; ++i){
            hole = allocate_list[i+1].base_address - (allocate_list[i].base_address+allocate_list[i].region_size);
            if (_size <= hole){
              start_addr = allocate_list[i].base_address + allocate_list[i].region_size;
            }         
          }
          if (start_addr == 0 ){
            start_addr = allocate_list[region_num-1].base_address + allocate_list[region_num-1].region_size;
          }
        }     
      }
    }else{
      Console::puts("Unable to allocate because it is out of space.\n");
      return 0;
    }
    

    // check whether the newly-allocated region is out of the vmpool
    unsigned long end_addr;
    end_addr = base_address + size;

    if ((start_addr + _size) <= end_addr)
    {
      allocate_list[region_num].base_address = start_addr;
      allocate_list[region_num].region_size = _size;
      
      region_num++;
      Console::puts("Allocated region of memory.\n");
      return start_addr;
      
    }
    else if ((start_addr + _size) > end_addr)
    {
      Console::puts("Unable to allocate because it is out of the vmpool.\n");
      return 0;
    }
    

    
}


//
void VMPool::release(unsigned long _start_address) {
    
    unsigned long index;
    for (int i=0; i<region_num; ++i){
      if (allocate_list[i].base_address = _start_address){
        index = i;
        page_table->free_page(_start_address);  //release the pages for current region
        break;
      }
    }
    
    // delete entry and reonstruct allocate_list
    if (region_num > 1){
      for (int j=0; j<region_num; ++j){
        if (j>index){
          allocate_list[j-1] = allocate_list[j];
        }
      }
    }else{
      allocate_list[0].base_address = 0;
      allocate_list[0].region_size = 0;
    }
    
    region_num--;

    // flush TLB by reloading CR3
    page_table->load();


    Console::puts("Released region of memory.\n");
}


//
bool VMPool::is_legitimate(unsigned long _address) {
    /*
    for (int k=0; k<region_num; ++k){
      Console::puti(allocate_list[k].base_address); Console::puts("\n");
    }

    Console::puti(_address);Console::puts("\n");
    */

    // check whether the address is legitimate
    if (allocate_list){
      for (int i=0; i<region_num; ++i){
        if((allocate_list[i].base_address <= _address)&&(_address < allocate_list[i].base_address + allocate_list[i].region_size)){
          Console::puts("Checked whether address is part of an allocated region.\n");
          Console::puts("The address is valid.\n");
          return true;
        }
      }
    }
    Console::puts("Checked whether address is part of an allocated region.\n");
    Console::puts("The address is invalid.\n");
    return false;

    
}

