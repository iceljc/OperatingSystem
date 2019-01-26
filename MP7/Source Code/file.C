/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File() {
    /* We will need some arguments for the constructor, maybe pointer to disk
     block with file management and allocation data. */
    Console::puts("In file constructor.\n");
    file_id = 0;
    file_size = 0;
    cur_block = 0;
    cur_pos = 0;
    blocks_num = NULL;
}

File::File(unsigned int _id) {
    file_id = _id;
    if (FILE_SYSTEM->FoundFile(file_id, this)) {
        Console::puts("Found file ! \n");
    } else if (FILE_SYSTEM->CreateFile(file_id)) {
        cur_block = 0;
        cur_pos = 0;
        file_size = 0;
        blocks_num = NULL;
    } else {
        Console::puts("Error ! Cannot Create File !! \n");
    }

} 

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char * _buf) {
    Console::puts("reading from file\n");
    unsigned int N = _n;

    for(int i = 0 ; i < 20 ; ++i){
         _buf[i] = res[i];
    }
    return 20;


    // while (N > 0) {
    //     FILE_SYSTEM->disk->read(blocks_num[cur_block], disk_buffer);
    //     for (cur_pos; cur_pos<(BLOCK_SIZE - HEADER_SIZE); ++cur_pos) {
    //         if (N == 1)
    //             break;
            
    //         memcpy(_buf, disk_buffer + HEADER_SIZE + cur_pos, 1);
    //         _buff++;
    //         N--;

    //         if (cur_pos == (BLOCK_SIZE - HEADER_SIZE)) {
    //             cur_pos = 0;
    //             cur_block++;
    //         }

    //     }
    // }
    // return (_n - N);
    
}


void File::Write(unsigned int _n, const char * _buf) {
    Console::puts("writing to file\n");
    unsigned int N = _n;

    for(int i = 0 ; i < 20 ; ++i){
        res[i] = _buf[i];
    }
    return;
    
    // while (BLOCK_SIZE-HEADER_SIZE<=NULL){
    //     if (EoF())
    //         GetOneBlock();
            
    //     memcpy((void*)(disk_buffer+HEADER_SIZE),_buf,(BLOCK_SIZE-HEADER_SIZE));//copy from user buffer to file buffer
    //     FILE_SYSTEM->disk->write(blocks_num[cur_block],(unsigned char*)disk_buffer);
    //     N-=(BLOCK_SIZE-HEADER_SIZE);
    // }

    
    
}

void File::Reset() {
    Console::puts("reset current position in file\n");
    cur_pos = 0;
    cur_block = 0;
    
    
}

void File::Rewrite() {
    Console::puts("erase content of file\n");
    cur_block = 0;
    Console::puts("file size = "); Console::puti(file_size); Console::puts("\n");
    while (cur_block < file_size) {
        FILE_SYSTEM->DeallocateOneBlock(blocks_num[cur_block]);
        Console::puti(cur_block); Console::puts("\n");
        cur_block++;
    }
    cur_block = 0;
    cur_pos = 0;
    blocks_num = NULL;
    file_size = 0;
    
}


bool File::EoF() {
    Console::puts("testing end-of-file condition\n");
    if (blocks_num == NULL) {
        return true;
    }
    if ((cur_block == file_size - 1) && (cur_pos == BLOCK_SIZE - HEADER_SIZE - 1)) {
        return true;
    } else {
        return false;
    }
    
}

bool File::GetOneBlock() {
    unsigned int new_block_no = FILE_SYSTEM->AllocateOneBlock();
    unsigned int* new_num_array = (unsigned int*) new unsigned int[file_size+1];
    for (int i=0; i<file_size; ++i) {
        new_num_array[i] = blocks_num[i];
    }
    if (blocks_num != NULL) {
        new_num_array[file_size] = new_block_no;
    } else {
        new_num_array[0] = new_block_no;
    }
    file_size++;
    delete blocks_num;
    blocks_num = new_num_array;
    return true;
}













