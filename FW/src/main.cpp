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
#include "sm_display.h"
#include "sm_hw_gpio.h"

int main(void)
{
    SmDisplay display;
    display.init(128,64);

    display.setPins(new SmHwGpioPin(GPIOA,1),
                    new SmHwGpioPin(GPIOA,3),
                    new SmHwGpioPin(GPIOA,2),
                    new SmHwGpioPin(GPIOA,0));
    display.setPix(20,20,1);
    display.update();
    while (1)
    {
        display.update();
    }
}
