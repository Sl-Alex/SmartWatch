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
    delay();

    SmartDisplay_init();
    SmartDisplay_setPix(20,20,1);
    SmartDisplay_update();
    while (1)
    {
        SmartDisplay_update();
    }
}
