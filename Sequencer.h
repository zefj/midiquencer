#ifndef SEQUENCER_H
#define SEQUENCER_H

#if !defined(SEQUENCER_STEPS_AMOUNT)
#define SEQUENCER_STEPS_AMOUNT 64
#endif

#define SEQUENCER_STEPS_PER_PAGE 8

class Sequencer
{
    public:
        int bpm;
        byte currentStep = 0;
        byte page = 1;
        bool isPlaying = 0;

        // enabled, note, velocity
        byte steps[SEQUENCER_STEPS_AMOUNT][3] = {
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

        void loop()
        {
            if (!isPlaying) {
                return;
            }

            if (!shouldBeat()) {
                return;
            }

            handleBeat();
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
    private:
        unsigned long lastBeatMillis = 0;
        int beatInterval;

        int calculateBeatInverval()
        {
            return (int)(60.0 / (float)bpm * 1000.0);
        }

        bool shouldBeat()
        {
            return (
                lastBeatMillis == 0
                || millis() - lastBeatMillis > beatInterval
            );
        }

        void handleBeat()
        {
            incrementStep();
            playStep();
            lastBeatMillis = millis();
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
            char enabled = steps[currentStep][0];
            char note = steps[currentStep][1];
            char velocity = steps[currentStep][2];

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