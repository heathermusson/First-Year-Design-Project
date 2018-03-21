#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUMPIXELS 64
#define BUTTON_1 12
#define BUTTON_2 11
#define BUTTON_3 10

int gameType = 1;
int board[8][8] = {0}; //game data
int lights[8][8] = {0}; //data used to control LEDs
int botBoard[8][8] = {0}; //temp board used for bot evaluation
int gameDone = 0;
int playerTurn = 1;
int lastPos[2] = {0};
int cursors = 0; //cursor toggle boolean
int cursorX = 0;
int cursorY = 0;
bool buttonMode = false; //determines whether buttons change rows or columns 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //declare LED matrix

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_1, INPUT); //declare pins and open serial port for debugging
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  Serial.begin(9600);
  
  pixels.begin(); //initialize LED matrix
  pixels.show();
  
  lights[0][0] = 3; //initialize cursor
  setGameType(); //choose whether a 1 or 2 player game will be played
  updateBoard(); 

  //THE REST OF SETUP IS SETTING UP A 1 SECOND SYSTEM INTERRUPT FOR THE CURSOR
  cli();
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
  //end of system interrupt setup
}

void loop() {
  // put your main code here, to run repeatedly:
  while(!gameDone) {
    playerTurn = takeTurn(playerTurn, lastPos, gameType);
  }
}

ISR(TIMER1_COMPA_vect) { //system interrupt that runs once per second to control cursor blinking
  if(!gameDone){
    if(gameType == 1 || playerTurn == 1) {
      cursors = !cursors;
      lights[cursorX][cursorY] = cursors > 0 ? board[cursorX][cursorY] : 3;
      updateBoard();
    }
  }
}

int takeTurn(int playerTurn, int lastPos[2], int gameType){
  if(gameType == 1) { //Two player game
    //printf("\nPlayer %d it is your turn.\n", playerTurn);
    letPlayerPlace(playerTurn, lastPos); //let player update the board with their move
    updateBoard(); //draw the board state
  } 
  else { //gametype == 2, human vs computer game
    if(playerTurn == 1) { //Human player
      //printf("\nPlayer %d it is your turn.\n", playerTurn);
      letPlayerPlace(playerTurn, lastPos);
      updateBoard();
    } 
    else { //Computer player
      //printf("\nIt's the computer's turn.\n");
      letComputerPlace(playerTurn, lastPos);
      updateBoard();
    }

  }
  int checker = checkBoard(lastPos, playerTurn);
  if(checker == 1) {
    //printf("PLAYER %d WINS\n", playerTurn);
    gameDone = 1;
    displayWinner(playerTurn);
    gameDone = 1;
  }
  else if(checker == 2) {
    displayWinner(0);
  }
  return playerTurn < 2 ? 2 : 1; //update the game to be the other player's turn
}

void letPlayerPlace(int player, int lastPos[2]) {
  /* Allows the player to place a piece wherever there is an empty space.
  **
  */
  int placed = 0;
  cursorX = 0;
  cursorY = 0;
  buttonMode = false; //reset buttonMode

  while(!placed) { //protects against invalid input
    char button = checkButton();
    switch(button) {
      case 's': //this case moved the cursor down one
        cursors = 0;
        TCNT1  = 0;//initialize timer value to 0
        if(cursorY < 7){
          lights[cursorX][cursorY] = board[cursorX][cursorY];
          lights[cursorX][cursorY+1] = 3;
          cursorY++;
          updateBoard();
        }
        break;
      case 'w': //moves cursor up one
        cursors = 0;
        TCNT1  = 0;//initialize counter value to 0
        if(cursorY > 0){
          lights[cursorX][cursorY] = board[cursorX][cursorY];
          lights[cursorX][cursorY-1] = 3;
          cursorY--;
          updateBoard();
        }
        break;
      case 'a': //moves cursor left
        cursors = 0;
        TCNT1  = 0;//initialize counter value to 0
        if(cursorX > 0){
          lights[cursorX][cursorY] = board[cursorX][cursorY];
          lights[cursorX-1][cursorY] = 3;
          cursorX--;
          updateBoard();
        }
        break;
      case 'd': //moves cursor right
        cursors = 0;
        TCNT1  = 0;//initialize counter value to 0
        if(cursorX < 7){
          lights[cursorX][cursorY] = board[cursorX][cursorY];
          lights[cursorX+1][cursorY] = 3;
          cursorX++;
          updateBoard();
        }
        break;
      case 'e': //places piece in the current spot if valid
        if(board[cursorX][cursorY] == 0) {
          board[cursorX][cursorY] = player;
          botBoard[cursorX][cursorY] = player;
          lights[cursorX][cursorY] = player;
          lights[0][0] = 3;
          lastPos[0] = cursorX;
          lastPos[1] = cursorY;
          placed = 1;
          delay(100);
        }
        break;
      default:
        break;
    }
  }
  return;
}

