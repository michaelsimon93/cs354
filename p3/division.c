/*
 * Assignment3 Do Some Division
 * by Zach Myers, and Michael Simon, section 2
 * 	
 * This program will ask the user for two integers and then will
 * perform a division. It will then out put the quotient and the 
 * remander. Thw program will run until the users uses the 
 * ^C command.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* Declares Global variable for number of completed divisions */
int NUMDIVISION;

/* 
 * quitHelper is a handler for the ^C interrupt. It will print 
 *   the total number of divisions and exit the program nicely 
 * Prameters: NONE
 */
void quitHelper(){
	/* Prints number of divisions and exitst the program */
	printf("\nNumber of Divisions: %d\nExiting.", NUMDIVISION);
	exit(0);	
}

/*
 * divideZeroHelper is a handler for the when the programs trys to 
 *   divide by zero. This handler will explain that there was an 
 *   attempt to divide by zero. It will then print the number of 
 *   divisions and exit the program nicely.
 * Prameters: NONE
 */
void divideZeroHelper(){
	/* Explains the attempt to divide by zero and number of divisions */
	/* Exits the program nicely */
	printf("Attempted to divide by 0.\nNumber of Divisions: %d", NUMDIVISION);
	printf("\nExiting.\n");
	exit(0);
}

/*
 * Starts the program. Run with shell% ./division
 * Parameters: NONE
 * Return: 0 if the program is competled correctly 
 */
int main(){
	/* Declares variables needed in the program */
	char input[100];
	int integer1, integer2;
	int sigErrorCheck;
	struct sigaction zeroIntr;
	struct sigaction exitIntr;

	/* Initializes the Global Variable for number of divisions */
	NUMDIVISION = 0;
	
	/* Sets the ^C interrupt handler to our custom made handler */
	exitIntr.sa_handler = quitHelper;
	sigErrorCheck = sigaction(SIGINT, &exitIntr, NULL);

	/* Checks if the signal action was set */
	if(sigErrorCheck != 0){
		fprintf(stderr, "Error could not set signal action.");
		exit(1);
	}
	
	/* Sets the divide by zero interrupt handler to our made handler */
	zeroIntr.sa_handler = divideZeroHelper;
	sigErrorCheck = sigaction(SIGFPE, &zeroIntr, NULL);

	/* Checks if the signal action was set */
	if(sigErrorCheck != 0){
		fprintf(stderr, "Error could not set signal action.");
		exit(1);
	}

	while(1){
	 	/* The while loop that will continuosly ask user for numbers */
		printf("Enter first integer: ");
		/* Gets the user input for integer 1 and checks if NULL */
		if( fgets(input, sizeof(input), stdin) == NULL ){
			fprintf(stderr, "Error could not get input");
			exit(1);
		}
		
		/* Changes the user input to an integer */
		integer1 =  atoi(input);
		
		printf("Enter second integer: ");
		/* Getst the user input for integer 2 and Check if NULL*/
		if( fgets(input, sizeof(input), stdin) == NULL ){
			fprintf(stderr, "Error could not get input");
			exit(1);
		}
		
		/* Changes the user input to an integer */
		integer2 = atoi(input);
		
		/* Prints the quotient and the remainder */
		printf("%d / %d is %d with a remainder of %d\n", integer1, integer2, 
integer1/integer2, integer1%integer2);
		
		/* Increase the number of divisions counter */
		/*   will only reach this point if correct */
		NUMDIVISION++;
	}

	return 0;
}
