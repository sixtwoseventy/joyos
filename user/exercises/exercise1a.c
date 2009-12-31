// Include headers from OS
#include <joyos.h>

/* MIT 6.270 2010: Spacecats!
 * 
 * Introduction to C
 * Steven Herbst (herbst@mit.edu)
 * 
 * Exercise 1A
 * You're going to write a simple application to interact with
 * your robot in a serial terminal.  Here are the goals:
 * 
 * 1A-1 First, get your Happyboard to print out a message over the 
 * serial when it is turned on.  This line of code is already
 * written, just change it to be specific to your team.
 * 
 * 1A-2 Second, write code that will echo back what you type
 * in the serial terminal.  So the result might look something
 * like this:
 *
 * > hello there!
 * hello there!
 * > echo
 * echo
 * 
 * This is the same as Slide 14 from Lecture 1.  You'll have to
 * use scanf, allocate a buffer, and all that good stuff.  But it 
 * will only take three lines.  
 * 
 * The "while(1) { ... }" syntax just means that whatever you 
 * write in between the curly braces will run in a loop.  That is, 
 * as soon as the last instruction executes, the first one runs again.
 * We'll talk more about the loops later this evening.
 * 
 * 1A-3 Third, write a bit of code that keeps a running sum of all
 * the numbers entered (signed integers), and every time another
 * integer is entered, prints out the sum in a formatted string that
 * includes your team number and a timestamp.  Example:
 * 
 * > 3
 * [Team1] 3 (1256953732)
 * > 11
 * [Team1] 14 (1256955420)
 * 
 * If you have trouble for some reason, build up to this by first
 * printing out the sum, by itself, then add the team number, and 
 * finally, the timestamp.
 * 
 * Now, about the timestamp...  you can print out just the time 
 * with this line:
 *
 * uart_printf("%lu\n", get_time());
 * 
 * See if you can modify this to accomplish the task at hand...
 *
 * For a bit more info about printf, including how to
 * print out multiple numbers, check out: http://ur1.ca/itmm
 * Full documentation: http://ur1.ca/itml
 * (or just google "printf"...)
 * 
 * Oh, and by the way, all this text is in a multiline comment.
 * You don't have to delete it; it won't get turned into machine
 * instructions.  In fact, use the same syntax to write your own 
 * comments.
 */

int usetup (void) {
	return 0;
}

int
umain (void) {
	// 1A-1
	// Change this line to print a greeting from your team.
	uart_printf("Hello from Team <your team # here...>!\n");

	// 1A-2
	// Comment out the greeting, and echo back user input
	// You'll have to create a buffer variable to hold the 
	// input string...
	while(1){
		// put your code here
	}

	// 1A-3
	// Comment out the "echo" code, and write code that
	// prints out a running sum of all numbers entered, along
	// with a timestamp.
	while(1){
		// put your code here
	}

	return 0;
}

