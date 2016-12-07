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

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

static const int BOARD_SIZE = 8;

/**
* a structure representing one cell
*/
typedef BOOL t_cell;

typedef t_cell[BOARD_SIZE][BOARD_SIZE] t_board;

/**
* representation of the board
*/
t_board cells;

/**
* temporary storage used while calculating the next state
*/
t_board tmp;


/**
* true = game is running, false = debugging mode
*/
bool isRunning = false;

char last_ch; //naposledy precteny znak
char char_cnt = 0;

t_board startPositions []  =  {  
                                {
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0}
                                },
                                {
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1}
                                },
                                {
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0}
                                },
                              };



t_board hello = {
                    {0,1,0,1,0,1,1,0},
                    {0,1,0,1,0,1,1,0},
                    {0,1,0,1,0,1,1,0},
                    {0,1,1,1,0,1,1,0},
                    {0,1,1,1,0,1,1,0},
                    {0,1,0,1,0,1,1,0},
                    {0,1,0,1,0,1,1,0},
                    {0,1,0,1,0,1,1,0}
                };

void computeNextStep(){

}

void setCellsStart(int index){
  t_board * newBoard = NULL;
  if(index == 0){
      newBoard = &hello;
  } else if(index > 3) {
    fprintf(stderr, "Index too big, max is 3, current was %d\n",index); 
    return;
  } else {
    newBoard = &startPositions[index -1];
  }
  memcpy(&cells,newBoard,sizeof(t_board));
}


void actualizeDiplay(){

}


void print_user_help(void) { }

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

void fpga_initialized()
{
}

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
        case '0': isRunning = ~isRunning; break;
        case '#': if(!isRunning) computeNextStep(); break;
        case '1': isRunning = false; setCellsStart(1); break;
        case '2': isRunning = false; setCellsStart(2); break;
        case '3': isRunning = false; setCellsStart(3); break;
        default:
            //nothing...
              break;
      }


      ////old
      if (char_cnt > 31) {
         LCD_clear();
         char_cnt = 0;
      }
      LCD_append_char(ch);
      char_cnt++;
    }
  }
  
  return 0;
}

interrupt (TIMERA0_VECTOR) Timer_A (void){
  if(isRunning)
    computeNextStep();
  CCR0 += 0x4000; // plan next interrupt
}

int main(void)
{
  char_cnt = 0;
  last_ch = 0;

  initialize_hardware();
  keyboard_init();
  LCD_init();
  LCD_clear();

  setCellsStart(0); 

  CCTL0 = CCIE // enable timer interrupt
  CCR0 = 0x4000; // next interrupt after about half a second

  TACL = TASSEL_1 + MC_2;  // ACLK f = 32786 Hz = 0x8000 Hz 

  while (1)
  {
    keyboard_idle();  // obsluha klavesnice
    terminal_idle();  // obsluha terminalu
    actualizeDisplay();
  }         
}


