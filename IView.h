#ifndef IVIEW_H
#define IVIEW_H

#include <Arduino.h>

#include <LiquidCrystal.h>
extern LiquidCrystal lcd;

class IView
{
    public:
        virtual void enable();
        virtual void loop();
        virtual void print();

    protected:
        void lcdPadPrint(String text, byte row) {
            lcd.setCursor(0, row);

            byte length = lcd.print(text);

            for (byte i = length; i < 16; i++) {
                lcd.write(' ');
            }
        }
};

#endif