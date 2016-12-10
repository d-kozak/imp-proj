/*******************************************************************************
   main.c: LCD + keyboard demo
   Copyright (C) 2012 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Michal Bidlo <bidlom AT fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

/**
* defines the size of the game board
*/
#define BOARD_SIZE 8

/**
* messaged that are shown on the display when user hits some button
*/
static const char * welcomeMsg = "Welcome :)";
static const char * title1 = "E1 - Oscilators";
static const char * title2 = "E2 - Chaos";
static const char * title3 = "E3 - Glider";
static const char * set8Neighbour = "8 neighbors mode";
static const char * set9Neighbour = "9 neighbors mode";


/**
* structure representing one cell
*/
typedef bool t_cell;

/**
* structure representing the game field
*/
typedef struct {
	t_cell cells[BOARD_SIZE][BOARD_SIZE];
} t_board;


/**
* representation of the board
*/
t_board currentBoard;

/**
* temporary storage used while calculating the next state
* also serves as 'hello'
*/
t_board tmp = {
	{	 0,1,0,1,0,1,1,0,
		 0,1,0,1,0,1,1,0 ,
		 0,1,0,1,0,1,1,0 ,
		 0,1,1,1,0,1,1,0 ,
		 0,1,1,1,0,1,1,0 ,
		 0,1,0,1,0,1,1,0 ,
		 0,1,0,1,0,1,1,0 ,
		 0,1,0,1,0,1,1,0
	}
};


/**
* true = game is running, false = debugging mode
*/
bool isRunning = false;

/**
* true = 8 neighbours, false  = 9 neighbours
*/
bool eightNeighbourMode = true;

/**
*
*/
unsigned char currentCol = 254;

char last_ch; //naposledy precteny znak

/**
* three startting positions
*/
t_board startPositions[] =
{
	{{
		  0,0,0,0,0,0,0,0,
		  0,0,1,0,0,0,0,0,
		  0,0,1,0,0,0,0,0,
		  0,0,1,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,1,1,1,0,
		  0,0,0,1,1,1,0,0,
      0,0,0,0,0,0,0,0

	}},{{

      0,0,0,0,0,0,1,1,
      0,1,1,0,0,0,1,1,
      0,1,1,0,0,0,0,0,
      0,0,0,1,1,0,0,0,
      0,0,0,1,1,0,0,0,
      0,0,0,0,0,0,0,0,
      0,0,0,0,1,1,1,0,
      0,0,0,0,0,0,0,0

	}},{{

      0,0,1,0,0,0,0,0,
      1,0,1,0,0,0,0,0,
      0,1,1,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
	}}
};


/**
* @param row row
* @param col col
* @return cell at indexes specifies by rows and cols
* the board is circular which means that -1 is considered to be the the last row/col opposite edge of the board
*/
t_cell getCell(int row,int col){
  if(row < 0)
    row = BOARD_SIZE - 1;
  else if(row >= BOARD_SIZE)
    row = 0;

  if(col < 0)
    col = BOARD_SIZE - 1;
  else if(col >= BOARD_SIZE)
    col = 0;

    return currentBoard.cells[row][col];
}

/**
* counts the number of alive neighbours of a cell at index specified by row and col
* it uses either 8-neighbour mode or 9-neighbour mode based on the value of eightNeighbourMode
* @param row row
* @param col col
*/
int countAliveNeighbors(int row,int col){
  int i,j,count = 0;
  for(i = row - 1 ; i < row + 2 ;   i++) {
    for(j = col - 1 ; j < col + 2  ; j++ ) {
        if(eightNeighbourMode && i == row && j == col)
          continue;
        if(getCell(i,j))
          count++;
    }
  }
  return count;
}

/**
* computes next generation of cells
* iterates over the entire board, stores results in tmp
* and whn the calculation is finished, it copies them to the main board
*/
void computeNextStep(){
  int i,j,count;
  for(i = 0; i < BOARD_SIZE ; i++){
    for(j = 0; j < BOARD_SIZE ; j++){
        t_cell current_cell = currentBoard.cells[i][j];
        if(current_cell){
          // is alive
          count = countAliveNeighbors(i,j);
          if(count < 2)
            tmp.cells[i][j] = false;
          else if(count == 2 || count == 3)
            tmp.cells[i][j] = true;
          else
            tmp.cells[i][j] = false;
        } else {
          // is dead
          if(countAliveNeighbors(i,j) == 3)
            tmp.cells[i][j] = true;
          else
            tmp.cells[i][j] = false;
        }
    }
  }

  memcpy(&currentBoard,&tmp, sizeof(t_board));
}

/**
*sets startPosition of the board
*@param index shouls be between 0-3
* there are three start posisions + special 'hello' position
*/
void setCellsStart(int index){
  t_board * newBoard = NULL;
  if(index == 0){
      newBoard = &tmp;
  } else if(index > 3) {
    term_send_str("Index too big, max is 3");
    return;
  } else {
    newBoard = &startPositions[index -1];
  }
  memcpy(&currentBoard,newBoard,sizeof(t_board));
}