/* The bot is able to evaluate the board and give a score for different situations. It evaluates in two ways, the first is by possible moves, the second is by possible enemy moves.
 *  This allows the bot to catch tricky setups by the enemy.
*/
void letComputerPlace(int player, int lastPos[2]) { 
  int enemy = 1;
  int a = 0;
  int b = 0;
  while(board[a][b] != 0) { //get starting free space
    if(a < 7) {
      b++;
    }
    else {
      a = 0;
      b++;
    }
  }
  botBoard[a][b] = player;
  long maxEval = evaluate(); //get initial evaluation as a comparison baseline
  botBoard[a][b] = 0;
  int maxX = a;
  int maxY = b;
  for(int i = 0; i < 8; i++) { //evaluate what the board score is for each possible move
    for(int j = 0; j < 8; j++) {
      if(board[i][j] == 0) {
        botBoard[i][j] = 2;
        long currentEval = evaluate();
        botBoard[i][j] = 0;
        
        if(currentEval > maxEval) {
          maxEval = currentEval;
          maxX = i;
          maxY = j;
        }
      }
    }
  }
  a = 0;
  b = 0;
  while(board[a][b] != 0) {
    if(a < 7) {
      b++;
    }
    else {
      a = 0;
      b++;
    }
  }
  botBoard[a][b] = enemy;
  long minEval = evaluate();
  botBoard[a][b] = 0;
  int minX = a;
  int minY = b;
  for(int i = 0; i < 8; i++) { //repeat the evaluation proccess except using the OTHER player's moves to find tricky blocks
    for(int j = 0; j < 8; j++) {
      if(board[i][j] == 0) {
        botBoard[i][j] = 1;
        long currentEval = evaluate();
        botBoard[i][j] = 0;
        
        if(currentEval < minEval) {
          minEval = currentEval;
          minX = i;
          minY = j;
        }
      }
    }
  }
  if(minEval * -1 > maxEval) { //determine which evaluation method should be used
    lastPos[0] = minX;
    lastPos[1] = minY;
    
    board[minX][minY] = player;
    botBoard[minX][minY] = player;
    lights[minX][minY] = player;
  }
  else {
    lastPos[0] = maxX;
    lastPos[1] = maxY;
    
    board[maxX][maxY] = player;
    botBoard[maxX][maxY] = player;
    lights[maxX][maxY] = player;
  }
}

int checkBoard(int lastPos[2], int player) { //check if there is 5 in a row anywhere on the board. This code looks gross, but is quite efficient because it only scans around the most recent move.
  int win = 0;
  int horizontalInRow = 0; //declare counters
  int verticalInRow = 0;
  int diagonal1InRow = 0;
  int diagonal2InRow = 0;
  int posCpy[2]; //variable to track where we were counting from
  memcpy(posCpy, lastPos, sizeof(int) * 2);

  //The reason there are so many increments is to stop it from counting the starting position and end position twice.
  while(posCpy[0] >= 0 && board[posCpy[0]][posCpy[1]] == player) {
    horizontalInRow++;
    posCpy[0]--;
  }
  
  posCpy[0] = lastPos[0]; //whenever this code comes up, it is to move the count cursor back to the starting position
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
  
  if(horizontalInRow >=5 || verticalInRow >=5 || diagonal1InRow >=5 || diagonal2InRow >=5) {
    win = 1;
  }
  else {
    win = 0;
  }
  if(win) {
    return 1;
  }
  else { //check if the board is full for the 'draw' event
    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        if(board[i][j] == 0){
          return 0;
        }

      }
    }
    return 2;
  }
}

void displayWinner(int winner) { //this function flashes lights in the winner's color
  int array[8][8] = {0,0};

  for (int i  = 0; i < 15; i++) {

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
    updateBoard();
    delay(500);
  }

}


char checkButton(){ //determines which buttons are being pressed. If both direction buttons are pressed, the direction axis switches
  while(true){
    if(buttonMode) {
      if(digitalRead(BUTTON_3) == HIGH) { //LOW is the state for pushed button
        return 'e';
      }
      if(digitalRead(BUTTON_1) == HIGH){
        delay(100); //delays to ensure both can be pressed together within reasonable time
        if(digitalRead(BUTTON_2) == HIGH) {
          buttonMode = !buttonMode;
          delay(100);
        }
        else {
          return 'a';
        }
      }
      if(digitalRead(BUTTON_2) == HIGH) {
        delay(100);
        if(digitalRead(BUTTON_1) == HIGH) {
          buttonMode = !buttonMode;
          delay(100);
        }
        else {
          return 'd';
        }
      }
    }
    else {
      if(digitalRead(BUTTON_3) == HIGH) {
        return 'e';
      }
      if(digitalRead(BUTTON_1) == HIGH) {
        delay(100);
        if(digitalRead(BUTTON_2) == HIGH) {
          buttonMode = !buttonMode;
          delay(100);
        }
        else {
          return 's';
        }
      }
      if(digitalRead(BUTTON_2) == HIGH) {
        delay(100);
        if(digitalRead(BUTTON_1) == HIGH) {
          buttonMode = !buttonMode;
          delay(100);
        }
        else {
          return 'w';
        }
      }
    }
  }
}

