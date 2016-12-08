/* Assignment4 2-Dimensional Array
 * by Zach Myers, section 2
 *    Michael Simon, section 2
 *
 * This program plays creates a two dimensional array 
 * as a global variable. In this verison the inner 
 * loop works through a single row and the outer 
 * loop iterats through the rows. It then will intialize
 * each element with the value of the index. 
 */

#include <stdlib.h>

int arr[3000][500];


int main() {
	
	int row;
	int col;
	for(row = 0; row < 3000; row++){
		for(col = 0; col < 500; col++){
			arr[row][col] = row + col;
		}
	}

	return 0;
}

