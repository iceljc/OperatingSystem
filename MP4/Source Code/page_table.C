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

const unsigned int PageTable::vmpool_max_num = 5;
VMPool* PageTable::vmpool_manager[vmpool_max_num];

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
    page_directory = (unsigned long *)(process_mem_pool->get_frames(1)*FRAME_SIZE);

    page_table = (unsigned long *)(process_mem_pool->get_frames(1)*FRAME_SIZE);

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
    
    
    for(i = 1; i < shared_size/PAGE_SIZE-1; ++i)
    {
        // attribute set to: supervisor level, read/write, not present(010 in binary)
        page_directory[i] = 0 | 2;
    }

    //set the last PDE to point to itself
    page_directory[shared_size/PAGE_SIZE-1] = (unsigned long)page_directory|3; 
    
    for (int i=0; i<vmpool_max_num; ++i){
        vmpool_manager[i] = NULL;
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
    // this address recursively points to the page directory
    // by referencing itself twice
    unsigned long *page_Dir = (unsigned long *)0xFFFFF000;
    unsigned long *page_Table;
    
    unsigned long address;
    unsigned long error = _r->err_code;
    
    if ((error & 1) == 1){
        Console::puts("Protection fault.\n");
    }
    else{
        
        address = read_cr2();
        
        // check if the logical address is legitimate
        Console::puts("Check if the logical address is legitimate. \n");
        VMPool** vm_manager = current_page_table->vmpool_manager;

        int check_flag = -1;
        for (int i=0; i<vmpool_max_num; ++i){
            if (vm_manager[i] != NULL){
                if (vm_manager[i]->is_legitimate(address)){
                    check_flag = i;
                    Console::puts("Valid address...\n");
                    break;
                }
            }
        }

        if (check_flag < 0){
            Console::puts("Invalid address...\n");
        }
        //
        
        
        unsigned long dir_index;
        unsigned long pt_index;
        
        dir_index = address >> 22;
        pt_index = (address >> 12) & 0x3FF;
        
        if ((page_Dir[dir_index] & 1) == 0){
            Console::puts("Page table is not present.\n");
            
            // get a frame from process mem pool
            page_Dir[dir_index] = (unsigned long)((process_mem_pool->get_frames(1)<<12)|3);
            
            // get the address of the new page table
            page_Table = (unsigned long *)(0xFFC00000|(dir_index<<12)); // it points to multiple of 4KB

            // initialize page table
            for (int i=0; i<ENTRIES_PER_PAGE; ++i){
                page_Table[i] = 0;
            }
            
            // load the frame into page table
            page_Table[pt_index] = (process_mem_pool->get_frames(1)<<12) | 3;

            Console::puts("handled page fault.\n");
            
        }else{
            Console::puts("Page table is present.\n");
            
            // get the address of the present page table
            page_Table = (unsigned long *)(0xFFC00000|(dir_index<<12));
            
            // load the frame into page table
            page_Table[pt_index] = ((process_mem_pool->get_frames(1))<<12) | 3;

            Console::puts("handled page fault.\n");
            
        }
        
        
    }
    
    
}

//
void PageTable::register_pool(VMPool * _vm_pool)
{
    int flag = -1;

    for(int i=0; i<vmpool_max_num; ++i){
        if (vmpool_manager[i] == NULL){
            flag = i;
            vmpool_manager[flag] = _vm_pool;
            Console::puts("register VM pool successfull.\n");
            break;
        }
    }

    if (flag < 0){
        Console::puts("register VM pool failed.\n");
    }

    
    
}


//
void PageTable::free_page(unsigned long _page_no) 
{
    unsigned long dir_index = _page_no >> 22;
    unsigned long pt_index = (_page_no >> 12) & 0x3FF;
    unsigned long *page_Table = (unsigned long *)(0xFFC00000|(dir_index<<12));
    unsigned long frame_num = page_Table[pt_index];
    process_mem_pool->release_frames(frame_num);

    Console::puts("freed page\n");
}






