/* Assignment4 1-Dimensional Array
 * by Zach Myers, section 2
 *    Michael Simon, section 2
 *
 * This program plays creates a one dimensional array 
 * as a global variable. It then will intialize each element 
 * with the value of the index. 
 */

#include <stdlib.h>

int arr[100000];


int main() {
	
	int i;
	for(i = 0; i < 100000; i++){
		arr[i] = i;
	}

	return 0;
}

