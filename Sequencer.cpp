#include "Sequencer.h"

void Sequencer::begin()
{
    setBPM(80);
}

void Sequencer::setBPM(int newBPM)
{
    bpm = newBPM;
    beatInterval = calculateBeatInverval();
}

int Sequencer::calculateBeatInverval()
{
    return (int)(60.0 / (float)bpm * 1000.0);
}

byte Sequencer::getPageOfStep(byte step) 
{
    return 1 + floor(step/SEQUENCER_STEPS_PER_PAGE);
}

bool Sequencer::shouldBeat()
{
    return millis() - lastBeatMillis > beatInterval;
}

void Sequencer::handleBeat()
{
    incrementStep();
    playStep();
    lastBeatMillis = millis();
}

void Sequencer::incrementStep()
{
    currentStep = currentStep + 1;

    if (currentStep > SEQUENCER_STEPS_AMOUNT - 1) {
        currentStep = 0;
    }

    page = 1 + floor(currentStep/SEQUENCER_STEPS_PER_PAGE);
}

void Sequencer::playStep()
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

void Sequencer::loop()
{
    if (!shouldBeat()) {
        return;
    }

    handleBeat();
}