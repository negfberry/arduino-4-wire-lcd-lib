#include "LCD4Pin.h"
#include <Arduino.h>

#define EPIN B00000001
#define RPIN B00000010

LCD4Pin::LCD4Pin(byte dpin, byte lpin, byte cpin, byte bpin)
{
  _dpin  = dpin;
  _lpin = lpin;
  _cpin = cpin;
  _bpin = bpin;
  _controlRegister = 0x20 | 0x14; // Pass PWM pin to LED driver
                                  // Set contrast (0-7) to 5
   pinMode(_dpin, OUTPUT);
   pinMode(_lpin, OUTPUT);
   pinMode(_cpin, OUTPUT);  
}

LCD4Pin::LCD4Pin(byte dpin, byte lpin, byte cpin)
{
  _dpin  = dpin;
  _lpin = lpin;
  _cpin = cpin;
  _bpin = 255;
  _controlRegister = 0x14;        // Pass BL SR output to LED driver
                                  // Set contrast (0-7) to 5
   pinMode(_dpin, OUTPUT);
   pinMode(_lpin, OUTPUT);
   pinMode(_cpin, OUTPUT);  
}

void LCD4Pin::begin(byte rows, byte cols, byte font) {
  byte arg;
  _rows = rows % 4;
  _row_offsets[0] = 0x00;
  _row_offsets[0] = 0x40;
  _row_offsets[0] = 0x00 + cols;
  _row_offsets[0] = 0x40 + cols; 
  arg = font & 0x01;
  arg |= rows > 1 ? 2 : 1;
  if(font) _lcdctl |= 0x80;       // Remember 5x10 font for custom chars
  command(0x30 | arg);            // This will set contrast as set above
  command(0x06);                  // Cursor moves right, dipsplay doesn't shift
  backlight(150);                 // Set backlight (0-255) to 150
}
void LCD4Pin::begin(byte rows, byte cols) {
  byte arg;
  _rows = rows % 4;
  _row_offsets[0] = 0x00;
  _row_offsets[0] = 0x40;
  _row_offsets[0] = 0x00 + cols;
  _row_offsets[0] = 0x40 + cols;
  arg = rows > 1 ? 2 : 1;
  command(0x30 | arg);            // This will set contrast as set above
  command(0x06);                  // Cursor moves right, dipsplay doesn't shift
  backlight(150);                 // Set backlight (0-255) to 150
}

// write command/data
void LCD4Pin::xfer2lcd(byte value, byte regsel) 
{
  if(regsel == HIGH) _controlRegister |= RPIN;
  else _controlRegister &= ~RPIN;
  shift16();
  _dataRegister = value;
  shift16();
  xferCmd2lcd();
}

// Send a command to the LCD
void LCD4Pin::command(byte value) {
  xfer2lcd(value, LOW);
}

// Send data to the LCD
void LCD4Pin::data(byte value) {
  xfer2lcd(value, HIGH);
}

// Next 3 are primitives for the Print class
size_t LCD4Pin::write(byte value) {
  xfer2lcd(value, HIGH);
}

size_t LCD4Pin::write(const char *str) {
  while(*str) {
    xfer2lcd(*str, HIGH);
    str++;
  }
}

size_t LCD4Pin::write(const byte *buf, size_t len) {
  while(len) {
    xfer2lcd(buf[--len], HIGH);
  }
}

// Set contrast from 0 (can't see anything), to
// 7 (a bit over the top).
void LCD4Pin::contrast(byte value) {
  _controlRegister &= ~EPIN;
  shift16();
  value &= 7;
  value <<= 2;
  _controlRegister &= B11100011;
  _controlRegister |= value;
  shift16();
}

// Set backlight intensity. In 4 wire mode, sets
// intensity from 0 (off) to 255 (full). In 3 wire
// mode, 0 is off, not 0 is full.
void LCD4Pin::backlight(byte value) {
  if(_bpin != 255) analogWrite(_bpin, value);
  else {
    _controlRegister &= ~EPIN;
    shift16();
    if(value) _controlRegister |= 0x40;
    else _controlRegister &= ~0x40;
    shift16();
  }
}

// Send pulse on the ENABLE line to transfer command
void LCD4Pin::xferCmd2lcd(void) 
{
  _controlRegister &= ~EPIN;
  shift16();
  delayMicroseconds(2);
  _controlRegister |= EPIN;
  shift16();
  delayMicroseconds(2);
  _controlRegister &= ~EPIN;
  shift16();
  delayMicroseconds(37);
}

