/* Connect 5
 * ---------
 * This game is played between two players: either two humans, or one human vs artificial intelligence.
 * The players take turns placing their game pieces on an 8x8 board. To win the game, a player must place
 * 5 of their pieces adjacent to each other in either a row, column, or on a diagonal. A tie is achieved when
 * all of the spaces on the board contain a playing piece, yet none are arranged in the winning format described
 * above. When the game ends due to either a win or a tie, the board lights up and the players are notified of the
 * game's outcome. (i.e, Player 1 Wins!)*/


/*int upButton();
int downButton();
int rightButton();
int leftButton();
int enterButton();*/
//int checkWin();

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TIE 3

int takeTurn(int playerTurn, int lastPos[2], int gameType);// Calls functions which allow the players to place their pieces and checks the game status (win/tie).
void letPlayerPlace(int player, int lastPos[2]);//Allows players to place their pieces on the board.
void letComputerPlace(int player, int lastPos[2]);//Allows AI to place pieces during its turn.
void updateBoard(void);//Loads the newly placed pieces to the board.
int checkBoard(int lastPos[2], int playerTurn);//Checks the board for the game status (win/tie/game still in progress).
void displayWinner(int winner);//Flashes the color of the winning player and alerts the players of their win.
void waitFor(unsigned int secs);//Allows the players time to place their pieces, rather than immediately calling the checkBoard function.

int board[8][8] = {0};//Matrix representing the game board.
int lights[8][8] = {0};//Matrix representing colour outputs of the game board.
int gameDone = 0;//Game Status (0 - Game in Progress, 1 - Game Finished).

int main(void) {
	lights[0][0] = 3;//3 is the cursor
	printf("\nConnect 5\n");
	printf("\nThe goal of this game is to connect five of your pieces, either in a row, column, or diagonal, before your opponent connects five of their pieces.\n");
	printf("\nHow would you like to play?\n");
	printf("1 - Human vs Human\n");
	printf("2 - Human vs Computer\n");

	int gameType = 0;//1 - Human v Human, 2 - Human v Computer
	scanf("%d", &gameType); //gameType is not error checked, since on the actual game it wont need to be.
	printf("\nThis is the game board. The character 3 represents your cursor. To move your cursor during the game, you can use the following commands:\n");
	printf("s - Moves your cursor down 1 space\n");
	printf("w - Moves your cursor up 1 space\n");
	printf("a - Moves your cursor to the left 1 space\n");
	printf("d - Moves your cursor to the right 1 space\n");
	printf("e - Places your piece in the selected spot.\n\n");

	updateBoard();//spawns board in
	int playerTurn = 1;//player 1's turn
	int lastPos[2] = {0};//array containing info about the last piece placed is set to zero (since no places have been placed yet)
	while(!gameDone) {//while the game is still in progress...
		playerTurn = takeTurn(playerTurn, lastPos, gameType);//allow players to take their turns.
	}
	return 0;
}

/* takeTurn
 * ---------
 * This function allows each player to take their turn by placing their piece. After each piece placed, it calls the function
 * "checker" to see if a player has won or if there is a tie.
 * If it's a Type 1 game (Human VS Human), the function alternates between letting player 1 and player 2 place a piece.
 * If it's a Type 2 game (Human VS Computer), the function calls alternates between letting player 1 and the computer place a piece.
 * (Human pieces are placed via the function "letPlayerPlace", whereas AI pieces are placed via the function "letComputerPlace").*/

int takeTurn(int playerTurn, int lastPos[2], int gameType){//allows each player to take their turn
	if(gameType == 1) { //Two Human-Player Game
		printf("\nPlayer %d it is your turn.\n", playerTurn);//Player's Turn
		letPlayerPlace(playerTurn, lastPos);
		updateBoard();
	} else { // Human vs Computer (AI) Game
		if(playerTurn == 1) { //Human Player's Turn
			printf("\nPlayer %d it is your turn.\n", playerTurn);
			letPlayerPlace(playerTurn, lastPos);
			updateBoard();
		} else { //Computer Player's Turn
			printf("\nIt's the computer's turn.\n");
			letComputerPlace(playerTurn, lastPos);
			updateBoard();
		}

	}
	int checker = checkBoard(lastPos, playerTurn);//checks for a win or tie after a piece is placed
	if(checker == 1) {//The current player wins!
		/*for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 8; j++) {
				lights[i][j] = playerTurn;
			}
		}*/
		printf("PLAYER %d WINS\n", playerTurn);
		displayWinner(playerTurn);
		gameDone = 1; //flags the game as finished
	}
	else if(checker == 2) { //It's a tie! (All places on board are filled)
		displayWinner(TIE);
	}
	return playerTurn < 2 ? 2 : 1;
}


