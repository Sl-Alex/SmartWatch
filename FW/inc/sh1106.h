#ifndef SH1106_H_INCLUDED
#define SH1106_H_INCLUDED

// Set Lower Column Start Address for Page Addressing Mode (00h~0Fh)
#define LCD_CMD_SET_COL_L               0x00
// Set Higher Column Start Address for Page Addressing Mode (10h~1Fh)
#define LCD_CMD_SET_COL_H               0x10

// Set Memory Addressing Mode (20h)
#define LCD_CMD_ADDR_MODE               0x20
// след байт:
//  0 - Horizontal Addressing Mode;
//  1 - Vertical Addressing Mode;
//  2 - Page Addressing Mode (RESET);
//  3 - Invalid

// Set Column Address (21h)

// Set Page Address (22h)


// Set Display Start Line (40h~7Fh)
#define LCD_CMD_START_LINE              0x40

// Set Contrast Control for BANK0 (81h)
#define LCD_CMD_CONTRAST                0x81
// след байт: контрастность



// Charge Bump Setting
#define LCD_CMD_SET_CHARGE_BUMP         0x8D
// след байт: значение


// Set Segment Re-map (A0h/A1h)
// column address 0 is mapped to SEG0 (RESET)
#define LCD_CMD_SEGMENT_MAP_0           0xA0
// column address 127 is mapped to SEG0
#define LCD_CMD_SEGMENT_MAP_1           0xA1


// Entire Display ON (A4h/A5h)
#define LCD_CMD_ALL_OFF                 0xA4
#define LCD_CMD_ALL_ON                  0xA5


// Set Normal/Inverse Display (A6h/A7h)
#define LCD_CMD_NORMAL_DISPLAY          0xA6
#define LCD_CMD_INVERSE_DISPLAY         0xA7


// Set Multiplex Ratio (A8h)
#define LCD_CMD_MULTIPLEX_RATIO         0xA8
// след байт: значение


#define LCD_CMD_DCDC_ON                 0xAD


// Set Display ON/OFF (AEh/AFh)
#define LCD_CMD_DISPLAY_ON              0xAF
#define LCD_CMD_DISPLAY_OFF             0xAE


// Set Page Start Address for Page Addressing Mode (B0h~B7h)
#define LCD_CMD_SET_PAGE                0xB0


// Set COM Output Scan Direction (C0h/C8h)
// C0h, X[3]=0b: normal mode (RESET) Scan from COM0 to COM[N –1]
#define LCD_CMD_SCAN_DIR_PLUS           0xC0
// C8h, X[3]=1b: remapped mode. Scan from COM[N-1] to COM0
#define LCD_CMD_SCAN_DIR_MINUS          0xC8


// Set Display Offset (D3h)
#define LCD_CMD_DISPLAY_OFFSET          0xD3
// след байт: значение


// Set Display Clock Divide Ratio/ Oscillator Frequency (D5h)
#define LCD_CMD_SET_CLOCK_DIVIDE        0xD5
// след байт:   Display Clock Divide Ratio + Oscillator Frequency


// Set Pre-charge Period (D9h)
#define LCD_CMD_SET_PRE_CHARGE_PERIOD   0xD9
// след байт: значение


// Set COM Pins Hardware Configuration (DAh)
#define LCD_CMD_SET_PIN_CONF            0xDA
// след байт: значение


// Set VCOMH Deselect Level (DBh)
#define LCD_CMD_SET_VCOMH               0xDB
// след байт: значение

#ifdef __cplusplus
extern "C" {
#endif
void LCDCommand(char command);
void LCDCommand_param(char command, char param);
void LCDData(unsigned char val);
extern void LcdInit(void);
void LcdLedON();
void LcdLedOFF();
void LcdClear();
void LcdFill(unsigned char fill_Data);
extern void LcdOut(char * data);
#ifdef __cplusplus
}
#endif

#endif /* SH1106_H_INCLUDED */

