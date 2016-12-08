/* a few allocations in multiples of 4 bytes followed by frees */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

/* Tests for the case of NULL ptr and ptr that points outside of 
 * allocated memory 
 * Returns 0 when success */
int main() {
   assert(Mem_Init(4096) == 0);
   void* ptr[4];

   /* Tests that the memory was freed successfully */
   ptr[0] = Mem_Alloc(4);
   assert(Mem_Free(ptr[0]) == 0);
   assert(Mem_Free(ptr[0]) == -1);

   /* create a NULL pointer to use as a test */
   ptr[2] = NULL;

   assert(Mem_Free(ptr[2]) == -1);

   /* This ptr used for try to free memory outside of allocated space */
   ptr[3] = Mem_Alloc(16);

   /* Try to free locaton outside of allocated memory */
   assert(Mem_Free(ptr[3] - 500) == -1);

   return 0;
}
