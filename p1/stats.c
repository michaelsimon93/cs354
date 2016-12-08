/*Assignment 1: Averagen the Integers
 * by Zach Myers, section 2
 * by Michael Simon, section 2 
 *
 * This program reads integers from a file and places them into an array.
 * Then the program will calculate the average of the integers and print
 * out the Average and how many numbers where averaged.
 */
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* main inputs integers from a given file from the arguments into an array 
 * 	and computes the average and the amount of numbers averaged.
 * 	Returns 0 when the function has correctly computed average. 
 *
 *Parameters:
 * argc: The number of white space seperated strings on command line.
 * argv[]: Array of pointers to characters.
 */
int main(int argc, char *argv[])
{
	/*Decalartion of Variables*/
	int fd, filesize, itr, *numarray;
	float sum, average;
	char *numfile;	
	struct stat stbuf; /*Used to hold file information*/	

	/*Checks the input arguments to make sure a file was given*/
	if(argc != 2) {
		fprintf(stderr, "usage: stats <file>\n");
		exit(1);
	}
	
	numfile = argv[1];
	fd = open(numfile, O_RDONLY);
	assert(fd > -1); /* exits if open() failed*/

	fstat(fd, &stbuf);/*Gets the file information*/

	filesize = stbuf.st_size;
	numarray = malloc(filesize);	

	/*Reads the file, puts the numbers into numarray, checks for error*/
	if(read(fd, numarray, filesize) == -1){	
		fprintf(stderr, "error reading file -- quiting");
			exit(1);	
	}
	
	/*Closes the file, checks for error closing*/
	if(close(fd) == -1){
		fprintf(stderr, "error closing file -- quiting");
		exit(1);
	}
	
	sum = 0;
	filesize = filesize/sizeof(int);/*need to divide by ints byte size*/
	/*computes the sum of all the numbers*/
	for(itr = 0; itr < filesize; itr++){
		sum += *numarray;
		numarray++;
	}	
	
	
	average = sum/(itr);/*computes the average of the numbers*/

	printf("Average = %f \nNumber Count = %d\n", average, itr);
	
	return 0;
}
