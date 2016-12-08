/******************************************************************************
 * FILENAME: mem.c
 * AUTHOR:   cherin@cs.wisc.edu <Cherin Joseph>
 * MODIFIED BY: Zach Myers, Section 2, Michael Simon, Section 2
 * DATE:     20 Nov 2013
 * PROVIDES: Contains a set of library functions for memory allocation
 * *****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/* this structure serves as the header for each block */
typedef struct block_hd{
  /* The blocks are maintained as a linked list */
  /* The blocks are ordered in the increasing order of addresses */
  struct block_hd* next;

  /* size of the block is always a multiple of 4 */
  /* ie, last two bits are always zero - can be used to store other information*/
  /* LSB = 0 => free block */
  /* LSB = 1 => allocated/busy block */

  /* So for a free block, the value stored in size_status will be the same as the block size*/
  /* And for an allocated block, the value stored in size_status will be one more than the block size*/

  /* The value stored here does not include the space required to store the header */

  /* Example: */
  /* For a block with a payload of 24 bytes (ie, 24 bytes data + an additional 8 bytes for header) */
  /* If the block is allocated, size_status should be set to 25, not 24!, not 23! not 32! not 33!, not 31! */
  /* If the block is free, size_status should be set to 24, not 25!, not 23! not 32! not 33!, not 31! */
  int size_status;

}block_header;

/* Global variable - This will always point to the first block */
/* ie, the block with the lowest address */
block_header* list_head = NULL;


/* Function used to Initialize the memory allocator */
/* Not intended to be called more than once by a program */
/* Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated */
/* Returns 0 on success and -1 on failure */
int Mem_Init(int sizeOfRegion)
{
  int pagesize;
  int padsize;
  int fd;
  int alloc_size;
  void* space_ptr;
  static int allocated_once = 0;
  
  if(0 != allocated_once)
  {
    fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
    return -1;
  }
  if(sizeOfRegion <= 0)
  {
    fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
    return -1;
  }

  /* Get the pagesize */
  pagesize = getpagesize();

  /* Calculate padsize as the padding required to round up sizeOfRegio to a multiple of pagesize */
  padsize = sizeOfRegion % pagesize;
  padsize = (pagesize - padsize) % pagesize;

  alloc_size = sizeOfRegion + padsize;

  /* Using mmap to allocate memory */
  fd = open("/dev/zero", O_RDWR);
  if(-1 == fd)
  {
    fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
    return -1;
  }
  space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (MAP_FAILED == space_ptr)
  {
    fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
    allocated_once = 0;
    return -1;
  }
  
  allocated_once = 1;
  
  /* To begin with, there is only one big, free block */
  list_head = (block_header*)space_ptr;
  list_head->next = NULL;
  /* Remember that the 'size' stored in block size excludes the space for the header */
  list_head->size_status = alloc_size - (int)sizeof(block_header);
  
  return 0;
}


/* Function for allocating 'size' bytes. */
/* Returns address of allocated block on success */
/* Returns NULL on failure */
/* Here is what this function should accomplish */
/* - Check for sanity of size - Return NULL when appropriate */
/* - Round up size to a multiple of 4 */
/* - Traverse the list of blocks and allocate the best free block which can accommodate the requested size */
/* -- Also, when allocating a block - split it into two blocks when possible */
/* Tips: Be careful with pointer arithmetic */
void* Mem_Alloc(int size)
{
 
  /* Variables for searching the memory for a block */
  block_header* curBlock;
  block_header* smallestBlock; 
  smallestBlock = NULL;
	 
  /* Checks for size sanity */
  if(size <= 0){
    fprintf(stderr, "Error: Tried to allocated a negative or zero size.");
	return NULL;
  }

  /* Checks to make sure memory has been initialized */
  if(list_head == NULL){
    fprintf(stderr, "Error: Memory was not been Initialized.");
	return NULL;
  }

  if(size % 4 != 0){
    size =  size - (size % 4) + 4;
  }

  curBlock = list_head;
  
  /* Searches memory for the smallest block */
  while(curBlock != NULL){
    if(curBlock->size_status % 2 == 0){
      if(size <= curBlock->size_status){
        if(smallestBlock == NULL){
	  smallestBlock = curBlock;
	}else{
	  if(smallestBlock->size_status > curBlock->size_status){
	    smallestBlock = curBlock;
	  }
	}
      }
    }
    curBlock = curBlock->next;
  }

  /* checking if there was a block found */
  if(smallestBlock == NULL){
    return NULL;
  }

  /* Setting the block to occupied and coalesce */
  if (smallestBlock->size_status - size > 12){
    block_header remainingBlock; 
    
    remainingBlock.next = smallestBlock->next;
    /* need to subtract 8 because of the header */
    remainingBlock.size_status = smallestBlock->size_status - size - 8;

    *(block_header *)((char*)smallestBlock + size + 8) = remainingBlock; 
    /* adding 8 because need to get to the start of the header */
    smallestBlock->next = (block_header *)((char*)smallestBlock + size + 8);
    smallestBlock->size_status = size + 1;

  }else{
   /* This is the case when the remaining space is not large enought for another
   *  block or the block was exactly the right size */
    smallestBlock->size_status = smallestBlock->size_status + 1;  
  } 

  /* again adding 8 to get address from start of header */
  return (void*)((char*)smallestBlock + 8);

}

