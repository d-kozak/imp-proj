/*******************************************************************************
   utils.c: misc code for display_lowlevel
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Josef Strnadel <strnadel AT stud.fit.vutbr.cz>

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

#include <string.h>

unsigned char str_tmp[MAX_COMMAND_LEN+1];

/*******************************************************************************
 * Funkce pro vypocet 2. mocniny cisla i 
*******************************************************************************/
unsigned int pow2(unsigned int i)
{
  int j, res = 1;
  for(j = 1; j <= i; j++)
    res *= 2;
  return res;
}

/*******************************************************************************
 * Funkce pro prevod 8-bitoveho binarniho cisla b7b6b5b4b3b2b1b0 na dekadicke
*******************************************************************************/
unsigned int bin2dec(char bin[8])
{
  int i, j, res = 0;
  for(i=7; i>=0; i--)
  {
    if (bin[i]=='0' || bin[i]=='1')
      j = (bin[i] - 48) * pow2(7 - i);
    else j = 0;
    res += j;
  }
  return res;
}

/*******************************************************************************
 * Funkce vraci poslednich len znaku z retezce str
*******************************************************************************/
unsigned char *get_data(unsigned char *str, int len)
{
  int i, j = len-1;

  strcpy(str_tmp, str);

  str_tmp[len]=0;  
  for(i = (strlen(str) - 1); i >= (strlen(str) - len); i--)
  {
    str_tmp[j--] = str[i];
  }

  return(str_tmp);
}
