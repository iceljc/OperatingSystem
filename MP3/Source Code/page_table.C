#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
    kernel_mem_pool = _kernel_mem_pool;
    process_mem_pool = _process_mem_pool;
    shared_size = _shared_size;
    paging_enabled = 0;

    Console::puts("Initialized Paging System\n");
}



//
PageTable::PageTable()
{
    page_directory = (unsigned long *)(kernel_mem_pool->get_frames(1)<<12);
    page_table = (unsigned long *)(kernel_mem_pool->get_frames(1)<<12);
    
    unsigned long address = 0;
    unsigned int i;

    //map the first 4MB memory
    for (i = 0; i < shared_size/PAGE_SIZE; ++i){
        
        // attribute set to: supervisor level, read/write, present(011 in binary)
        page_table[i] = address | 3;
        address += PAGE_SIZE;
    
    }
    
    // fill the first entry of the page directory
    // attribute set to: supervisor level, read/write, present(011 in binary)
    page_directory[0] = (unsigned long)page_table;
    page_directory[0] = page_directory[0] | 3;
    
    
    for(i = 1; i < shared_size/PAGE_SIZE; ++i)
    {
        // attribute set to: supervisor level, read/write, not present(010 in binary)
        page_directory[i] = 0 | 2;
    }
    
    
    Console::puts("Constructed Page Table object\n");
    
}



//
void PageTable::load()
{
    current_page_table = this;
    write_cr3((unsigned long)page_directory);
    
    Console::puts("Loaded page table\n");
}



//
void PageTable::enable_paging()
{
    unsigned long cr_0 = read_cr0();
    write_cr0(cr_0 | 0x80000000);  // 
    
    paging_enabled = 1;
    
    Console::puts("Enabled paging\n");
}





//
void PageTable::handle_fault(REGS * _r)
{
    unsigned long *page_Dir = current_page_table->page_directory;
    unsigned long *PT;
    unsigned long address = read_cr2();
    unsigned long error = _r->err_code;
    
    // address = page table number (10 bits) | page number (10 bits) | offset (12 bits)

    if((error & 1) == 1){
        Console::puts("Protection fault.\n");
    }
    
    else
    {
 
        if ((page_Dir[address>>22] & 1) == 0){ // check the last bit in PDE
            
            Console::puts("Page is not present.\n");
            // get a free frame first
            // move the frame number towards left 12 bits, set last 3 bits 011
            // and put it into PDE
            // PDE = page_Dir[address>>22]
            page_Dir[address>>22] = (unsigned long)((kernel_mem_pool->get_frames(1)<<12) | 3);
            
            // get the page table address and set the last 12 bits zero
            PT = (unsigned long *)((page_Dir[address>>22] >> 12) << 12);
            
            // initialize the PTE
            for(int i=0; i<ENTRIES_PER_PAGE; ++i){
                PT[i] = 0;
            }
            
            // put the physical frame number into PTE
            // get the frame address, move towards left 12 bits, and set last 3 bits 011
            // page number = (address >> 12) & 0x3FF
            // PTE = page_Table[(address >> 12) & 0x3FF]
            PT[(address>>12) & 0x3FF] = (process_mem_pool->get_frames(1)<<12) | 3;
            
            Console::puts("page fault handled.\n");
        }
        else{
            Console::puts("Page is present.\n");
            // we already have the page, so no need to update PDE
            // get the page table address
            // PDE = page_Dir[address>>22]
            PT = (unsigned long *)((page_Dir[address>>22] >> 12) << 12);
            
            // put the physical frame number into PTE
            // get the frame address, move towards left 12 bits, and set last 3 bits 011
            // page number = (address >> 12) & 0x3FF
            // PTE = page_Table[(address >> 12) & 0x3FF]
            PT[(address>>12) & 0x3FF] = (process_mem_pool->get_frames(1)<<12) | 3;

            Console::puts("page fault handled.\n");
        }
        
    }
        
}







