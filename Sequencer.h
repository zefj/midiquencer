#if !defined(SEQUENCER_STEPS_AMOUNT)
#define SEQUENCER_STEPS_AMOUNT 64
#endif

#define SEQUENCER_STEPS_PER_PAGE 16

#include <Arduino.h>

class Sequencer
{
    public:
        int bpm;
        byte currentStep = 0;
        byte page = 1;

        // enabled, note, velocity
        char steps[SEQUENCER_STEPS_AMOUNT][3] = {
            // {1, 0, 0},
            // {1, 63, 120},
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

        void begin();

        void setBPM(int newBPM);

        byte getPageOfStep(byte step);

        void loop();
    private:
        unsigned long lastBeatMillis = 0;
        int beatInterval;

        int calculateBeatInverval();

        bool shouldBeat();

        void handleBeat();

        void incrementStep();

        void playStep();
};