/**
* prints help to the terminal
*/
void print_user_help(void) {
	term_send_str("Hello, sorry, but no commands are suported right now, you have to use the buttons on fitkit");
}


/**
* decodes command typed into terminal, no command suported now
*/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

void fpga_initialized()
{
}

/**
* routine which lets the user control the app by pressing buttons on fitkit
*/
int keyboard_idle()
{
  char ch;
  ch = key_decode(read_word_keyboard_4x4());
  if (ch != last_ch) // osetreni drzeni klavesy
  {
    last_ch = ch;
    if (ch != 0) // pokud byla stisknuta klavesa
    {
      switch(ch){
        case '0':  isRunning = !isRunning; break;
        case '#':  if(isRunning) isRunning = false; computeNextStep(); break;
        case '1':  LCD_clear(); LCD_append_string(title1); isRunning = false; setCellsStart(1); break;
        case '2':  LCD_clear(); LCD_append_string(title2); isRunning = false; setCellsStart(2); break;
        case '3':  LCD_clear(); LCD_append_string(title3); isRunning = false; setCellsStart(3); break;
        case 'C':  LCD_clear();  break;
        case '8':  LCD_clear(); LCD_append_string(set8Neighbour); eightNeighbourMode = true; break;
        case '9':  LCD_clear(); LCD_append_string(set9Neighbour); eightNeighbourMode = false; break;
        default:
            //nothing...
              break;
        }
      }
  }

  return 0;
}

/**
* peforms left rotation, returns the result
*/
unsigned char rol(unsigned char num)
{
  return ((num << 1) | (num >> (7)));
}

/**
* debug function which prints only elements at the diagonal
*/
void danceOnTheDiagonal(unsigned char column){
  switch(column){
    case 0: P4OUT = 1; P1OUT = 0; break;
    case 1: P4OUT = 2; P1OUT = 0; break;
    case 2: P4OUT = 4; P1OUT = 0; break;
    case 3: P4OUT = 8; P1OUT = 0; break;
    case 4: P4OUT = 16; P1OUT = 0; break;
    case 5: P4OUT = 32; P1OUT = 0; break;
    case 6: P4OUT = 0; P1OUT = 16; break;
    case 7: P6OUT = 0; P1OUT = 32; break;
    default:
     term_send_str("error, default in switch in preparePinsForCol");
     term_send_num(currentCol);
     term_send_str_crlf("");
     return;
  }
}

/**
* set the output pins to light a specified column
*/
void preparePinsForCol(unsigned char column){
  int i;
  unsigned char p4 = 0;
  unsigned char p1 = 0;
  for( i = 0 ; i < 6 ; i++)
    p4 |= (currentBoard.cells[i][column] << i);
  p1 |= (currentBoard.cells[6][column] << 4);
  p1 |= (currentBoard.cells[7][column] << 5);
  P4OUT = p4;
  P1OUT = p1;
}

/**
* helper function which lights a specified column based on the value of currentCol
*/
void lightCol(){

  switch (currentCol) {
    case 254: preparePinsForCol(0); break;
    case 253: preparePinsForCol(1); break;
    case 251: preparePinsForCol(2); break;
    case 247: preparePinsForCol(3); break;
    case 239: preparePinsForCol(4); break;
    case 223: preparePinsForCol(5); break;
    case 191: preparePinsForCol(6); break;
    case 127: preparePinsForCol(7); break;
    default:
     term_send_str("error, default in switch in lightRow");
     term_send_num(currentCol);
     term_send_str_crlf("");
     return;
  }
  P6OUT = currentCol;
}

/**
* debug function, lights all pins
*/
void lightItAll(){
  P6OUT = 0;
  P4OUT = 63;
  P1OUT = 48;
}

/**
* main function
*/
int main(void)
{
  last_ch = 0;

  initialize_hardware();
  WDG_stop();
  keyboard_init();
  LCD_init();
  LCD_clear();

  setCellsStart(0);

  CCTL0 = CCIE; // enable timer interrupt
  CCR0 = 0x0040; // next interrupt after about half a second

  TACTL = TASSEL_1 + MC_2;  // ACLK f = 32786 Hz = 0x8000 Hz

  // all pins in port 6 are used
  P6DIR = 255;

  // pinns 0-5 in port 4 are used for rows 0 to 5
  P4DIR |= 63;

  // pins 4 and 5 in port 1 are used for row 6 and 7
  P1DIR |= 48;

  lightItAll();
  LCD_append_string(welcomeMsg);
  while (1)
  {
    keyboard_idle();  // obsluha klavesnice
    terminal_idle();  // obsluha terminalu

    delay_ms(100); // delays a specified amout of time before comuputing the next step
    if(isRunning){
      computeNextStep();
    }
  }
}

interrupt (TIMERA0_VECTOR) Timer_A (void)
{
  currentCol = rol(currentCol);
  lightCol();
  CCR0 += 0x0040; // plan next interrupt
}