/* letPlayerPlace
 * --------------
 * This function allows the current human player to place a piece on any empty board space of their choice.*/

void letPlayerPlace(int player, int lastPos[2]) {
	int cursorX = 0;
	int cursorY = 0;
	int placed = 0;

	//resetTimer();
	while(!placed) {//Waits for the player to place their piece.
		/*if(timer > 750) {//flash cursor while waiting for the current player to place their piece
			lights[cursorX][cursorY] = cursor > 0 ? board[cursorX][cursorY] : cursor;
			cursor = !cursor;
			resetTimer();
		}*/
		char button;
		scanf("%c", &button);
		switch(button) {
			case 's': //Cursor Moved Down
				if(cursorY < 7){
					lights[cursorX][cursorY] = board[cursorX][cursorY];
					lights[cursorX][cursorY+1] = 3;
					cursorY++;
					updateBoard();
				}
				break;
			case 'w': //Cursor Moved Up
				if(cursorY > 0){
					lights[cursorX][cursorY] = board[cursorX][cursorY];
					lights[cursorX][cursorY-1] = 3;
					cursorY--;
					updateBoard();
				}
				break;
			case 'a': //Cursor Moved Left
				if(cursorX > 0){
					lights[cursorX][cursorY] = board[cursorX][cursorY];
					lights[cursorX-1][cursorY] = 3;
					cursorX--;
					updateBoard();
				}
				break;
			case 'd': //Cursor Moved Right
				if(cursorX < 7){
					lights[cursorX][cursorY] = board[cursorX][cursorY];
					lights[cursorX+1][cursorY] = 3;
					cursorX++;
					updateBoard();
				}
				break;
			case 'e': //Piece Placed
				if(board[cursorX][cursorY] == 0) {
					board[cursorX][cursorY] = player;
					lights[cursorX][cursorY] = player;
					lights[0][0] = 3;
					lastPos[0] = cursorX;
					lastPos[1] = cursorY;
					placed = 1;
				}
				break;
			default:
				break;
		}

		/*if(enterButton()) {
			if(board[cursorX][cursorY] == 0) {
				board[cursorX][cursorY] = player;
				lights[cursorX][cursorY] = player;
				placed = 1;
			}
		}
		else if(upButton()) {
			cursor = 0;
			lights[cursorX][cursorY] = board[cursorX][cursorY];
			cursorY++;
		}
		else if(downButton()) {
			cursor = 0;
			lights[cursorX][cursorY] = board[cursorX][cursorY];
			cursorY--;
		}
		else if(leftButton()) {
			cursor = 0;
			lights[cursorX][cursorY] = board[cursorX][cursorY];
			cursorX--;
		}
		else if(downButton()) {
			cursor = 0;
			lights[cursorX][cursorY] = board[cursorX][cursorY];
			cursorX++;
		}*/

	}
	return;
}

/* letComputerPlace
 * ----------------
 * This function allows the AI to place a piece each turn. It does so by assigning a score to each place on the board, where a higher score
 * represents a high desirability to place there*/

void letComputerPlace(int player, int lastPos[2]) {
	//Initialize variables.
	int cursorX = 0;
	int cursorY = 0;
	int placed = 0;

	while (!placed) {
		 cursorX = rand() % 8; //Randomly select an x-coordinate.
		 cursorY = rand() % 8; //Randomly select a y-coordinate.
		 if(board[cursorX][cursorY] == 0) { //Check if the selected coorindates are empty (ie. not already occupied by a game piece (1,2))
			 board[cursorX][cursorY] = player;
			 lights[cursorX][cursorY] = player;
			 lights[0][0] = 3; //Reset cursor.
			 lastPos[0] = cursorX;
			 lastPos[1] = cursorY;
			 placed = 1;
		 }
	}
}

