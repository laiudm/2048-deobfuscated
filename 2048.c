// https://gist.github.com/justecorruptio/10248923
// https://news.ycombinator.com/item?id=7535666
// https://news.ycombinator.com/item?id=7373566
// https://gist.github.com/justecorruptio/10248923 author's annotation

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// fun debug technique - redirect stderr messages to another screen:
// 1. find the device name of the other screen by typing "tty". It will o/p eg /dev/pts/1
// 2. on cmd line redirect any error output to this device - eg ./kilo 2>/dev/pts/1
// 3. write debug messages to stderr, eg by using the debug macro below

// following inspired by http://stackoverflow.com/questions/3576396/variadic-macros-with-0-arguments-in-c99
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define debug(...) fprintf(stderr, "\nDEBUG " __FILE__ ":" TOSTRING(__LINE__) ": " __VA_ARGS__);

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define boardSize 16

// All thinking about the logic can be done using this layout and assuming entry is a right arrow
// board is arranged (row, col) 
// 15 14 13 12		(3,3) (3,2) (3,1) (3,0)
// 11 10  9  8		(2,3) (2,2) (2,1) (2,0)
//  7  6  5  4		(1,3) (1,2) (1,1) (1,0)
//  3  2  1  0		(0,3) (0,2) (0,1) (0,0)

int board[boardSize];
int	W;
int keyCount;	// just for debugging

// 90 deg rotation. It's a rotate to the right
// (0,0) -> (0,3)
// (0,3) -> (3,3)
// (3,3) -> (3,0)
// (3,0) -> (0,0)

int coordTranslate(int dirn, int row, int col){	// translate to abs. coords
	return dirn 
				? coordTranslate(dirn-1, col, 3-row) 	// rotate by 90 deg & return coord at direction - 1
				: 4*row + col;							// base case: just return coord
}

void shift(int simulate, int dirn){
	for(int row=4; row--;) {
		
		int colReadPosn = 0;
		int lastValueInRow = 0;
		for(int colWritePosn=0; colWritePosn<4;) {	// start at the RHS of the row (for a 'right-arrow' operation)
			if (colReadPosn<4) {
				// process the next value in the row...
				int P = board[coordTranslate(dirn, row, colReadPosn++)];
				if(lastValueInRow && P) {
					if (!simulate) {
						board[coordTranslate(dirn, row, colWritePosn)]=lastValueInRow<<(lastValueInRow==P);	// x2 if they have the same value
					};
					colWritePosn++;
				};
				
				W |= P>>11;	//2^11 = 2048, so sets the bottom bit when 2048 is reached
				// update the lastValueInRow depending on P
				lastValueInRow = lastValueInRow
									?P						// it has already been set, so depends on P
										?lastValueInRow-P?P:0	// both set; update if they're different, otherwise reset to 0 (they were the same and were merged)
										:lastValueInRow		// P is 0, so it's left at it it's original value
									:P;						// it hasn't been set yet, so it's just P				
			} else {
				// no more values to process in the row
				//debug("%i row = %i, colWritePosn = %i, lastValueInRow = %i\n", keyCount, row, colWritePosn, lastValueInRow);
				if (!simulate) {
					board[coordTranslate(dirn, row, colWritePosn)]=lastValueInRow;
				};
				colWritePosn++;
				W |= 2*!lastValueInRow;	// sets the 2nd bit if the lastValueInRow is empty. So the bit is set if there's one more more empty last positions. Nice
				lastValueInRow=0;
			}
		}
	}
}

int scanCodeConvert[] = { 2, 3, 1, 0};		// 0 = right, 1 = down, 2 = left, 3 = up (each a 90 deg rotation)

void T(void){
	
	// find & fill an empty slot, starting at a random place
	int i;
	for(i=boardSize+rand()%boardSize; board[i%boardSize]*i; i--);
	if (i)
		board[i%boardSize] = 2<<rand()%2;
	
	// see if a valid move exists; W will be set accordingly
	W = 0;
	for(int dirn=0; dirn<4;)
		shift( true, dirn++);
	
	// display board
	// comment out the next line for seq. board positions to be displayed
	puts("\x1b[2J\x1b[H");		// Refer to https://vt100.net/docs/vt100-ug/chapter3.html#S3.3 "esc[2J" = erase all the display; "esc[H" = direct cursor addressing 
	//printf("\n%i W=%i\n", keyCount++, W);
	for(int posn=boardSize; posn--; ) {
		printf(board[posn]?"%4d|":"    |", board[posn]);
		if (posn%4 == 0)
			puts("");
	}
	
	int scanCode;	
	if (W==2) {
		// we haven't won yet
		read(0, &scanCode, 3);
		//debug("scanCode: %i\n", scanCodeConvert[(scanCode>>16)%4]);
		shift( false, scanCodeConvert[(scanCode>>16)%4]);
		T();
	}

}


int main(){
	system("stty cbreak");	// Set tty mode to not wait for enter key
	T();
	puts(W&1 ? "WIN" : "LOSE");
	return 0;
}


//[2048]