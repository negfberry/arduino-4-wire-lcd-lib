#ifndef LCD4Pin_h
#define LCD4Pin_h

#include "Arduino.h"

/*
 * High
 *
 * Shift Register Pin 7   - D0
 * Shift Register Pin 1   - D1
 * Shift Register Pin 2   - D2
 * Shift Register Pin 3   - D3
 * Shift Register Pin 4   - D4
 * Shift Register Pin 5   - D5
 * Shift Register Pin 6   - D6
 * Shift Register Pin 15  - D7
 *
 * Low
 *
 * Shift Register Pin 7   - E         0x01
 * Shift Register Pin 1   - RS        0x02
 * Shift Register Pin 2   - P0        0x1c
 * Shift Register Pin 3   - P1
 * Shift Register Pin 4   - P2
 * Shift Register Pin 5   - PWM/!BIN  0x20
 * Shift Register Pin 6   - BL        0x40
 * Shift Register Pin 15  - Spare
 *
 */

#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00
#define LCD_BLINK_ON 0x01
#define LCD_BLINK_OFF 0x00
#define LCD_5x8_FONT 0
#define LCD_5x10_FONT 1

class LCD4Pin: public Print {
public:
  LCD4Pin(byte dpin, byte lpin, byte cpin, byte bpin);
  LCD4Pin(byte dpin, byte lpin, byte cpin);
  virtual size_t write(byte);
  virtual size_t write(const char *);
  virtual size_t write(const byte *, size_t);
  void command(byte);
  void data(byte);
  void contrast(byte);
  void clear();
  void home();
  void display();
  void noDisplay();
  void control(byte);
  void begin(byte, byte, byte);
  void begin(byte, byte);
  void createChar(byte, const byte *); 
  void setCursor(byte, byte);
  void backlight(byte);
  void scrollDisplayLeft(void);
  void scrollDisplayRight(void);
  void leftToRight(void);
  void rightToLeft(void);
  void autoscroll(void);
  void noAutoscroll(void);
  void cursor();
  void noCursor();
  void blink();
  void noBlink();


private:
  void shift16();
  void xfer2lcd(byte, byte);
  void writebyte(byte);
  void xferCmd2lcd();

  byte _dpin;
  byte _lpin;
  byte _cpin;
  byte _bpin;
  byte _lcdctl;
  byte _dataRegister;    // Stores databits d0-d7
  byte _controlRegister; // Stores control/contrast/backlight data
  byte _rows;            // Number of rows
  byte _row_offsets[4];  // Starting address of each row in RAM
};

#endif

