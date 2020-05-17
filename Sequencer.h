#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <HardwareTimer.h>

#if !defined(SEQUENCER_STEPS_AMOUNT)
#define SEQUENCER_STEPS_AMOUNT 64
#endif

#define SEQUENCER_STEPS_PER_PAGE 8

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
        int bpm;
        byte currentStep = 0;
        byte page = 1;
        bool isPlaying = 0;

        // enabled, note, velocity
        struct StepOptions steps[SEQUENCER_STEPS_AMOUNT] = {
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

        void begin()
        {
            setBPM(120);
        }

        void setBPM(int newBPM)
        {
            bpm = newBPM;
            beatInterval = calculateBeatInverval();
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
        int beatInterval;

        int calculateBeatInverval()
        {
            return (int)(60.0 / (float)bpm * 1000.0);
        }

        void incrementStep()
        {
            currentStep = currentStep + 1;

            if (currentStep > SEQUENCER_STEPS_AMOUNT - 1) {
                currentStep = 0;
            }

            page = 1 + floor(currentStep/SEQUENCER_STEPS_PER_PAGE);
        }

        void playStep()
        {
            byte enabled = steps[currentStep].enabled;
            byte note = steps[currentStep].note;
            byte velocity = steps[currentStep].velocity;

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
