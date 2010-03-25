// Include headers from OS
#include <joyos.h>

/* MIT 6.270 2010: Spacecats!
 *
 * Introduction to C
 * Steven Herbst (herbst@mit.edu)
 *
 * Exercise 1B
 * You will extend the terminal application to number all
 * of its output messages, and will modify it to echo user
 * input multiple times using a for loop.  Then you'll turn
 * the terminal app into a guess-the-number game.
 *
 * 1B-1 The goal of the 'print' function, below, is to
 * streamline the process of writing text to the terminal.
 * So calling print("testing...") should print something
 * along these lines:
 *
 * > testing
 * [Team1] testing...               (0)
 *
 * Further calls to the function should cause the ID #
 * to increment:
 *
 * > asdf
 * [Team1] asdf                     (1)
 * > 6.270 is awesome
 * [Team1] 6.270 is awesome         (2)
 *
 * Again, you might find it useful to check out the
 * full printf documentation: http://ur1.ca/itml
 *
 * Write the code that accomplishes this task in the body
 * of the 'print' function, and combine it with the code from
 * 1A-2 to echo back user input with an ID #.
 *
 * 1B-2 Modify the echo code such that users first enter text
 * (char*), then a number (uint8_t).  The HappyBoard should
 * print out the text the requested number of times (using a
 * 'for' loop):
 * > testing
 * > 3
 * [Team1] testing...               (0)
 * [Team1] testing...               (1)
 * [Team1] testing...               (2)
 * > foo
 * > 2
 * [Team1] foo                      (3)
 * [Team1] foo                      (4)
 *
 * 1B-3 Write a guess the number game.  You know, something
 * like this:
 * [Team1] Play "Guess the Number?" (0)
 * > y
 * > 100
 * [Team1] Too low.                 (1)
 * > 30000                          (2)
 * [Team1] Too high.                (3)
 * > 1028
 * [Team1] Damn straight.           (4)
 * [Team1] Play again?              (5)
 * > n
 *
 * You'll find this function useful:
 * rand() - return a random integer between 0 and 32767
 * More info here: http://ur1.ca/itna
 *
 * Also, you can use uart_scanf("%c", x) to read a
 * character into the variable x, then test if it's equal
 * to 'y' or 'n', for example, with these comparisons:
 *
 * x == 'y'  // 1 if x is the character 'y', 0 otherwise
 * x == 'n'  // 1 if x is the character 'n', 0 otherwise
*
* Note that characters use single quotes, while strings
* use double quotes.
*/

// Declare the 'print' function.  It returns nothing
// (hence the 'void' keyword), and takes one argument: a
// string (that's what "char*" means).  You will fill in
// the details of the implementation.
void print(char*);

int usetup (void) {
    return 0;
}

int
umain (void) {
    // 1B-1 Echo back user input using the 'print' function,
    // including ID #'s

    // 1B-2 Modify the echo code such that users first enter
    // text (char*), then a number (uint8_t).  The HappyBoard
    // should print out the text the requested number of times
    // (using a 'for' loop)

    // 1B-3 Write a guess-the-number game using the 'print' function
    //

    return 0;
}

void print(char *msg){
    static uint16_t msg_id=0;
    // The 'static' keyword means that 'msg_id'
    // is only initialized once, and holds its value
    // after the function has returned.  Thus, it
    // is created in memory and set to zero when the
    // program starts, not each time the function is called.
    // So if the "implementation..." part of this
    // function were replaced with this bit of code:
    //
    // msg_id++;
    //
    // then successive calls to print would cause
    // 'msg_id' to increment up.
    //
    // print("Hello World!"); // msg_id now equals 1
    // print("Robots in disguise.") // msg_id now equals 2
    //
    // But because msg_id is declared inside the 'print'
    // function, it can only be used inside this function.
    // (Not in umain() or usetup(), or any other function...)

    //
    // fill in your implementation here...
    //

    return;  // optional - functions that don't return anything
    // don't need a return statement.
}
