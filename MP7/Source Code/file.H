/*
     File        : file.H

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Simple File class with sequential read/write operations.
 
*/

#ifndef _FILE_H_
#define _FILE_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define SYSTEM_DISK_SIZE2 10485760
#define BLOCK_SIZE 512
#define SYSTEM_BLOCKS SYSTEM_DISK_SIZE2/BLOCK_SIZE
#define HEADER_SIZE 12

#define FREE 0x0000
#define USED 0xFFFF
#define BLOCK_DATA_SIZE 125

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* class  F i l e   */
/*--------------------------------------------------------------------------*/
class FileSystem;
extern FileSystem* FILE_SYSTEM;

class File  {

friend class FileSystem;
    
private:
    /* -- your file data structures here ... */
    
    /* -- maybe it would be good to have a reference to the file system? */
    // FileSystem* file_system;

    unsigned int inode_block_no;
    unsigned int file_id;
    unsigned int file_size;
    unsigned int* blocks_num;
    unsigned int cur_block;
    unsigned int cur_pos;

    
public:

    char res[20];

    File(/* you may need arguments here; maybe a pointer to the disk block
          containing file management and file allocation data */);
    /* Constructor for the file handle. Set the ’current
     position’ to be at the beginning of the file. */

    File(unsigned int _id);
    
    int Read(unsigned int _n, char * _buf);
    /* Read _n characters from the file starting at the current location and
     copy them in _buf.  Return the number of characters read. 
     Do not read beyond the end of the file. */
    
    void Write(unsigned int _n, const char * _buf);
    /* Write _n characters to the file starting at the current location, 
     if we run past the end of file, 
     we increase the size of the file as needed. */
    
    void Reset();
    /* Set the ’current position’ at the beginning of the file. */
    
    void Rewrite();
    /* Erase the content of the file. Return any freed blocks.
     Note: This function does not delete the file! It just erases its content. */
    
    bool EoF();
    /* Is the current location for the file at the end of the file? */

    bool GetOneBlock();

};

#endif
