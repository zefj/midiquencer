#ifndef CONTROLS_H
#define CONTROLS_H

#include <JC_Button.h>

#include "RotaryEncoder.h"
#define ENCODER_PIN_CLK PA8
#define ENCODER_PIN_DATA PA9
#define ENCODER_PIN_BUTTON PA10

RotaryEncoder encoder = RotaryEncoder(ENCODER_PIN_DATA, ENCODER_PIN_CLK);

void encoderISR()
{
    encoder.read();
}

#define BUTTON_ACCEPT_PIN PB11
#define BUTTON_CANCEL_PIN PB10
#define BUTTON_DEBOUNCE 250

class Controls
{
    public:
        Button buttonAccept = Button(BUTTON_ACCEPT_PIN, BUTTON_DEBOUNCE);
        Button buttonCancel = Button(BUTTON_CANCEL_PIN, BUTTON_DEBOUNCE);
        
        int encoderPosition = 0;

        void begin()
        {
            buttonAccept.begin();
            buttonCancel.begin();

            pinMode(ENCODER_PIN_CLK, INPUT_PULLUP);
            pinMode(ENCODER_PIN_DATA, INPUT_PULLUP);

            attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_CLK), encoderISR, CHANGE);  //call encoderISR() every high->low or low->high changes
            attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_DATA), encoderISR, CHANGE);  //call encoderISR() every high->low or low->high changes
        }

        void loop()
        {
            updateEncoderPosition();
            readButtonAccept();
            readButtonCancel();

            if (buttonAccept.wasPressed()) {
                Serial.println("Accept");
            }


            if (buttonCancel.wasPressed()) {
                Serial.println("Cancel");
            }
        }
    private:
        int16_t previousPosition = 0;

        void updateEncoderPosition() {
            int16_t newPosition = encoder.getPosition();

            if (encoderPosition != newPosition) {
                encoderPosition = newPosition;
                Serial.println(newPosition);
            }
        }

        void readButtonAccept() {
            buttonAccept.read();
        }

        void readButtonCancel() {
            buttonCancel.read();
        }
};

#endif