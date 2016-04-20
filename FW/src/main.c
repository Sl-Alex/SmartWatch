/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x_conf.h"
#include "smartdisplay.h"

int main(void)
{
    int i,x,y;
        delay();

    SmartDisplay_init();
    SmartDisplay_setPix(20,20,1);
    SmartDisplay_update();
/*    LcdInit();
    LcdFill(0x00);
LcdSetPix(0,0,1);
LcdSetPix(0,1,1);
LcdSetPix(1,0,1);
LcdSetPix(127,0,1);
LcdSetPix(126,0,1);
LcdSetPix(127,1,1);
LcdSetPix(0,63,1);
LcdSetPix(0,62,1);
LcdSetPix(1,63,1);
LcdSetPix(127,63,1);
LcdSetPix(126,63,1);
LcdSetPix(127,62,1);
for (x = 44; x < 84; x++)
{
    LcdSetPix(x,32,1);
}
for (y = 12; y < 52; y++)
{
    LcdSetPix(64,y,1);
}
LcdUpdate();
  while(1)
  {
      LcdSetPix(i,10,0);
      LcdSetPix(127-i,53,0);
      LcdSetPix(117,i & 0x3F,0);
      LcdSetPix(10,63 - (i & 0x3F),0);
      i++;
      if (i == 128) i = 0;
      LcdSetPix(i,10,1);
      LcdSetPix(127-i,53,1);
      LcdSetPix(117,i & 0x3F,1);
      LcdSetPix(10,63 - (i & 0x3F),1);
      LcdUpdate();
  }
  */
  while (1)
  {
      SmartDisplay_update();
      //for (i = 0; i < 30000; i++);
  }
}
