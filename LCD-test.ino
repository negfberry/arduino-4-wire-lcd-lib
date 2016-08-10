#include <LCD4Pin.h>

// No dimmable backlight control, just on or
// off - specify 3 pins only
LCD4Pin lcd(8, 9, 10);
// Backlight fully controlled - specify
// fourth pin for backlight brightness
// (pin must be PWM capable)
LCD4Pin lcd_bl(5, 6, 7, 11);

void setup() {
  // 4x20 LCD, with 5x8 characters (default)
  lcd.begin(4, 20);
  // 1x16 LCD, with 5x10 characters. 5x10
  // is only supported on one line of a display.
  lcd_bl.begin(1, 16, LCD_5x10_FONT);
}

void loop() {
  static int n;

  lcd.clear();
  lcd_bl.clear();
  lcd.backlight(200);     // anything from 1-255 would do.
  lcd_bl.backlight(200);
  // Blinking cursor on for the 4x20
  // Do it one way here
  lcd.control(LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINK_ON);
  // No cursor on for the 1x16
  // And another way here
  lcd_bl.noCursor();
  lcd.setCursor(1, 5);
  lcd.print("Hello World!");
  lcd.setCursor(2, 9);
  lcd.print(n++);
  lcd_bl.setCursor(0, 2);
  lcd_bl.print(millis());
  delay(1000);
}