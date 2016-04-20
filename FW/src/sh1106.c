#include "sh1106.h"

#include "misc.h"
#include "stm32f10x_gpio.h"

#define SH1106_DC   GPIO_Pin_0
#define SH1106_CS   GPIO_Pin_1
#define SH1106_DAT  GPIO_Pin_2
#define SH1106_CLK  GPIO_Pin_3
//------------------------------------------------------------------

void spi_Init()
{
    GPIO_InitTypeDef port;

    // Enable necessary HW
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_Out_PP;
    port.GPIO_Pin = SH1106_DC | SH1106_CS | SH1106_DAT | SH1106_CLK;
    port.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &port);

 /*
    SPI_CMD_REN &= ~(LCD_DC | LCD_RST | LCD_SDA);
    SPI_CMD_SEL &= ~(LCD_DC | LCD_RST | LCD_SDA);
    SPI_CMD_DIR |= (LCD_DC | LCD_RST | LCD_SDA);
    SPI_CMD_OUT &= ~(LCD_DC | LCD_RST | LCD_SDA);

    SPI_CLK_REN &= ~LCD_SCL;
    SPI_CLK_SEL &= ~LCD_SCL;
    SPI_CLK_DIR |= LCD_SCL;
    SPI_CLK_OUT &= ~LCD_SCL;

    SPI_CMD_OUT |= LCD_RST;
    */
}

void delay()
{
    int i;
    for (i = 1000000; i > 0; i--)
    {
        asm("NOP");
    }
}


//------------------------------------------------------------------
void spi_Write(char a)
{
    char i;
    GPIOA->BRR = SH1106_CS;
    for(i = 0; i < 8; i++)
    {
        if (a & 0x80)
            GPIOA->BSRR = SH1106_DAT;
        else
            GPIOA->BRR = SH1106_DAT;

        GPIOA->BSRR = SH1106_CLK;
        a <<= 1;
        GPIOA->BRR = SH1106_CLK;
    }
    GPIOA->BSRR = SH1106_CS;
}

//****************************************************************


void LCDCommand(char command)
{
    GPIOA->BRR = SH1106_DC;
    spi_Write(command);
}

// передать команду в LCD
void LCDCommand_param(char command, char param)
{
    LCDCommand(command);
    spi_Write(param);
}

void LCDData(unsigned char val)
{
    GPIOA->BSRR = SH1106_DC;
    spi_Write(val);
}

void LcdInit()
{
    spi_Init();

    LCDCommand(LCD_CMD_DISPLAY_OFF);//+ display off

    LCDCommand(LCD_CMD_SET_COL_L);
    LCDCommand(LCD_CMD_SET_COL_H);
    LCDCommand(LCD_CMD_START_LINE);//+ Set Page Start Address for Page Addressing Mode,0-7

    LCDCommand_param(LCD_CMD_CONTRAST, 0xFF); ///~~~0xCF);

    LCDCommand(LCD_CMD_SEGMENT_MAP_1);
    LCDCommand(LCD_CMD_SCAN_DIR_MINUS);
    LCDCommand(LCD_CMD_NORMAL_DISPLAY);

//--set multiplex ratio(1 to 64)
    LCDCommand_param(LCD_CMD_MULTIPLEX_RATIO, 63);
//-set display offset    (not offset)
    LCDCommand_param(LCD_CMD_DISPLAY_OFFSET, 0x00);

//--set display clock divide ratio/oscillator frequency
//   set divide ratio
    LCDCommand_param(LCD_CMD_SET_CLOCK_DIVIDE, 0xF0);//

    LCDCommand_param(LCD_CMD_SET_PRE_CHARGE_PERIOD, 0xF1);

    LCDCommand_param(LCD_CMD_SET_PIN_CONF, 0x12);//--  0x02???

    LCDCommand_param(LCD_CMD_SET_VCOMH, 0x40);//--
//    LCDCommand_param(LCD_CMD_SET_VCOMH, 0x20);  // 0.77 * VCC
//    LCDCommand_param(LCD_CMD_SET_VCOMH, 0x10);  // 0.77 * VCC

// Set Memory Addressing Mode
    LCDCommand_param(LCD_CMD_ADDR_MODE, 0x02);

    LCDCommand_param(LCD_CMD_SET_CHARGE_BUMP, 0x14); // Enable Charge Pump

    LCDCommand(LCD_CMD_ALL_OFF);
    LCDCommand(LCD_CMD_NORMAL_DISPLAY);

    LCDCommand(LCD_CMD_DISPLAY_ON);//+ --turn on oled panel

    LcdClear();
}

// включить LCD
void LcdLedON()
{
//    LCDCommand_param(LCD_CMD_SET_CHARGE_BUMP, 0x14); // Enable Charge Pump
    LCDCommand(LCD_CMD_DISPLAY_ON);
}

// выключить LCD
void LcdLedOFF()
{
    LCDCommand(LCD_CMD_DISPLAY_OFF);
//    LCDCommand_param(LCD_CMD_SET_CHARGE_BUMP, 0x10); // Disable Charge Pump
}


//*********************************************************************

void LcdClear()
{
    LcdFill(0x00);
}


void LcdFill(unsigned char fill_Data)
{
    char m = 0;
    char n = 0;
    for(m=0; m<8; m++)
    {
        LCDCommand(LCD_CMD_SET_PAGE + m);    // page0-page1
        LCDCommand(LCD_CMD_SET_COL_L);         // low column start address
        LCDCommand(LCD_CMD_SET_COL_H);        // high column start address

        for(n=0; n<128; n++)
        {
            LCDData(fill_Data);
        }
    }
}

void LcdOut(char * data)
{
    char m = 0;
    char n = 0;

    for(m=0; m<8; m++)
    {
        LCDCommand(LCD_CMD_SET_PAGE + m);    // page0-page1
        LCDCommand(LCD_CMD_SET_COL_L);         // low column start address
        LCDCommand(LCD_CMD_SET_COL_H);        // high column start address

        for(n=0; n<128; n++)
        {
            LCDData(*data);
            data++;
        }
    }
}