/* updateBoard
 * -----------
 * This function updates the board after each new piece is placed.*/

void updateBoard(void) {
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			printf("%d ", lights[j][i]);
		}
		printf("\n");
	}
}

/* waitFor
 * --------
 * Allows the player to have time to place their piece rather than searching for an immediate input.*/
void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}


/* displayWinner
 * --------------
 * Takes in int winner, which is either 1, or 2 indicating which player has
 * won. Then modifies the array 10 times, so that the LED lights are either on or off,
 * flashing the winner's player colours on the LEDs.*/

void displayWinner(int winner) {
	int array[8][8] = {0,0};

	for (int i  = 0; i < 5; i++) {

		if(i % 2 == 0) { //When i is even.

			for(int j = 0; j < 8; j++) {

				for (int k = 0; k < 8; k++) {
					lights[j][k] = 0;
				}
			}

			//Output array values to arduino.

		} else { // When i is odd.

			for(int j = 0; j < 8; j++) {

				for(int k = 0; k < 8; k++) {
					lights[j][k] = winner;
				}
			}

			//Output array values to arduino.

		}
		printf("\n\n\n\n");
		updateBoard();
		waitFor(2);
	}
}

/* checkBoard
 * --------------
 * After each turn, the checkBoard function scans the board for a win or a tie.
 * A win occurs when five pieces owned by the same player are adjacent, and a
 * tie occurs when all of the board spaces contain pieces yet there is no winner.*/

int checkBoard(int lastPos[2], int player) {
	int win = 0;
	int horizontalInRow = 0;
	int verticalInRow = 0;
	int diagonal1InRow = 0;
	int diagonal2InRow = 0;
	int posCpy[2];
	memcpy(posCpy, lastPos, sizeof(int) * 2);
	while(posCpy[0] >= 0 && board[posCpy[0]][posCpy[1]] == player) {
		horizontalInRow++;
		posCpy[0]--;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	while(posCpy[0] <= 7 && board[posCpy[0]][posCpy[1]] == player) {
		horizontalInRow++;
		posCpy[0]++;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	horizontalInRow--;
	while(posCpy[1] <= 7 && board[posCpy[0]][posCpy[1]] == player) {
		verticalInRow++;
		posCpy[1]++;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	verticalInRow--;
	while(posCpy[1] >= 0 && board[posCpy[0]][posCpy[1]] == player) {
		verticalInRow++;
		posCpy[1]--;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	while(posCpy[0] >= 0 && posCpy[1] >= 0 && board[posCpy[0]][posCpy[1]] == player) {
		diagonal1InRow++;
		posCpy[0]--;
		posCpy[1]--;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	diagonal1InRow--;
	while(posCpy[0] <= 7 && posCpy[1] <= 7 && board[posCpy[0]][posCpy[1]] == player) {
		diagonal1InRow++;
		posCpy[0]++;
		posCpy[1]++;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	while(posCpy[0] >= 0 && posCpy[1] <= 7 && board[posCpy[0]][posCpy[1]] == player) {
		diagonal2InRow++;
		posCpy[0]--;
		posCpy[1]++;
	}
	posCpy[0] = lastPos[0];
	posCpy[1] = lastPos[1];
	diagonal2InRow--;
	while(posCpy[0] <= 7 && posCpy[1] >= 0 && board[posCpy[0]][posCpy[1]] == player) {
		diagonal2InRow++;
		posCpy[0]++;
		posCpy[1]--;
	}
	if(horizontalInRow >=5 || verticalInRow >=5 || diagonal1InRow >=5 || diagonal2InRow >=5) {//If a player has five adjacent pieces, they have won the game!
		win = 1;;
	}
	else {
		win = 0;
	}
	if(win) {
		return 1;
	}
	else {//If there is no win, check for a tie.
		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 8; j++) {
				if(board[i][j] == 0){//if there exists an empty place on the board, then there is no tie and the game is still in progress.
					return 0;
				}

 			}
		}
		return 2;//If there are no empty spaces left on the board, and no win, the game is a tie.
	}
}