void setGameType(void) { //choose whether pvp or player vs bot game should happen
  while(true) {
    if(digitalRead(BUTTON_1) == HIGH) {
      gameType = 1;
      return;
    }
    if(digitalRead(BUTTON_2) == HIGH) {
      gameType = 2;
      return;
    }
  }
}

void updateBoard(void) { //updates the LED matrix to display the current board state based on the lights matrix
  for(int i=0;i<NUMPIXELS;i++){
    if(i < 8) {
      switch(lights[0][map(i,0,7,0,7)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else if(i < 16) {
      switch(lights[1][map(i,8,15,7,0)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else if(i < 24) {
      switch(lights[2][map(i,16,23,0,7)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else if(i < 32) {
      switch(lights[3][map(i,24,31,7,0)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else if(i < 40) {
      switch(lights[4][map(i,32,39,0,7)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else if(i < 48) {
      switch(lights[5][map(i,40,47,7,0)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else if(i < 56) {
      switch(lights[6][map(i,48,55,0,7)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    else {
      switch(lights[7][map(i,56,63,7,0)]) {
        case 0:
          pixels.setPixelColor(i, pixels.Color(0,0,0));
          break;
        case 1:
          pixels.setPixelColor(i, pixels.Color(50,0,0));
          break;
        case 2:
          pixels.setPixelColor(i, pixels.Color(0,50,0));
          break;
        case 3:
          pixels.setPixelColor(i, pixels.Color(0,0,50));
          break;
      }
    }
    pixels.show();
  }
}

/* Using these events scores:
1. 5 In a row (50000 points)
2. 4 in a row completely unblocked (500 points)
3. 4 in a row half blocked (125 points)
4. 3 in a row completely unblocked (150 points)
5. 3 in a row half blocked (100 points)
6. 2 in a row completely unblocked (50 points)
6.5 2 in a row half blocked (20 points)
7. 1 standalone surrounded by empty space (25 points)

8. Enemy 4 in a row at all unblocked (-25000)
9. Enemy 3 in a row half blocked (-125)
10. Enemy 3 in a row completely unblocked (-20000)
11. Enemy 2 in a row half blocked (-100)
12. Enemy 2 in a row completely unblocked (-150)
*/


/* checkRowBlock
 * ------------------
 * Checks a known row of n pieces to see if which (potentially both) ends are blocked, and returns:
 *  - 1 if one end is blocked
 *  - 2 if two ends are blocked
 *  - 0 is neither end is blocked
 * This function checks for:
 * - A regular block
 * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
 * - If the end of the botBoard is reached. */

int checkRowBlock(int i, int j, int n) {
  /* Checks for:
   * - A regular block
   * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
   * - If the end of the botBoard is reached.
   */

   //Check for reaching the end of the botBoard being a block.
   //Check if one side is unblocked and the other on the edge of the botBoard.
   if(j == 0) { //Checks if the last piece or first piece is on the edge of the botBoard.
     if(botBoard[i][j + n] != 0) {
       return 2;
     }
     return 1;
   }
   if(j + n == 8) { //Checks if the last piece is on the edge of the botBoard.
     if(botBoard[i][j - 1] != 0) {
       return 2;
     }
     return 1;
   }

  //Checks for further blocks
  switch (n) {
    case 4: //ie. there is no further blocks for this case.
    break;

    case 3:
    if(j - 1 > -1 && j + n + 1 < 8) { //Checks for case where 211102
      if(botBoard[i][j - 1] != 0 && botBoard[i][j + n + 1] != 0) {
        return 2;
      }
    }
    if(j - 2 > -1 && j + n < 8) { //Checks for case where 201112
      if(botBoard[i][j - 2] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if(j - 1 > -1 && j + n + 2 < 8) { // Checks for case where 211002
      if(botBoard[i][j - 1] != 0 && botBoard[i][j + n + 2] != 0) {
        return 2;
      }
    }
    if (j - 1 > -1 && j + n + 1 < 8) { //Checks for case where 21102
      if(botBoard[i][j - 1] != 0 && botBoard[i][j + n + 1] != 0) {
        return 2;
      }
    }
    if (j - 3 > -1 && j + n < 8) { //Checks for case where 200112
      if(botBoard[i][j - 3] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    if (j - 2 > -1 && j + n < 8) { //Checks for case where 20112
      if(botBoard[i][j - 2] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if(j - 1 > -1 && j + n + 3 < 8) { //Checks for case where 210002
      if(botBoard[i][j - 1] != 0 && botBoard[i][j + n + 3] != 0) {
        return 2;
      }
    }
    if (j - 1 > -1 && j + n + 2 < 8) { //Checks for case where 21002
      if(botBoard[i][j - 1] != 0 && botBoard[i][j + n + 2] != 0) {
        return 2;
      }
    }
    if (j - 1 > -1 && j + n + 1 < 8) { //Checks for case where 2102
      if(botBoard[i][j - 1] != 0 && botBoard[i][j + n + 1] != 0) {
        return 2;
      }
    }
    if(j - 4 > -1 && j + n < 8) { //Checks for case where 200012
      if(botBoard[i][j - 4] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    if(j - 2 > -1 && j + n < 8) { //Checks for case where 20012
      if(botBoard[i][j - 3] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    if(j - 2 > -1 && j + n < 8) { //Checks for case where 20112
      if(botBoard[i][j - 2] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if((j + n) < 8 && (j - 1) > -1) { //Ensures this does not go over the end of the botBoard.

    if(botBoard[i][j - 1] != 0 && botBoard[i][j + n] != 0) { //Blocked on right and left side.
      return 2;
    }
    if (botBoard[i][j - 1] != 0 || botBoard[i][j + n] != 0) { //Blocked on either the left or right side.
      return 1;
    }
  }


  return 0;
}

/* checkColumnBlock
 * ------------------
 * Checks a known column of n pieces to see if which (potentially both) ends are blocked, and returns:
 *  - 1 if one end is blocked
 *  - 2 if two ends are blocked
 *  - 0 is neither end is blocked
 * This function checks for:
 * - A regular block
 * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
 * - If the end of the botBoard is reached. */

int checkColumnBlock(int i, int j, int n) {
   //Check for reaching the end of the botBoard being a block.
   //Check if one side is unblocked and the other on the edge of the botBoard.
   if(i == 0) { //Checks if the last piece or first piece is on the edge of the botBoard.
     if(botBoard[i + n][j] != 0) {
       return 2;
     }
     return 1;
   }
   if(i + n == 8) { //Checks if the last piece is on the edge of the botBoard.
     if(botBoard[i - 1][j] != 0) {
       return 2;
     }
     return 1;
   }

  //Checks for further blocks
  switch (n) {
    case 4: //ie. there is no further blocks for this case.
    break;

    case 3:
    if(i - 1 > -1 && i + n + 1 < 8) { //Checks for case where 211102
      if(botBoard[i - 1][j] != 0 && botBoard[i + n + 1][j] != 0) {
        return 2;
      }
    }
    if(i - 2 > -1 && i + n < 8) { //Checks for case where 201112
      if(botBoard[i - 2][j] != 0 && botBoard[i + n][j] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if(i - 1 > -1 && i + n + 2 < 8) { // Checks for case where 211002
      if(botBoard[i - 1][j] != 0 && botBoard[i + n + 2][j] != 0) {
        return 2;
      }
    }
    if (i - 1 > -1 && i + n + 1 < 8) { //Checks for case where 21102
      if(botBoard[i - 1][j] != 0 && botBoard[i + n + 1][j] != 0) {
        return 2;
      }
    }
    if (i - 3 > -1 && i + n < 8) { //Checks for case where 200112
      if(botBoard[i - 3][j] != 0 && botBoard[i + n][j] != 0) {
        return 2;
      }
    }
    if (i - 2 > -1 && i + n < 8) { //Checks for case where 20112
      if(botBoard[i - 2][j] != 0 && botBoard[i + n][j] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if(i - 1 > -1 && i + n + 3 < 8) { //Checks for case where 210002
      if(botBoard[i - 1][j] != 0 && botBoard[i + n + 3][j] != 0) {
        return 2;
      }
    }
    if (i - 1 > -1 && i + n + 2 < 8) { //Checks for case where 21002
      if(botBoard[i - 1][j] != 0 && botBoard[i + n + 2][j] != 0) {
        return 2;
      }
    }
    if (i - 1 > -1 && i + n + 1 < 8) { //Checks for case where 2102
      if(botBoard[i - 1][j] != 0 && botBoard[i + n + 1][j] != 0) {
        return 2;
      }
    }
    if(i - 4 > -1 && i + n < 8) { //Checks for case where 200012
      if(botBoard[i - 4][j] != 0 && botBoard[i + n][j] != 0) {
        return 2;
      }
    }
    if(i - 3 > -1 && i + n < 8) { //Checks for case where 20012
      if(botBoard[i - 3][j] != 0 && botBoard[i + n][j] != 0) {
        return 2;
      }
    }
    if(i - 2 > -1 && i + n < 8) { //Checks for case where 20112
      if(botBoard[i][j - 2] != 0 && botBoard[i][j + n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if((i + n) < 8 && (i - 1) > -1) { //Ensures this does not go over the end of the botBoard.
    if(botBoard[i - 1][j] != 0 && botBoard[i + n][j] != 0) { //Blocked on right and left side.

      return 2;
    }
    if (botBoard[i - 1][j] != 0 || botBoard[i + n][j] != 0) { //Blocked on either the left or right side.
      return 1;
    }
  }


  return 0;
}

/* checkRDiagBlock
 * ------------------
 * Checks a known diagonal of n pieces to see if which (potentially both) ends are blocked, and returns:
 *  - 1 if one end is blocked
 *  - 2 if two ends are blocked
 *  - 0 is neither end is blocked
 * This function checks for:
 * - A regular block
 * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
 * - If the end of the botBoard is reached. */

int checkRDiagBlock(int i, int j, int n) {
   //Check for reaching the end of the botBoard being a block.
   //Check if one side is unblocked and the other on the edge of the botBoard.
   if(i == 0 || j == 0) { //Checks if the first piece is on the edge of the botBoard.
     if(botBoard[i + n][j + n] != 0) {
       return 2;
     }
     return 1;
   }
   if(i + n == 8 || j + n == 8) { //Checks if the last piece is on the edge of the botBoard.
     if(botBoard[i - 1][j - 1] != 0) {
       return 2;
     }
     return 1;
   }

  //Checks for further blocks
  switch (n) {
    case 4: //ie. there is no further blocks for this case.
    break;

    case 3:
    if((i - 1 > -1 && j - 1 > -1) && (i + n + 1 < 8 && j + n + 1 < 8)) { //Checks for case where 211102
      if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n + 1][j + n + 1] != 0) {
        return 2;
      }
    }
    if((i - 2 > -1 && j - 2 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 201112
      if(botBoard[i - 2][j - 2] != 0 && botBoard[i + n][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if((i - 1 > -1 && j - 1 > -1) && (i + n + 2 < 8 && j + n + 2 < 8)) { // Checks for case where 211002
      if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n + 2][j + n + 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j - 1 > -1) && (i + n + 1 < 8 && j + n + 1 < 8)) { //Checks for case where 21102
      if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n + 1][j + n + 1] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j - 3 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 200112
      if(botBoard[i - 3][j - 3] != 0 && botBoard[i + n][j + n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j - 2 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 20112
      if(botBoard[i - 2][j - 2] != 0 && botBoard[i + n][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if((i - 1 > -1 && j - 1 > -1) && (i + n + 3 < 8 && j + n + 3 < 8)) { //Checks for case where 210002
      if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n + 3][j + n + 3] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j - 1 > -1) && (i + n + 2 < 8 && j + n + 2 < 8)) { //Checks for case where 21002
      if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n + 2][j + n + 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j - 1 > -1) && (i + n + 1 < 8 && j + n + 1 < 8)) { //Checks for case where 2102
      if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n + 1][j + n + 1] != 0) {
        return 2;
      }
    }
    if ((i - 4 > -1 && j - 4 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 200012
      if(botBoard[i - 4][j - 4] != 0 && botBoard[i + n][j + n] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j - 3 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 20012
      if(botBoard[i - 3][j - 3] != 0 && botBoard[i + n][j + n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j - 2 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 20112
      if(botBoard[i - 2][j - 2] != 0 && botBoard[i + n][j + n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if(((i + n) < 8 && (j + n) < 8) && ((i - 1) > -1 && (j - 1) > -1)) { //Ensures this does not go over the end of the botBoard.
    if(botBoard[i - 1][j - 1] != 0 && botBoard[i + n][j + n] != 0) { //Blocked on right and left side.
      return 2;
    }
    if (botBoard[i - 1][j - 1] != 0 || botBoard[i + n][j + n] != 0) { //Blocked on either the left or right side.
      return 1;
    }
  }


  return 0;
}

/* checkLDiagBlock
 * ------------------
 * Checks a known diagonal of n pieces to see if which (potentially both) ends are blocked, and returns:
 *  - 1 if one end is blocked
 *  - 2 if two ends are blocked
 *  - 0 is neither end is blocked
 * This function checks for:
 * - A regular block
 * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
 * - If the end of the botBoard is reached. */

int checkLDiagBlock(int i, int j, int n) {
   //Check for reaching the end of the botBoard being a block.
   //Check if one side is unblocked and the other on the edge of the botBoard.
   if(i == 0 || j == 7) { //Checks if the first piece is on the edge of the botBoard.
     if(botBoard[i + n][j - n] != 0) {
       return 2;
     }
     return 1;
   }
   if(i + n == 8 || j - n == -1) { //Checks if the last piece is on the edge of the botBoard.
     if(botBoard[i - 1][j + 1] != 0) {
       return 2;
     }
     return 1;
   }

  //Checks for further blocks
  switch (n) {
    case 4: //ie. there is no further blocks for this case.
    break;

    case 3:
    if((i - 1 > -1 && j + 1 < 8) && (i + n + 1 < 8 && j - n - 1 > -1)) { //Checks for case where 211102
      if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n + 1][j - n - 1] != 0) {
        return 2;
      }
    }
    if((i - 2 > -1 && j + 2 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 201112
      if(botBoard[i - 2][j + 2] != 0 && botBoard[i + n][j - n] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if((i - 1 > -1 && j + 1 < 8) && (i + n + 2 < 8 && j - n - 2 > -1)) { // Checks for case where 211002
      if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n + 2][j - n - 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j + 1 < 8) && (i + n + 1 < 8 && j - n - 1 > -1)) { //Checks for case where 21102
      if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n + 1][j - n - 1] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j + 3 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 200112
      if(botBoard[i - 3][j + 3] != 0 && botBoard[i + n][j - n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j + 2 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 20112
      if(botBoard[i - 2][j + 2] != 0 && botBoard[i + n][j - n] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if((i - 1 > -1 && j + 1 < 8) && (i + n + 3 < 8 && j - n - 3 > -1)) { //Checks for case where 210002
      if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n + 3][j - n - 3] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j + 1 < 8) && (i + n + 2 < 8 && j - n - 2 > -1)) { //Checks for case where 21002
      if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n + 2][j - n - 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j + 1 < 8) && (i + n + 1 < 8 && j - n - 1 > -1)) { //Checks for case where 2102
      if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n + 1][j - n - 1] != 0) {
        return 2;
      }
    }
    if ((i - 4 > -1 && j + 4 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 200012
      if(botBoard[i - 4][j + 4] != 0 && botBoard[i + n][j - n] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j + 3 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 20012
      if(botBoard[i - 3][j + 3] != 0 && botBoard[i + n][j - n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j + 2 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 20112
      if(botBoard[i - 2][j + 2] != 0 && botBoard[i + n][j - n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if(((i + n) < 8 && (j - n) > -1) && ((i - 1) > -1 && (j + 1) < 8)) { //Ensures this does not go over the end of the botBoard.
    if(botBoard[i - 1][j + 1] != 0 && botBoard[i + n][j - n] != 0) { //Blocked on right and left side.
      return 2;
    }
    if (botBoard[i - 1][j + 1] != 0 || botBoard[i + n][j - n] != 0) { //Blocked on either the left or right side.
      return 1;
    }
  }


  return 0;
}

/* evaluate
 * ----------------
 * Creates a score for the game botBoard, based on the current botBoard instance. These scores will allow the computer
 * to intelligently place pieces on the game botBoard. The score is created by evaluating each row/column/diagonal
 * of pieces to see how many pieces is in the row, and if the row/column/diagonal is blocked on eithr side by an
 * enemy piece. */

double evaluate() {
  double generalScore = 0; //Will hold the general socre for the botBoard (ie. without the computer piece placed).


  //Scan for rows of pieces (will also find single pieces)
  for(int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if(botBoard[i][j] != 0) {
        int enemy = 0; //Stores which player (computer/human) is the 'enemy' piece for the current spot.
        if(botBoard[i][j] == 1) {
          enemy = 2;
        }
        if(botBoard[i][j] == 2) {
          enemy = 1;
        }

        int n = 1;
        while((botBoard[i][j + n] != 0 && botBoard[i][j + n] != enemy) && (j + n) < 8) {
          n++;
        }

        //At this point, n is the number of pieces in a row.
        if(botBoard[i][j] == 1) { //ie. this is a row of player pieces.
          switch(n) {
            case 1:
            generalScore = generalScore; // Does not change the score of the botBoard.
            break;

            case 2:
            //Need to check if unblocked on either side
            switch(checkRowBlock(i, j, n)) {
              case 0:
              generalScore = generalScore - 150; //Score event 12
              break;

              case 1:
              generalScore = generalScore - 100; //Score event 11
              break;

              case 2:
              break;
            }
            break;

            case 3:
            //Need to check if unblocked on either side
            switch(checkRowBlock(i, j, n)) {
              case 0:
              generalScore = generalScore - 20000; //Score event 10
              break;

              case 1:
              generalScore = generalScore - 125; //Score event 9
              break;

              case 2:
              break;
            }
            break;

            case 4:
            //Need to check if unblocked on either side
            switch(checkRowBlock(i, j, n)) {
              case 0:
              generalScore = generalScore - 25000; //Score event 8
              break;

              case 1:
              break;

              case 2:
              break;
            }
            break;

            case 5:
            generalScore = generalScore - 40000;
            break;
          }
        }

        if(botBoard[i][j] == 2) { //ie. This is a row of computer pieces.
          switch(n) {
            case 1:
            if (j + 1 < 8 && botBoard[i][j + 1] == 0) { //Checks if a piece is surrounded by empty space.
              if (j - 1 > -1 && botBoard[i][j - 1] == 0) {
                if(i + 1 > -1 && botBoard[i + 1][j] == 0) {
                  if (i - 1 < 8 && botBoard[i - 1][j] == 0) {
                    if ((i + 1 > -1 && j + 1 < 8) && botBoard[i + 1][j + 1] == 0) {
                      if ((i + 1 > -1 && j - 1 > -1) && botBoard[i + 1][j - 1] == 0) {
                        if ((i - 1 < 8 && j + 1 < 8) && botBoard[i - 1][j + 1] == 0) {
                          if((i - 1 < 8 && j - 1 > -1) && botBoard[i -1][j - 1] == 0) {
                            generalScore = generalScore + 25;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            break;

            case 2:
            //Need to check if unblocked on either side
            switch(checkRowBlock(i, j, n)) {
              case 0:
              generalScore = generalScore + 50; //Score event 6
              break;

              case 1:
              generalScore = generalScore + 20; //Score event 6.5
              break;

              case 2:
              break;
            }
            break;

            case 3:
            //Need to check if unblocked on either side
            switch(checkRowBlock(i, j, n)) {
              case 0:
              generalScore = generalScore + 150; //Score event 4
              break;

              case 1:
              generalScore = generalScore + 100; //Score event 5
              break;

              case 2:
              break;
            }
            break;

            case 4:
            //Need to check if unblocked on either side
            switch(checkRowBlock(i, j, n)) {
              case 0:
              generalScore = generalScore + 500; //Score event 2
              break;

              case 1:
              generalScore = generalScore + 125; //Score event 3
              break;

              case 2:
              break;
            }
            break;

            case 5:
            generalScore = generalScore + 50000;
            break;

          }

        }
        j = j + n - 1; // Must increment j by n - 1, so that the next spot evaluated is after the row of pieces.
      }
    }
  }

  //Scan for columns of pieces
  for(int j = 0; j < 8; j++) {
    for (int i = 0; i < 8; i++) {
      if(botBoard[i][j] != 0) {
        int enemy = 0; //Stores which player (computer/human) is the 'enemy' piece for the current spot.
        if(botBoard[i][j] == 1) {
          enemy = 2;
        }
        if(botBoard[i][j] == 2) {
          enemy = 1;
        }

        int n = 1;
        while((botBoard[i + n][j] != 0 && botBoard[i + n][j] != enemy) && (i + n) < 8) {
          n++;
        }

        //At this point, n is the number of pieces in a column.
        if(botBoard[i][j] == 1) { //ie. this is a column of player pieces.
          switch(n) {
            case 1:
            generalScore = generalScore; // Does not change the score of the botBoard.
            break;

            case 2:
            //Need to check if unblocked on either side
            switch(checkColumnBlock(i, j, n)) {
              case 0:
              generalScore = generalScore - 150; //Score event 12
              break;

              case 1:
              generalScore = generalScore - 100; //Score event 11
              break;

              case 2:
              break;
            }
            break;

            case 3:
            //Need to check if unblocked on either side
            switch(checkColumnBlock(i, j, n)) {
              case 0:
              generalScore = generalScore - 20000; //Score event 10
              break;

              case 1:
              generalScore = generalScore - 125; //Score event 9
              break;

              case 2:
              break;
            }
            break;

            case 4:
            //Need to check if unblocked on either side
            switch(checkColumnBlock(i, j, n)) {
              case 0:
              generalScore = generalScore - 25000; //Score event 8
              break;

              case 1:
              break;

              case 2:
              break;
            }
            break;

            case 5:
            generalScore = generalScore - 40000;
            break;
          }
        }

        if(botBoard[i][j] == 2) { //ie. This is a row of compuer pieces.
          switch(n) {
            case 1: //Case where 1 piece is surrounded by empty spots is handled above.
            break;

            case 2:
            //Need to check if unblocked on either side
            switch(checkColumnBlock(i, j, n)) {
              case 0:
              generalScore = generalScore + 50; //Score event 6
              break;

              case 1:
              generalScore = generalScore + 20; //Score event 6.5
              break;

              case 2:
              break;
            }
            break;

            case 3:
            //Need to check if unblocked on either side
            switch(checkColumnBlock(i, j, n)) {
              case 0:
              generalScore = generalScore + 150; //Score event 4
              break;

              case 1:
              generalScore = generalScore + 100; //Score event 5
              break;

              case 2:
              break;
            }
            break;

            case 4:
            //Need to check if unblocked on either side
            switch(checkColumnBlock(i, j, n)) {
              case 0:
              generalScore = generalScore + 500; //Score event 2
              break;

              case 1:
              generalScore = generalScore + 125; //Score event 3
              break;

              case 2:
              break;
            }
            break;

            case 5:
            generalScore = generalScore + 50000;
            break;

          }

        }
        i = i + n - 1; // Must increment j by n - 1, so that the next spot evaluated is after the column of pieces.
      }
    }
  }

  /*int diagStore[30][30] = {0};
  int storeCount = 0;
  int diagPrevFound = 0;*/

  //Scan for right diagonals of pieces
  for(int i = 0; i < 7; i++) {
    for(int j = 0; j < 7; j++) {
      if(botBoard[i][j] != 0) {
        /*for(int m = 0; m < storeCount; m++) { //Checks to make sure this diagonal piece has not already been found.
          if(diagStore[m][0] == i && diagStore[m][1] == j) {
            diagPrevFound = 1;
          }
        }*/
        //if(diagPrevFound == 0) {
          //diagStore[storeCount][0] = i;
          //diagStore[storeCount][1] = j;
          //storeCount++;

          int enemy = 0; //Stores which player (computer/human) is the 'enemy' piece for the current spot.
          if(botBoard[i][j] == 1) {
            enemy = 2;
          }
          if(botBoard[i][j] == 2) {
            enemy = 1;
          }

          int n = 1;
          while((botBoard[i + n][j + n] != 0 && botBoard[i + n][j + n] != enemy) && ((i + n) < 8 && (j + n < 8))) {
            //diagStore[storeCount][0] = i;
            //diagStore[storeCount][1] = j;
            //storeCount++;
            n++;
          }

          //At this point, n is the number of pieces in a diagonal.
          if(botBoard[i][j] == 1) { //ie. this is a diagonal of player pieces.
            switch(n) {
              case 1:
              generalScore = generalScore; // Does not change the score of the botBoard.
              break;

              case 2:
              //Need to check if unblocked on either side
              switch(checkRDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore - 150; //Score event 12
                break;

                case 1:
                generalScore = generalScore - 100; //Score event 11
                break;

                case 2:
                break;
              }
              break;

              case 3:
              //Need to check if unblocked on either side
              switch(checkRDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore - 20000; //Score event 10
                break;

                case 1:
                generalScore = generalScore - 125; //Score event 9
                break;

                case 2:
                break;
              }
              break;

              case 4:
              //Need to check if unblocked on either side
              switch(checkRDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore - 25000; //Score event 8
                break;

                case 1:
                break;

                case 2:
                break;
              }
              break;

              case 5:
              generalScore = generalScore - 40000;
              break;
            }
          }

          if(botBoard[i][j] == 2) { //ie. This is a diagonal of compuer pieces.
            switch(n) {
              case 1: //Case where 1 piece is surrounded by empty spots is handled above.
              break;

              case 2:
              //Need to check if unblocked on either side
              switch(checkRDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore + 50; //Score event 6
                break;

                case 1:
                generalScore = generalScore + 20; //Score event 6.5
                break;

                case 2:
                break;
              }
              break;

              case 3:
              //Need to check if unblocked on either side
              switch(checkRDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore + 150; //Score event 4
                break;

                case 1:
                generalScore = generalScore + 100; //Score event 5
                break;

                case 2:
                break;
              }
              break;

              case 4:
              //Need to check if unblocked on either side
              switch(checkRDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore + 500; //Score event 2
                break;

                case 1:
                generalScore = generalScore + 125; //Score event 3
                break;

                case 2:
                break;
              }
              break;

              case 5:
              generalScore = generalScore + 50000;
              break;

            }

          }
          //i = i + n - 1; // Must increment j by n - 1, so that the next spot evaluated is after the column of pieces.
        //}
        //diagPrevFound = 0;
      }
    }
  }

  //Scan for left diagonals of pieces
  for(int i = 0; i < 7; i++) {
    for(int j = 1; j < 8; j++) {
      if(botBoard[i][j] != 0) {
        /*for(int m = 0; m < storeCount; m++) { //Checks to make sure this diagonal piece has not already been found.
          if(diagStore[m][0] == i && diagStore[m][1] == j) {
            diagPrevFound = 1;
          }
        }*/
        //if(diagPrevFound == 0) {
          //diagStore[storeCount][0] = i;
          //diagStore[storeCount][1] = j;
          //storeCount++;

          int enemy = 0; //Stores which player (computer/human) is the 'enemy' piece for the current spot.
          if(botBoard[i][j] == 1) {
            enemy = 2;
          }
          if(botBoard[i][j] == 2) {
            enemy = 1;
          }

          int n = 1;
          while((botBoard[i + n][j - n] != 0 && botBoard[i + n][j - n] != enemy) && ((i + n) < 8 && (j - n < 8))) {
            //diagStore[storeCount][0] = i;
            //diagStore[storeCount][1] = j;
            //storeCount++;
            n++;
          }

          //At this point, n is the number of pieces in a diagonal.
          if(botBoard[i][j] == 1) { //ie. this is a diagonal of player pieces.
            switch(n) {
              case 1:
              generalScore = generalScore; // Does not change the score of the botBoard.
              break;

              case 2:
              //Need to check if unblocked on either side
              switch(checkLDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore - 150; //Score event 12
                break;

                case 1:
                generalScore = generalScore - 100; //Score event 11
                break;

                case 2:
                break;
              }
              break;

              case 3:
              //Need to check if unblocked on either side
              switch(checkLDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore - 20000; //Score event 10
                break;

                case 1:
                generalScore = generalScore - 125; //Score event 9
                break;

                case 2:
                break;
              }
              break;

              case 4:
              //Need to check if unblocked on either side
              switch(checkLDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore - 25000; //Score event 8
                break;

                case 1:
                break;

                case 2:
                break;
              }
              break;

              case 5:
              generalScore = generalScore - 40000;
              break;
            }
          }

          if(botBoard[i][j] == 2) { //ie. This is a diagonal of computer pieces.
            switch(n) {
              case 1: //Case where 1 piece is surrounded by empty spots is handled above.
              break;

              case 2:
              //Need to check if unblocked on either side
              switch(checkLDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore + 50; //Score event 6
                break;

                case 1:
                generalScore = generalScore + 20; //Score event 6.5
                break;

                case 2:
                break;
              }
              break;

              case 3:
              //Need to check if unblocked on either side
              switch(checkLDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore + 150; //Score event 4
                break;

                case 1:
                generalScore = generalScore + 100; //Score event 5
                break;

                case 2:
                break;
              }
              break;

              case 4:
              //Need to check if unblocked on either side
              switch(checkLDiagBlock(i, j, n)) {
                case 0:
                generalScore = generalScore + 500; //Score event 2
                break;

                case 1:
                generalScore = generalScore + 125; //Score event 3
                break;

                case 2:
                break;
              }
              break;

              case 5:
              generalScore = generalScore + 50000;
              break;

            }

          }
          //i = i + n - 1; // Must increment j by n - 1, so that the next spot evaluated is after the column of pieces.
        //}
        //diagPrevFound = 0;
      }
    }
  }

  return generalScore;
}

