#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <fitkitlib.h>

// Base address for VideoRAM
#define BASE_ADDR_VRAM 0x8000

#define VGA_COLUMNS 80
#define VGA_ROWS 60
#define DRAW_H 10
#define DRAW_W 10

const unsigned char drawing[DRAW_H][DRAW_W] = {

        {0,0,0,1,1,1,1,0,0,0},
        {0,0,1,0,0,0,0,1,0,0},
        {0,1,0,0,0,0,0,0,1,0},
        {1,0,0,2,0,0,2,0,0,1},
        {1,0,0,0,4,4,0,0,0,1},
        {1,0,0,0,4,4,0,0,0,1},
        {1,0,3,0,0,0,0,3,0,1},
        {0,1,0,3,3,3,3,0,1,0},
        {0,0,1,0,0,0,0,1,0,0},
        {0,0,0,1,1,1,1,0,0,0}
};

void print_user_help(void) { }

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) {

    return CMD_UNKNOWN;
}

void fpga_initialized(void) { }

void VGA_Clear(void) {
  unsigned short i, j;
  unsigned char color = 0;

  for (i=0; i<VGA_ROWS; i++) {
     for (j = 0; j<VGA_COLUMNS; j++) {
        FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE,
            BASE_ADDR_VRAM + i*VGA_COLUMNS + j, &color, 2, 1);
     }
  }
}

void VGA_Draw(void) {

    unsigned short i, j;
    unsigned short v_edge = (VGA_ROWS - DRAW_H)   /2;
    unsigned short h_edge = (VGA_COLUMNS - DRAW_W)/2;
    unsigned char color;

    for (i=0; i < DRAW_H; i++) {
        for (j = 0; j < DRAW_W; j++) {
            color = drawing[i][j];
            if (color != 0)   // barva jina nez cerna
                FPGA_SPI_RW_AN_DN(
                    SPI_FPGA_ENABLE_WRITE,
                    BASE_ADDR_VRAM + (i + v_edge) * 128 + j + h_edge,
                    &color, 2, 1);
        }
    }
}

int main(void) {

    initialize_hardware();
    WDG_stop();

	VGA_Clear();
    VGA_Draw();
    
    while (1) {
        terminal_idle();
    }
}