// Shift 16 bits out to the 4 wire adapter
void LCD4Pin::shift16(void)
{
    digitalWrite(_lpin, LOW);
    shiftOut(_dpin, _cpin, MSBFIRST, _controlRegister);  
    shiftOut(_dpin, _cpin, MSBFIRST, _dataRegister);  
    digitalWrite(_lpin, HIGH);
}

// Clear the LCD and position the cursor to the
// upper-left corner.
void LCD4Pin::clear(void) {
  command(0x01);
}

// Position the cursor (visible of not) to the
// upper-left of the LCD.
void LCD4Pin::home(void) {
  command(0x02);
  delayMicroseconds(1520);
}

// Sets display on/off, cursor on/off,
// and blink on/off. Use ORed combination of:
// LCD_DISPLAY_ON or
// LCD_DISPLAY_OFF
// LCD_CURSOR_ON or
// LCD_CURSOR_OFF
// LCD_BLINK_ON or
// LCD_BLINK_OFF
void LCD4Pin::control(byte mode) {
  mode &= 0x07;
  _lcdctl &= 0x1f;        // Don't overwrite font, dir, or shift flags
  _lcdctl |= mode;
  command(0x08 | mode);
}

// Turn on the display
void LCD4Pin::display() {
  _lcdctl |= LCD_DISPLAY_ON;
  command(0x08 | _lcdctl & 0x01f);
}

// Turn off the display
void LCD4Pin::noDisplay() {
  _lcdctl &= ~LCD_DISPLAY_ON;
  command(0x08 | _lcdctl & 0x01f);
}

// Turn on the blinking cursor
void LCD4Pin::blink() {
  _lcdctl |= LCD_BLINK_ON;
  command(0x08 | _lcdctl & 0x01f);
}

// Turn off the blinking cursor
void LCD4Pin::noBlink() {
  _lcdctl &= ~LCD_BLINK_ON;
  command(0x08 | _lcdctl & 0x01f);
}
// Turn on the cursor
void LCD4Pin::cursor() {
  _lcdctl |= LCD_CURSOR_ON;
  command(0x08 | _lcdctl & 0x01f);
}

// Turn off the cursor
void LCD4Pin::noCursor() {
  _lcdctl &= ~LCD_CURSOR_ON;
  command(0x08 | _lcdctl & 0x01f);
}

// Create a custom character on the LCD. Up to eight
// 5x8 pixel characters are supported (numbered 0 to 7),
// or four 5x10 pixel characters (numbered 0 to 3) if
// LCD_5x10_FONT is set, either with begin() or control().
// Each custom character (chr) is specified by an array
// of bytes (pix), one for each row. The array is eight
// elements for LCD_5x8_FONT, or ten for LCD_5x10_FONT.
void LCD4Pin::createChar(byte chr, const byte *pix) {
  if(_lcdctl & 0x80) {
    chr &= 0x03;
    chr <<= 4;
    command(0x40 | chr);
    write(pix, (size_t) 10);
  } else {
    chr &= 0x07;
    chr <<= 3;
    command(0x40 | chr);
    write(pix, (size_t) 8);
  }
}

// Move the LCD cursor (visible or not) to
// col, row. Use this - setCursor(0, 0) - not home,
// if you haven't messed with scrolling and stuff.
// It's way quicker.
void LCD4Pin::setCursor(byte col, byte row) {
  row &= 3;
  command(0x80 | (col + _row_offsets[row]));
}

// Scroll the contents of the display (text and
// cursor) one space to the left
void LCD4Pin::scrollDisplayLeft(void) {
  command(0x18);
}

// Scroll the contents of the display (text and
// cursor) one space to the right
void LCD4Pin::scrollDisplayRight(void) {
  command(0x1c);
}

// Set the direction for text written to the
// LCD to left-to-right
void LCD4Pin::leftToRight(void) {
  _lcdctl |= 0x40;
  command(0x04 | (_lcdctl >> 6) & 0x03);
}

// Set the direction for text written to the
// LCD to left-to-right
void LCD4Pin::rightToLeft(void) {
  _lcdctl &= ~0x40;
  command(0x04 | (_lcdctl >> 6) & 0x03);
}

// Auto scroll
// If text direction is left-to-right, the display scrolls
// to the left; if direction is right-to-left,
// the display scrolls to the right. 
void LCD4Pin::autoscroll(void) {
  _lcdctl |= 0x20;
  command(0x04 | (_lcdctl >> 6) & 0x03);
}

// Turn off auto scroll above
void LCD4Pin::noAutoscroll(void) {
  _lcdctl &= ~0x20;
  command(0x04 | (_lcdctl >> 6) & 0x03);
}

