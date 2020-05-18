#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <HardwareTimer.h>

#define SEQUENCER_STEPS_PER_PAGE 8

#define MIN_STEPS 8
#define MAX_STEPS 64

#define MIN_BPM 50
#define MAX_BPM 250

#define BANKS_AMOUNT 4

const char* NOTES[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

struct StepOptions {
    byte enabled = 0;
    byte note = 60; // C4
    byte velocity = 127; // Max velocity by default
};

class Sequencer
{
    public:
        uint8_t bpm;
        byte currentStep = 0;
        byte page = 1;
        bool isPlaying = 0;
        uint8_t stepsAmount = MAX_STEPS;

        // enabled, note, velocity
        struct StepOptions stepOptions[BANKS_AMOUNT][MAX_STEPS] = {
            {1, 63, 120},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 72, 69},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 50, 43},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0},
            // {1, 0, 0}
        };

        uint8_t bankChannels[4] = {
            1,
            1,
            1,
            1
        };

        void begin()
        {
            setBPM(120);
        }

        void setBPM(uint8_t newBPM)
        {
            if (bpm != newBPM) {
                bpm = newBPM;
                beatInterval = calculateBeatInverval();
            }
        }

        void setSteps(uint8_t newStepsAmount)
        {
            if (stepsAmount != newStepsAmount) {
                stepsAmount = newStepsAmount;
                reset();
            }
        }

        byte getPageOfStep(byte step) 
        {
            return 1 + floor(step/SEQUENCER_STEPS_PER_PAGE);
        }

        void play()
        {
            isPlaying = 1;
        }

        void stop()
        {
            isPlaying = 0;
            lastBeatMillis = 0;
        }

        void reset()
        {
            currentStep = 0;
            lastBeatMillis = 0;
        }

        bool shouldBeat()
        {
            return (
                lastBeatMillis == 0
                || millis() - lastBeatMillis >= beatInterval
            );
        }

        void handleBeat()
        {
            Serial.print("Handling beat, current step: ");
            Serial.print(currentStep);
            Serial.print(". Beat precision: ");
            Serial.print(millis() - lastBeatMillis);
            Serial.println();

            incrementStep();
            playStep();
            lastBeatMillis = millis();
        }
    private:
        unsigned long lastBeatMillis = 0;
        uint16_t beatInterval;

        uint16_t calculateBeatInverval()
        {
            Serial.print("New beat interval: "); Serial.print((uint16_t)(60.0 / (float)bpm * 1000.0)); Serial.println();
            return (uint16_t)(60.0 / (float)bpm * 1000.0);
        }

        void incrementStep()
        {
            currentStep = currentStep + 1;

            if (currentStep > stepsAmount - 1) {
                currentStep = 0;
            }

            page = 1 + floor(currentStep/SEQUENCER_STEPS_PER_PAGE);
        }

        void playStep()
        {
            byte enabled = stepOptions[uiState.bank][currentStep].enabled;
            byte note = stepOptions[uiState.bank][currentStep].note;
            byte velocity = stepOptions[uiState.bank][currentStep].velocity;

            // if (enabled) {
            //     Serial.print(millis());
            //     Serial.print(" STEP ");
            //     Serial.print(currentStep, 10);
            //     Serial.print(", ");
            //     Serial.print(note, 10);
            //     Serial.print(", ");
            //     Serial.print(velocity, 10);
            //     Serial.println();
            // }
        }
};

#endif
