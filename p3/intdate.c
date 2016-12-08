/* 
 * Assignment3 Periodic Alram 
 * by Zach Myers and Michael Simon section 2
 *
 * This program will execute an interrupt that sounds an alarm.
 * The alarm will sound every three seconds until the user 
 * inputs the ^C (control + C) five times. On the fifth entry
 * the program with nicely exit.
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h> 
#include <unistd.h>

/* Global variable for seconds between time displays*/
int WAIT;
/* Global variable to keep track of quit signals*/
int NUMSIGS;

/* 
 * timePrint is a handler for the SIGALRM interrupt that will print 
 *   the currnet time. This handler will also reset the alarm with 
 *   the correct wait time in seconds.
 * Parameters: NONE
 */
void timePrint(){
	/* Creates a variable to hold the time */
	time_t timeCurr;
	
	/* Gets the current time */
	time(&timeCurr);
	/* Prints the time in the correct format and rests the alarm */
	printf("current time is %s", ctime(&timeCurr));
	alarm(WAIT);	

}

/* 
 * quitHelper is a handler for the SIGINT interrupt that is called 
 *   every time the user inputs the ^C in the command line. This 
 *   handler will test the NUMSIGS global variable for the fifth
 *   time that this interrupt was called and exit the program.
 * Parameters: NONE
 */
void quitHelper(){
	/* Tests NUMSIGS to see if the user has used quit command 5 times */
	if(NUMSIGS > 0){
		printf("\nConrtol-C caught. %d more before program is ended.\n", 
NUMSIGS);
		NUMSIGS--;
	
	}else{
		/* Will print a message and exit the program nicely */
		printf("\nFinal Control-c caught. Exiting.\n");
		exit(0);
	}
}

/* 
 * main starts the program. Run with shell% ./intdate
 * Parametes: NONE
 * Return: 0 if the main is successfully completed
 */
int main(){
	/* Declares variables needed */
	int sigErrorCheck;
	struct sigaction alarmInt;
	struct sigaction controlCInt;	

	/* Intializes Global variables */
	WAIT = 3;
	NUMSIGS = 4;
	
	/* Prompts the user how to exit the program */
	printf("Date will be printed every 3 seconds.");
	printf("\nEnter ^C to end the program:\n");

	/* Sets the handler of the alarm to our made handler */
	alarmInt.sa_handler = timePrint;
	sigErrorCheck = sigaction(SIGALRM, &alarmInt, NULL);

	/* Checks if the signal action was correctly set */
	if(sigErrorCheck != 0){
		fprintf(stderr, "Error could not set signal action.\n");
		exit(1);
	}

	/* Sets the handler of the ^C interrupt to our made handler */
	controlCInt.sa_handler = quitHelper;
	sigErrorCheck = sigaction(SIGINT, &controlCInt, NULL);

	/* Checks if the signal action was correctly set */
	if(sigErrorCheck != 0){
		fprintf(stderr, "Error could not set signal action.\n");
		exit(1);
	}

	/* Sets the alartm */
	alarm(WAIT);

	while(1){
	/*This loop will get interrupted by the alarm*/
	
	/*The ^C command will also interrupt but continue until 5th time*/
	}
	
	return 0;
}