/* Function for freeing up a previously allocated block */
/* Argument - ptr: Address of the block to be freed up */
/* Returns 0 on success */
/* Returns -1 on failure */
/* Here is what this function should accomplish */
/* - Return -1 if ptr is NULL */
/* - Return -1 if ptr is not pointing to the first byte of a busy block */
/* - Mark the block as free */
/* - Coalesce if one or both of the immediate neighbours are free */
int Mem_Free(void *ptr)
{

  block_header* curBlock;
  block_header* foundBlock;
  block_header* lastBlock;
  /* initializing variables */
  lastBlock = NULL;
  foundBlock = NULL;
  
  /* checks to see if ptr is NULL */
  if(ptr == NULL){
    return -1;
  }

  /* Searches memory for the block and uses the found variable to 
  *  quit the loop if the block is found, also saves the last block
  *  before the found block so it will be used to coalesce later */
  curBlock = list_head;
  while(curBlock != NULL){
    /* Adding 8 to curBlock to get to start of block */
    if((char*) curBlock + 8 == ptr){
       foundBlock = curBlock;
       break;
    }
    lastBlock = curBlock;
    curBlock = curBlock->next;
  }

  /* need to check if a block was found and if it is empty */
  if(foundBlock == NULL){
    return -1;
  }
  if(foundBlock->size_status % 4 == 0){
    return -1;
  }
  
  /* mark the found block as free */ 
  foundBlock->size_status = foundBlock->size_status - 1;
  
  /* check the next block to see if it is free and if it is coalesce */
  if(foundBlock->next != NULL){
    if(foundBlock->next->size_status % 4 == 0){
      /* There are 8 bytes for the header therefore adding 8 */
      foundBlock->size_status = foundBlock->size_status + foundBlock->next->size_status + 8;
      foundBlock->next = foundBlock->next->next;
    }
  } 
  
  /* check the block before to see if it is free and if it is coalesce */
  if(lastBlock != NULL){
    if(lastBlock->size_status % 4 == 0){
      /* There are 8 bytes for the header therefore adding 8 */
      lastBlock->size_status = lastBlock->size_status + foundBlock->size_status + 8;
      lastBlock->next = foundBlock->next;
    }
  }
 
 /* It will only reach this line if blocks were freed successfully */ 
  return 0;
}

/* Function to be used for debug */
/* Prints out a list of all the blocks along with the following information for each block */
/* No.      : Serial number of the block */
/* Status   : free/busy */
/* Begin    : Address of the first useful byte in the block */
/* End      : Address of the last byte in the block */
/* Size     : Size of the block (excluding the` header) */
/* t_Size   : Size of the block (including the header) */
/* t_Begin  : Address of the first byte in the block (this is where the header starts) */
void Mem_Dump()
{
  int counter;
  block_header* current = NULL;
  char* t_Begin = NULL;
  char* Begin = NULL;
  int Size;
  int t_Size;
  char* End = NULL;
  int free_size;
  int busy_size;
  int total_size;
  char status[5];

  free_size = 0;
  busy_size = 0;
  total_size = 0;
  current = list_head;
  counter = 1;
  fprintf(stdout,"************************************Block list***********************************\n");
  fprintf(stdout,"No.\tStatus\tBegin\t\tEnd\t\tSize\tt_Size\tt_Begin\n");
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  while(NULL != current)
  {
    t_Begin = (char*)current;
    Begin = t_Begin + (int)sizeof(block_header);
    Size = current->size_status;
    strcpy(status,"Free");
    if(Size & 1) /*LSB = 1 => busy block*/
    {
      strcpy(status,"Busy");
      Size = Size - 1; /*Minus one for ignoring status in busy block*/
      t_Size = Size + (int)sizeof(block_header);
      busy_size = busy_size + t_Size;
    }
    else
    {
      t_Size = Size + (int)sizeof(block_header);
      free_size = free_size + t_Size;
    }
    End = Begin + Size;
    fprintf(stdout,"%d\t%s\t0x%08lx\t0x%08lx\t%d\t%d\t0x%08lx\n",counter,status,(unsigned long int)Begin,(unsigned long int)End,Size,t_Size,(unsigned long int)t_Begin);
    total_size = total_size + t_Size;
    current = current->next;
    counter = counter + 1;
  }
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  fprintf(stdout,"*********************************************************************************\n");

  fprintf(stdout,"Total busy size = %d\n",busy_size);
  fprintf(stdout,"Total free size = %d\n",free_size);
  fprintf(stdout,"Total size = %d\n",busy_size+free_size);
  fprintf(stdout,"*********************************************************************************\n");
  fflush(stdout);
  return;
}
