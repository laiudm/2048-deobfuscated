// https://gist.github.com/justecorruptio/10248923
// https://news.ycombinator.com/item?id=7535666
// https://news.ycombinator.com/item?id=7373566
// https://gist.github.com/justecorruptio/10248923 author's annotation

#include <stdio.h>
int board[16],
	W,
	k;
	
#define boardSize 16	

main(){
	system("stty cbreak");
	T();
	puts(W&1 ? "WIN" : "LOSE");
}
int scanCodeConvert[] = { 2, 3, 1, 0};

shift(int simulate, int dirn){
	int i,j,l,P;
	for(i=4;i--;)
		for(j=k=l=0;k<4;) {
			if (j<4) {
				P=board[coordTranslate(dirn, i, j++)];
				W|=P>>11;
				l*P&&(simulate?board[coordTranslate(dirn, i, k)]=l<<(l==P):0,k++);
				l=l?
					P?
						l-P?P:0
					:l
				:P;
			} else {
				if (simulate) {
					board[coordTranslate(dirn, i, k)]=l;
				};
				++k;
				W|=2*!l;
				l=0;
			}
		}
}

coordTranslate(int dirn, int row, int col){
	return dirn 
				? coordTranslate(dirn-1, col, 3-row) 	// return coord at direction - 1
				: 4*row + col;							// base case - just return coord
}

T(void){
	int i;
	
	// find & fill an empty slot, starting at a random place
	for(i=boardSize+rand()%boardSize; board[i%boardSize]*i; i--);
	if (i)
		board[i%boardSize] = 2<<rand()%2;
	
	// see if a valid move exists; W will be set accordingly
	W = 0;
	for(i=0; i<4;)
		shift( 0, i++);
	
	// display board
	puts("\e[2J\e[H");
	for(i=boardSize; i--; ) {
		printf(board[i]?"%4d|":"    |", board[i]);
		if (!(i%4))
			puts("");
	}
	
	int scanCode;	
	if (W=2) {
		read(0, &scanCode, 3);
		shift( 1, scanCodeConvert[(scanCode>>16)%4]);
		T();
	}

}//[2048]