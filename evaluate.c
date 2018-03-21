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
#include <stdio.h>

int board[8][8] = {{0,0,0,0,0,0,0,0},
                   {0,1,1,1,1,1,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0}};


/* checkRowBlock
 * ------------------
 * Checks a known row of n pieces to see if which (potentially both) ends are blocked, and returns:
 *  - 1 if one end is blocked
 *  - 2 if two ends are blocked
 *  - 0 is neither end is blocked
 * This function checks for:
 * - A regular block
 * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
 * - If the end of the board is reached. */

int checkRowBlock(int i, int j, int n) {
  /* Checks for:
   * - A regular block
   * - A "further" block ie . 0 0 2 1 1 1 0 2 is a two sided block.
   * - If the end of the board is reached.
   */

   //Check for reaching the end of the board being a block.
   //Check if one side is unblocked and the other on the edge of the board.
   if(j == 0) { //Checks if the last piece or first piece is on the edge of the board.
     if(board[i][j + n] != 0) {
       return 2;
     }
     return 1;
   }
   if(j + n == 8) { //Checks if the last piece is on the edge of the board.
     if(board[i][j - 1] != 0) {
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
      if(board[i][j - 1] != 0 && board[i][j + n + 1] != 0) {
        return 2;
      }
    }
    if(j - 2 > -1 && j + n < 8) { //Checks for case where 201112
      if(board[i][j - 2] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if(j - 1 > -1 && j + n + 2 < 8) { // Checks for case where 211002
      if(board[i][j - 1] != 0 && board[i][j + n + 2] != 0) {
        return 2;
      }
    }
    if (j - 1 > -1 && j + n + 1 < 8) { //Checks for case where 21102
      if(board[i][j - 1] != 0 && board[i][j + n + 1] != 0) {
        return 2;
      }
    }
    if (j - 3 > -1 && j + n < 8) { //Checks for case where 200112
      if(board[i][j - 3] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    if (j - 2 > -1 && j + n < 8) { //Checks for case where 20112
      if(board[i][j - 2] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if(j - 1 > -1 && j + n + 3 < 8) { //Checks for case where 210002
      if(board[i][j - 1] != 0 && board[i][j + n + 3] != 0) {
        return 2;
      }
    }
    if (j - 1 > -1 && j + n + 2 < 8) { //Checks for case where 21002
      if(board[i][j - 1] != 0 && board[i][j + n + 2] != 0) {
        return 2;
      }
    }
    if (j - 1 > -1 && j + n + 1 < 8) { //Checks for case where 2102
      if(board[i][j - 1] != 0 && board[i][j + n + 1] != 0) {
        return 2;
      }
    }
    if(j - 4 > -1 && j + n < 8) { //Checks for case where 200012
      if(board[i][j - 4] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    if(j - 2 > -1 && j + n < 8) { //Checks for case where 20012
      if(board[i][j - 3] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    if(j - 2 > -1 && j + n < 8) { //Checks for case where 20112
      if(board[i][j - 2] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if((j + n) < 8 && (j - 1) > -1) { //Ensures this does not go over the end of the board.

    if(board[i][j - 1] != 0 && board[i][j + n] != 0) { //Blocked on right and left side.
      return 2;
    }
    if (board[i][j - 1] != 0 || board[i][j + n] != 0) { //Blocked on either the left or right side.
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
 * - If the end of the board is reached. */

int checkColumnBlock(int i, int j, int n) {
   //Check for reaching the end of the board being a block.
   //Check if one side is unblocked and the other on the edge of the board.
   if(i == 0) { //Checks if the last piece or first piece is on the edge of the board.
     if(board[i + n][j] != 0) {
       return 2;
     }
     return 1;
   }
   if(i + n == 8) { //Checks if the last piece is on the edge of the board.
     if(board[i - 1][j] != 0) {
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
      if(board[i - 1][j] != 0 && board[i + n + 1][j] != 0) {
        return 2;
      }
    }
    if(i - 2 > -1 && i + n < 8) { //Checks for case where 201112
      if(board[i - 2][j] != 0 && board[i + n][j] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if(i - 1 > -1 && i + n + 2 < 8) { // Checks for case where 211002
      if(board[i - 1][j] != 0 && board[i + n + 2][j] != 0) {
        return 2;
      }
    }
    if (i - 1 > -1 && i + n + 1 < 8) { //Checks for case where 21102
      if(board[i - 1][j] != 0 && board[i + n + 1][j] != 0) {
        return 2;
      }
    }
    if (i - 3 > -1 && i + n < 8) { //Checks for case where 200112
      if(board[i - 3][j] != 0 && board[i + n][j] != 0) {
        return 2;
      }
    }
    if (i - 2 > -1 && i + n < 8) { //Checks for case where 20112
      if(board[i - 2][j] != 0 && board[i + n][j] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if(i - 1 > -1 && i + n + 3 < 8) { //Checks for case where 210002
      if(board[i - 1][j] != 0 && board[i + n + 3][j] != 0) {
        return 2;
      }
    }
    if (i - 1 > -1 && i + n + 2 < 8) { //Checks for case where 21002
      if(board[i - 1][j] != 0 && board[i + n + 2][j] != 0) {
        return 2;
      }
    }
    if (i - 1 > -1 && i + n + 1 < 8) { //Checks for case where 2102
      if(board[i - 1][j] != 0 && board[i + n + 1][j] != 0) {
        return 2;
      }
    }
    if(i - 4 > -1 && i + n < 8) { //Checks for case where 200012
      if(board[i - 4][j] != 0 && board[i + n][j] != 0) {
        return 2;
      }
    }
    if(i - 3 > -1 && i + n < 8) { //Checks for case where 20012
      if(board[i - 3][j] != 0 && board[i + n][j] != 0) {
        return 2;
      }
    }
    if(i - 2 > -1 && i + n < 8) { //Checks for case where 20112
      if(board[i][j - 2] != 0 && board[i][j + n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if((i + n) < 8 && (i - 1) > -1) { //Ensures this does not go over the end of the board.
    if(board[i - 1][j] != 0 && board[i + n][j] != 0) { //Blocked on right and left side.

      return 2;
    }
    if (board[i - 1][j] != 0 || board[i + n][j] != 0) { //Blocked on either the left or right side.
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
 * - If the end of the board is reached. */

int checkRDiagBlock(int i, int j, int n) {
   //Check for reaching the end of the board being a block.
   //Check if one side is unblocked and the other on the edge of the board.
   if(i == 0 || j == 0) { //Checks if the first piece is on the edge of the board.
     if(board[i + n][j + n] != 0) {
       return 2;
     }
     return 1;
   }
   if(i + n == 8 || j + n == 8) { //Checks if the last piece is on the edge of the board.
     if(board[i - 1][j - 1] != 0) {
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
      if(board[i - 1][j - 1] != 0 && board[i + n + 1][j + n + 1] != 0) {
        return 2;
      }
    }
    if((i - 2 > -1 && j - 2 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 201112
      if(board[i - 2][j - 2] != 0 && board[i + n][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if((i - 1 > -1 && j - 1 > -1) && (i + n + 2 < 8 && j + n + 2 < 8)) { // Checks for case where 211002
      if(board[i - 1][j - 1] != 0 && board[i + n + 2][j + n + 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j - 1 > -1) && (i + n + 1 < 8 && j + n + 1 < 8)) { //Checks for case where 21102
      if(board[i - 1][j - 1] != 0 && board[i + n + 1][j + n + 1] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j - 3 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 200112
      if(board[i - 3][j - 3] != 0 && board[i + n][j + n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j - 2 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 20112
      if(board[i - 2][j - 2] != 0 && board[i + n][j + n] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if((i - 1 > -1 && j - 1 > -1) && (i + n + 3 < 8 && j + n + 3 < 8)) { //Checks for case where 210002
      if(board[i - 1][j - 1] != 0 && board[i + n + 3][j + n + 3] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j - 1 > -1) && (i + n + 2 < 8 && j + n + 2 < 8)) { //Checks for case where 21002
      if(board[i - 1][j - 1] != 0 && board[i + n + 2][j + n + 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j - 1 > -1) && (i + n + 1 < 8 && j + n + 1 < 8)) { //Checks for case where 2102
      if(board[i - 1][j - 1] != 0 && board[i + n + 1][j + n + 1] != 0) {
        return 2;
      }
    }
    if ((i - 4 > -1 && j - 4 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 200012
      if(board[i - 4][j - 4] != 0 && board[i + n][j + n] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j - 3 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 20012
      if(board[i - 3][j - 3] != 0 && board[i + n][j + n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j - 2 > -1) && (i + n < 8 && j + n < 8)) { //Checks for case where 20112
      if(board[i - 2][j - 2] != 0 && board[i + n][j + n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if(((i + n) < 8 && (j + n) < 8) && ((i - 1) > -1 && (j - 1) > -1)) { //Ensures this does not go over the end of the board.
    if(board[i - 1][j - 1] != 0 && board[i + n][j + n] != 0) { //Blocked on right and left side.
      return 2;
    }
    if (board[i - 1][j - 1] != 0 || board[i + n][j + n] != 0) { //Blocked on either the left or right side.
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
 * - If the end of the board is reached. */

int checkLDiagBlock(int i, int j, int n) {
   //Check for reaching the end of the board being a block.
   //Check if one side is unblocked and the other on the edge of the board.
   if(i == 0 || j == 7) { //Checks if the first piece is on the edge of the board.
     if(board[i + n][j - n] != 0) {
       return 2;
     }
     return 1;
   }
   if(i + n == 8 || j - n == -1) { //Checks if the last piece is on the edge of the board.
     if(board[i - 1][j + 1] != 0) {
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
      if(board[i - 1][j + 1] != 0 && board[i + n + 1][j - n - 1] != 0) {
        return 2;
      }
    }
    if((i - 2 > -1 && j + 2 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 201112
      if(board[i - 2][j + 2] != 0 && board[i + n][j - n] != 0) {
        return 2;
      }
    }
    break;

    case 2:
    if((i - 1 > -1 && j + 1 < 8) && (i + n + 2 < 8 && j - n - 2 > -1)) { // Checks for case where 211002
      if(board[i - 1][j + 1] != 0 && board[i + n + 2][j - n - 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j + 1 < 8) && (i + n + 1 < 8 && j - n - 1 > -1)) { //Checks for case where 21102
      if(board[i - 1][j + 1] != 0 && board[i + n + 1][j - n - 1] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j + 3 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 200112
      if(board[i - 3][j + 3] != 0 && board[i + n][j - n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j + 2 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 20112
      if(board[i - 2][j + 2] != 0 && board[i + n][j - n] != 0) {
        return 2;
      }
    }
    break;

    case 1:
    if((i - 1 > -1 && j + 1 < 8) && (i + n + 3 < 8 && j - n - 3 > -1)) { //Checks for case where 210002
      if(board[i - 1][j + 1] != 0 && board[i + n + 3][j - n - 3] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j + 1 < 8) && (i + n + 2 < 8 && j - n - 2 > -1)) { //Checks for case where 21002
      if(board[i - 1][j + 1] != 0 && board[i + n + 2][j - n - 2] != 0) {
        return 2;
      }
    }
    if ((i - 1 > -1 && j + 1 < 8) && (i + n + 1 < 8 && j - n - 1 > -1)) { //Checks for case where 2102
      if(board[i - 1][j + 1] != 0 && board[i + n + 1][j - n - 1] != 0) {
        return 2;
      }
    }
    if ((i - 4 > -1 && j + 4 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 200012
      if(board[i - 4][j + 4] != 0 && board[i + n][j - n] != 0) {
        return 2;
      }
    }
    if ((i - 3 > -1 && j + 3 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 20012
      if(board[i - 3][j + 3] != 0 && board[i + n][j - n] != 0) {
        return 2;
      }
    }
    if ((i - 2 > -1 && j + 2 < 8) && (i + n < 8 && j - n > -1)) { //Checks for case where 20112
      if(board[i - 2][j + 2] != 0 && board[i + n][j - n] != 0) {
        return 2;
      }
    }
    break;

  }

  //Next two if statements check for regular blocks.
  if(((i + n) < 8 && (j - n) > -1) && ((i - 1) > -1 && (j + 1) < 8)) { //Ensures this does not go over the end of the board.
    if(board[i - 1][j + 1] != 0 && board[i + n][j - n] != 0) { //Blocked on right and left side.
      return 2;
    }
    if (board[i - 1][j + 1] != 0 || board[i + n][j - n] != 0) { //Blocked on either the left or right side.
      return 1;
    }
  }


  return 0;
}

/* evaluate
 * ----------------
 * Creates a score for the game board, based on the current board instance. These scores will allow the computer
 * to intelligently place pieces on the game board. The score is created by evaluating each row/column/diagonal
 * of pieces to see how many pieces is in the row, and if the row/column/diagonal is blocked on eithr side by an
 * enemy piece. */

int evaluate() {
  int scores[8][8] = {0}; //Will hold the scores of each place on the board.
  int generalScore = 0; //Will hold the general socre for the board (ie. without the computer piece placed).
  int tempScore = 0;


  //Scan for rows of pieces (will also find single pieces)
  for(int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if(board[i][j] != 0) {
        int enemy = 0; //Stores which player (computer/human) is the 'enemy' piece for the current spot.
        if(board[i][j] == 1) {
          enemy = 2;
        }
        if(board[i][j] == 2) {
          enemy = 1;
        }

        int n = 1;
        while((board[i][j + n] != 0 && board[i][j + n] != enemy) && (j + n) < 8) {
          n++;
        }

        //At this point, n is the number of pieces in a row.
        if(board[i][j] == 1) { //ie. this is a row of player pieces.
          switch(n) {
            case 1:
            generalScore = generalScore; // Does not change the score of the board.
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
            generalScore = generalScore - 30000;
            break;
          }
        }

        if(board[i][j] == 2) { //ie. This is a row of computer pieces.
          switch(n) {
            case 1:
            if (j + 1 < 8 && board[i][j + 1] == 0) { //Checks if a piece is surrounded by empty space.
              if (j - 1 > -1 && board[i][j - 1] == 0) {
                if(i + 1 > -1 && board[i + 1][j] == 0) {
                  if (i - 1 < 8 && board[i - 1][j] == 0) {
                    if ((i + 1 > -1 && j + 1 < 8) && board[i + 1][j + 1] == 0) {
                      if ((i + 1 > -1 && j - 1 > -1) && board[i + 1][j - 1] == 0) {
                        if ((i - 1 < 8 && j + 1 < 8) && board[i - 1][j + 1] == 0) {
                          if((i - 1 < 8 && j - 1 > -1) && board[i -1][j - 1] == 0) {
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
      if(board[i][j] != 0) {
        int enemy = 0; //Stores which player (computer/human) is the 'enemy' piece for the current spot.
        if(board[i][j] == 1) {
          enemy = 2;
        }
        if(board[i][j] == 2) {
          enemy = 1;
        }

        int n = 1;
        while((board[i + n][j] != 0 && board[i + n][j] != enemy) && (i + n) < 8) {
          n++;
        }

        //At this point, n is the number of pieces in a column.
        if(board[i][j] == 1) { //ie. this is a column of player pieces.
          switch(n) {
            case 1:
            generalScore = generalScore; // Does not change the score of the board.
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
            generalScore = generalScore - 30000;
            break;
          }
        }

        if(board[i][j] == 2) { //ie. This is a row of compuer pieces.
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
      if(board[i][j] != 0) {
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
          if(board[i][j] == 1) {
            enemy = 2;
          }
          if(board[i][j] == 2) {
            enemy = 1;
          }

          int n = 1;
          while((board[i + n][j + n] != 0 && board[i + n][j + n] != enemy) && ((i + n) < 8 && (j + n < 8))) {
            //diagStore[storeCount][0] = i;
            //diagStore[storeCount][1] = j;
            //storeCount++;
            n++;
          }

          //At this point, n is the number of pieces in a diagonal.
          if(board[i][j] == 1) { //ie. this is a diagonal of player pieces.
            switch(n) {
              case 1:
              generalScore = generalScore; // Does not change the score of the board.
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
              generalScore = generalScore - 30000;
              break;
            }
          }

          if(board[i][j] == 2) { //ie. This is a diagonal of compuer pieces.
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
      if(board[i][j] != 0) {
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
          if(board[i][j] == 1) {
            enemy = 2;
          }
          if(board[i][j] == 2) {
            enemy = 1;
          }

          int n = 1;
          while((board[i + n][j - n] != 0 && board[i + n][j - n] != enemy) && ((i + n) < 8 && (j - n < 8))) {
            //diagStore[storeCount][0] = i;
            //diagStore[storeCount][1] = j;
            //storeCount++;
            n++;
          }

          //At this point, n is the number of pieces in a diagonal.
          if(board[i][j] == 1) { //ie. this is a diagonal of player pieces.
            switch(n) {
              case 1:
              generalScore = generalScore; // Does not change the score of the board.
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
              generalScore = generalScore - 30000;
              break;
            }
          }

          if(board[i][j] == 2) { //ie. This is a diagonal of computer pieces.
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

int main(void) {
  printf("%d\n", evaluate());
}
