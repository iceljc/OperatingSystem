/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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

const unsigned int ContFramePool::POOL_MAX_NUM=2;
unsigned int ContFramePool::pool_num=0;
ContFramePool* ContFramePool::pool_manager[POOL_MAX_NUM];

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    
    // assert(_n_frames <= FRAME_SIZE * 2);   // 8*1024 frames
    
    base_frame_no = _base_frame_no;
    nframes = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;
	n_info_frames = _n_info_frames;
    
    // If _info_frame_no is zero then we keep management info in the first
    //frame, else we use the provided frame to keep management info
    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }
    
    // Number of frames must be "fill" the bitmap!
    // assert ((nframes % 8 ) == 0);
    
    
    // Everything ok. Proceed to mark all bits in the bitmap
    for(int i=0; i < _n_frames; i++) {
        bitmap[i] = 0;  // 0->free, 1->allocated, 2->head of sequence
    }
    
    // Mark the frame as being used if it is being used
    if(_info_frame_no == 0) {
		if(_n_info_frames <= 1) {
			bitmap[0] = 2;
			nFreeFrames--;
		}
		else {
        	bitmap[0] = 2;
			for(int i=1; i < _n_info_frames; i++) {
				bitmap[i] = 1;
        		nFreeFrames-=_n_info_frames;
			}
		}
    }

	pool_manager[pool_num] = this;
	pool_num++;    
	

    Console::puts("Frame Pool initialized\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED
    assert(nFreeFrames > 0);
	unsigned int frame_no = base_frame_no;
	
	unsigned int i = 0;
	unsigned int j;
	unsigned int length = 0;

	while (length < _n_frames) {
		
		while ((i < nframes) && (bitmap[i] != 0)) {   // find the first 0
			i++;
		}
	
		j = i;  // j: first 0 position in local seqence
	
		while ((i < nframes) && (bitmap[i] == 0)) {
			i++;
			length = i-j;
			if (length == _n_frames) {
				break;
			}
		}

		if (i >= nframes) {
			return 0;
		}

	}

	frame_no += j;  // frame_no => sequence head location
    
    // Update bitmap
    mark_inaccessible(frame_no, _n_frames);
    
    return (frame_no);  // frame_no => head location

}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Mark all frames in the range as being used.
	assert((_base_frame_no >= base_frame_no) && (_base_frame_no <= base_frame_no + nframes - _n_frames));
	
	unsigned int base_index = _base_frame_no - base_frame_no;
	bitmap[base_index] = 2;
	for (int k = base_index + 1; k < base_index + _n_frames; k++) {
		bitmap[k] = 1;
	}
	nFreeFrames -= _n_frames;

}



void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // check if _first_frame_no is in the current pool
	unsigned int i;
	for(i = 0; i < pool_num; i++) {
		ContFramePool* curPool = pool_manager[i];
		unsigned long cur_base_frame_no = curPool->base_frame_no;
		unsigned long cur_nframes = curPool->nframes;
		if((_first_frame_no >= cur_base_frame_no) && (_first_frame_no < cur_base_frame_no + cur_nframes)) {
			curPool->_release_frames(_first_frame_no);
		}
		else {
			// Console::puts("Searching next frame pool\n");
			continue;
		
		}
	}



}


void ContFramePool::_release_frames(unsigned long _first_frame_no)
{
	unsigned int i = _first_frame_no - base_frame_no;
	unsigned int j;
	unsigned int k;
    
    
	if((_first_frame_no >= info_frame_no) && (_first_frame_no < info_frame_no + n_info_frames)) {
		assert(false);
	}
	else {
        // find the first 2
		while((i < base_frame_no + nframes) && (bitmap[i] != 2)) {
			i++;		
		}
        
		j = i; // local position of the head (2)
        i++;
        
        // find the next element that is not 1
		while((i < base_frame_no + nframes) && (bitmap[i] == 1)) {
			i++;
		}
        
        
		for(k = j; k < i; k++) {
			bitmap[k] = 0;		
		}
		nFreeFrames += (i-j);
	}	
	

}




unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    
	unsigned long n_info_frames;
	n_info_frames = (_n_frames) / FRAME_SIZE + (_n_frames % FRAME_SIZE > 0 ? 1 : 0);

	return n_info_frames;

}





