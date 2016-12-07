/*******************************************************************************
   main.c: obsluzny program pro rizeni 2 BRAM v FPGA z MCU
   Copyright (C) 2009 Brno University of Technology,
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
#include <stdlib.h>

#define BRAM1_BASE 0x0
#define BRAM2_BASE 0x400

// omezeni max. adresy jen z prostorovych duvodu vypisu
#define ADDR_LIMIT 0x0F

void print_user_help(void)
{
    term_send_str_crlf(" CLEAR1/CLEAR2......vynuluj obsah BRAM1/BRAM2");
    term_send_str_crlf(" WRITE1/WRITE2 addr......zapis hodnoty addr na adresu addr (desitkove) do BRAM1/BRAM2");
    term_send_str_crlf(" READ.............. cteni obsahu pameti BRAM1, BRAM2");
}

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
    unsigned short offset, val;

    if (strcmp6(cmd_ucase, "CLEAR1") || strcmp6(cmd_ucase, "CLEAR2"))
    {
        for (offset=0; offset <= ADDR_LIMIT; offset++)
        {
            val = 0;
            //zapis 16 bitove hodnoty val na adresu v prislusne BRAM
            FPGA_SPI_RW_AN_DN(
                SPI_FPGA_ENABLE_WRITE,
                cmd_ucase[5] == '1' ? BRAM1_BASE + offset : BRAM2_BASE + offset,
                (unsigned char *)&val, 2, 2);
        }
    }
    else if (strcmp6(cmd_ucase, "WRITE1") || strcmp6(cmd_ucase, "WRITE2"))
    {
        // konverze na hodnotut, pocatecni bile znaky preskoceny
        offset = (unsigned short) atoi(&cmd_ucase[6]);
        //zapis 16 bitove hodnoty offset na adresu prislusne BRAM
        FPGA_SPI_RW_AN_DN(
            SPI_FPGA_ENABLE_WRITE,
            cmd_ucase[5] == '1' ? BRAM1_BASE + offset : BRAM2_BASE + offset,
            (unsigned char *) &offset, 2, 2);
    }
    else if (strcmp4(cmd_ucase, "READ"))
    {
            term_send_str("BRAM1: ");
            for (offset = 0; offset <= ADDR_LIMIT; offset++)
            {
                //cteni 16 bitove hodnoty z adresy BRAM1_BASE + offset
                FPGA_SPI_RW_AN_DN( SPI_FPGA_ENABLE_READ, BRAM1_BASE + offset,
                    (unsigned char *)&val, 2, 2);
                term_send_num(val); term_send_str("  ");
            }
            term_send_crlf();

            term_send_str("BRAM2: "); // to same pro BRAM2
            for (offset = 0; offset <= ADDR_LIMIT; offset++)
            {
                FPGA_SPI_RW_AN_DN( SPI_FPGA_ENABLE_READ, BRAM2_BASE + offset,
                    (unsigned char *)&val, 2, 2);
                term_send_num(val); term_send_str("  ");
            }
            term_send_crlf();
    }
    else
    {
        return (CMD_UNKNOWN);
    }

    return (USER_COMMAND);
}

void fpga_initialized()
{
}

int main(void)
{
    initialize_hardware();

    while (1) terminal_idle();  // obsluha terminalu
}
