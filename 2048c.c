// https://gist.github.com/justecorruptio/10248923
// https://news.ycombinator.com/item?id=7535666
// https://news.ycombinator.com/item?id=7373566
// https://gist.github.com/justecorruptio/10248923 author's annotation

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int board[16],
	W;
	
#define boardSize 16	

int coordTranslate(int dirn, int row, int col){	// each call rotates the coords by 90 deg.
	return dirn 
				? coordTranslate(dirn-1, col, 3-row) 	// return coord at direction - 1
				: 4*row + col;							// base case - just return coord
}

void shift(int simulate, int dirn){
	for(int row=4; row--;) {
		
		int colReadPosn = 0;
		int lastValueInRow = 0;
		for(int col=0; col<4;) {
			if (colReadPosn<4) {
				// process the next value in the row...
				int P = board[coordTranslate(dirn, row, colReadPosn++)];
				if(lastValueInRow && P) {
					if (!simulate) {
						board[coordTranslate(dirn, row, col)]=lastValueInRow<<(lastValueInRow==P);	// x2 if they have the same value
					};
					col++;
				};
				
				W |= P>>11;	//2^11 = 2048, so sets the bottom bit when 2048 is reached
				// update the lastValueInRow depending on P
				//lastValueInRow = (lastValueInRow == P) ? 0 : MAX(lastValueInRow, P);	// my version, which seems much simpler then all the following...
				lastValueInRow = lastValueInRow
									?P						// it has already been set, so depends on P
										?lastValueInRow-P?lastValueInRow:0	// both set; leave if they're different value, otherwise reset to 0 (they've been merged)
										:lastValueInRow		// P is 0, so it's left at it it's original value
									:P;						// it hasn't been set yet, so it's just P				
			} else {
				// no more values to process in the row
				if (!simulate) {
					board[coordTranslate(dirn, row, col)]=lastValueInRow;
				};
				col++;
				W |= 2*!lastValueInRow;
				lastValueInRow=0;
			}
		}
	}
}

int scanCodeConvert[] = { 2, 3, 1, 0};

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
	puts("\x1b[2J\x1b[H");
	for(int posn=boardSize; posn--; ) {
		printf(board[posn]?"%4d|":"    |", board[posn]);
		if (!(posn%4))
			puts("");
	}
	
	int scanCode;	
	if (W==2) {
		read(0, &scanCode, 3);
		shift( false, scanCodeConvert[(scanCode>>16)%4]);
		T();
	}

}


int main(){
	system("stty cbreak");
	T();
	puts(W&1 ? "WIN" : "LOSE");
	return 0;
}


//[2048]