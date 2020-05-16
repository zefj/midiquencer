#ifndef CONTROLS_H
#define CONTROLS_H

#include <Arduino.h>

#include <JC_Button.h>
#include <Encoder.h>

#define ENCODER_PIN_A 0
#define ENCODER_PIN_B 1
// Encoder precision. One step equals 4 in the internal counter.
// We do the maths based on ENCODER_PRECISION, setting encoderPosition with respect
// to precision.
#define ENCODER_FULL_STEP 4 // One encoder step will change encoderPosition by 1
#define ENCODER_HALF_STEP 2 // Half encoder step will change encoderPosition by 1
#define ENCODER_QUARTER_STEP 1 // Quarter encoder step will change encoderPosition by 1 (default)
#if !defined(ENCODER_PRECISION)
#define ENCODER_PRECISION ENCODER_FULL_STEP // change the precision here
#endif

#define BUTTON_ACCEPT_PIN 8
#define BUTTON_CANCEL_PIN 9
#define BUTTON_DEBOUNCE 250

class Controls
{
    public:
        Encoder encoder = Encoder(ENCODER_PIN_B, ENCODER_PIN_A);
        Button buttonAccept = Button(BUTTON_ACCEPT_PIN, BUTTON_DEBOUNCE);
        Button buttonCancel = Button(BUTTON_CANCEL_PIN, BUTTON_DEBOUNCE);
        
        int encoderPosition = 0;

        void begin()
        {
            buttonAccept.begin();
            buttonCancel.begin();
        }

        void loop()
        {
            readEncoder();
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
        void readEncoder() {
            long newPosition = encoder.read();

            if (newPosition % ENCODER_PRECISION == 0) {
                long scaledNewPosition = newPosition / ENCODER_PRECISION;

                if (scaledNewPosition != encoderPosition) {
                    encoderPosition = scaledNewPosition;
                }
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