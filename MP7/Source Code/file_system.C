/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
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
#include "file_system.H"


/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    block_no = 0;
    files_num = 0;
    files = NULL;
    memset(disk_buffer, 0, BLOCK_SIZE);

}

/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/

void FileSystem::AddOneFile(File * newFile) {
    if (files == NULL) {
        files = newFile;
    } else {
        File* new_file_list = (File*) new File[files_num + 1];
        for (int i=0; i<files_num; ++i) {
            new_file_list[i] = files[i];
        }
        new_file_list[files_num+1] = *newFile;
        files_num++;
        delete files;
        files = new_file_list;
    }
}


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system form disk\n");
    disk = _disk;
    // disk->read(0, disk_buffer);
    files_num = block->size;
    for (int i=0; i<files_num; ++i) {
        disk->read(0, disk_buffer);
        File* newFile = new File();
        disk->read(block->data[i], disk_buffer); //puts file inode in buffer
        newFile->file_size = block->size;
        newFile->file_id = block->id;
        AddOneFile(newFile);
    }
    return true;
    
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) {
    Console::puts("formatting disk\n");
    FILE_SYSTEM->SetDisk(_disk);
    memset(disk_buffer, 0, BLOCK_SIZE);

    for (int i=0; i<SYSTEM_BLOCKS; ++i) {
        _disk->write(i, disk_buffer);
    }
    block->available = USED;
    block->size = 0;
    _disk->write(0, disk_buffer);
    
    return true;
    
}

File * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file\n");
    for (int i=0; i<files_num+1; ++i) {
        if (files[i].file_id == _file_id || files[i].file_id+1 == _file_id) {
            Console::puts("file found !! \n");
            return &files[i];
        }
    }
    return NULL;
    
}


bool FileSystem::FoundFile(int _file_id, File * _file) {
    for (int i=0; i<files_num+1; ++i) {
        if (files[i].file_id == _file_id) {
            *_file=files[i];
            return true;
        }
    }
    return false;
}




bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file\n");
    File* newFile = (File*) new File();
    if (FoundFile(_file_id, newFile)) {
        return false;
    }

    newFile->file_id = _file_id;
    newFile->file_size = 0;
    newFile->blocks_num = NULL;
    newFile->Rewrite();

    newFile->inode_block_no = AllocateOneBlock();
    disk->read(newFile->inode_block_no, disk_buffer);
    block->available = USED;
    block->size = 0;
    block->id = _file_id;
    disk->write(newFile->inode_block_no, disk_buffer);
    AddOneFile(newFile);
    return true;

    
}



bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file\n");
    File* new_file_list = (File*) new File[files_num];
    bool found = false;
    for (int i=0; i<files_num; ++i) {
        if (files[i].file_id == _file_id){
            found = true;
            files[i].Rewrite();
            DeallocateOneBlock(files[i].inode_block_no); //deletes inode of file
        }
        if (found) {
            new_file_list[i] = files[i+1];
        } else {
            new_file_list[i] = files[i];
            files_num--;
        }
    }
    delete files;
    files = new_file_list;
    if (files_num == 0) {
        files = NULL;
    }
    return found;

}


int FileSystem::AllocateOneBlock() {
    disk->read(block_no, disk_buffer);
    int check = 0;
    while (block->available==USED){
        if (block_no > (SYSTEM_BLOCKS-1)) {
            block_no = 0;
            check++;
            if (check > 1) {
                Console::puts("ERROR NO FREE BLOCKS!!!!\n");
                return 0;
            }
        }
        block_no++;
        disk->read(block_no, disk_buffer);
    }
    disk->read(block_no, disk_buffer);
    block->available = USED;
    disk->write(block_no, disk_buffer);
    return block_no;
}


 void FileSystem::DeallocateOneBlock(unsigned int _block_no) {
    Console::puts("deallocating start ......\n");
    disk->read(_block_no, disk_buffer);
    block->available = FREE;
    disk->write(_block_no, disk_buffer);
    Console::puts("deallocating end ......\n");

 }















