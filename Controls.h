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

#define BUTTON_ACCEPT_PIN PA5
#define BUTTON_CANCEL_PIN PA4
#define BUTTON_DEBOUNCE 25
#define BUTTON_LONG_PRESS 1500

class Controls
{
    public:
        Button buttonAccept = Button(BUTTON_ACCEPT_PIN, BUTTON_DEBOUNCE, false, false);
        Button buttonCancel = Button(BUTTON_CANCEL_PIN, BUTTON_DEBOUNCE, false, false);
        
        int16_t encoderPosition = 0;
        
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

            if (buttonAccept.wasReleased()) {
                Serial.println("Accept");
            }

            if (buttonCancel.wasReleased()) {
                Serial.println("Cancel");
            }

            if (buttonAccept.pressedFor(BUTTON_LONG_PRESS)) {
                Serial.println("Accept long press");
            }

            if (buttonCancel.pressedFor(BUTTON_LONG_PRESS)) {
                Serial.println("Cancel long press");
            }
        }

        int16_t lastEncoderPosition = 0;
        int16_t linearRotationHelper(int16_t currentValue, int16_t minValue, int16_t maxValue)
        {
            int16_t lastValue = currentValue;
            int16_t newValue = currentValue;

            if (lastEncoderPosition <= encoderPosition) {
                newValue += encoderPosition - lastEncoderPosition;

                //                          overflow
                if (newValue >= maxValue || newValue < lastValue) {
                    newValue = maxValue;
                }
            } else if (lastEncoderPosition > encoderPosition) {
                newValue -= lastEncoderPosition - encoderPosition;

                //                         underflow
                if (newValue < minValue || newValue > lastValue) {
                    newValue = minValue;
                }
            }

            lastEncoderPosition = encoderPosition;
            return newValue;
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