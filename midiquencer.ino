#include <JC_Button.h>
#include <LiquidCrystal.h>

// #define ENCODER_OPTIMIZE_INTERRUPTS
// todo probably not needed
// #define ENCODER_USE_INTERRUPTS
#include <Encoder.h>

#include "Sequencer.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 1, d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String notes[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

// encoder
// volatile int lastEncoding = 0;
#define ENCODER_PIN_A 3
#define ENCODER_PIN_B 2
// Encoder precision. One step equals 4 in the internal counter.
// We do the maths based on ENCODER_PRECISION, setting encoderPosition with respect
// to precision.
#define ENCODER_FULL_STEP 4 // One encoder step will change encoderPosition by 1
#define ENCODER_HALF_STEP 2 // Half encoder step will change encoderPosition by 1
#define ENCODER_QUARTER_STEP 1 // Quarter encoder step will change encoderPosition by 1 (default)
#define ENCODER_PRECISION ENCODER_FULL_STEP // change the precision here
Encoder encoder(ENCODER_PIN_B, ENCODER_PIN_A);
int encoderPosition = -999;
// Used by ui_loop_Sequence to save the last encoderPosition,
// which is then used for rotation detection to go to UI_STATE_PICK_STEP.
int encoderLastPosition;

#define BUTTON_ACCEPT_PIN 6
#define BUTTON_CANCEL_PIN 7
Button buttonAccept(BUTTON_ACCEPT_PIN);
Button buttonCancel(BUTTON_CANCEL_PIN);

// ui 
#define UI_STATE_SEQUENCE 0
#define UI_STATE_PICK_STEP 1
#define UI_STATE_STEP_OPTIONS 2
int uiState = UI_STATE_SEQUENCE;

Sequencer sequencer;

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    lcd.begin(16, 2);

    buttonAccept.begin();
    buttonCancel.begin();

    sequencer.begin();
}

void loop()
{
    readEncoder();
    readButtonAccept();
    readButtonCancel();

    if (buttonAccept.wasPressed()) {
        Serial.println("ACCEPT");
    }

    if (buttonCancel.wasPressed()) {
        Serial.println("CANCEL");
    }

    ui_loop();

    sequencer.loop();
}

void ui_loop()
{
    if (uiState == UI_STATE_SEQUENCE) {
        ui_loop_Sequence();
    } else if (uiState == UI_STATE_PICK_STEP) {
        ui_loop_PickStep();
    } else if (uiState == UI_STATE_STEP_OPTIONS) {
        ui_loop_StepOptions();
    }

    ui_print();
}

void ui_state_Sequence() {
    // Save current encoder value upon entering this state, because 
    // rotation in this step changes ui state.
    encoderLastPosition = encoderPosition;
    uiState = UI_STATE_SEQUENCE;
}

void ui_state_PickStep() {
    uiState = UI_STATE_PICK_STEP;
}

void ui_state_StepOptions() {
    uiState = UI_STATE_STEP_OPTIONS;
}

void ui_loop_Sequence() {
    if (encoderPosition != encoderLastPosition) {
        ui_state_PickStep();
    }

    encoderLastPosition = encoderPosition;
}

int pickStep_currentlySelectedStep = 0;
int pickStep_currentlySelectedStepEncoderPosition = 0;

void ui_loop_PickStep() {
    if (buttonCancel.wasPressed()) {
        ui_state_Sequence();
        return;
    }

    if (buttonAccept.wasPressed()) {
        ui_state_StepOptions();
        return;
    }

    if (pickStep_currentlySelectedStepEncoderPosition <= encoderPosition) {
        pickStep_currentlySelectedStep += encoderPosition - pickStep_currentlySelectedStepEncoderPosition;

        if (pickStep_currentlySelectedStep >= SEQUENCER_STEPS_AMOUNT) {
            pickStep_currentlySelectedStep = SEQUENCER_STEPS_AMOUNT - 1; // steps are 0-based
        }        

        pickStep_currentlySelectedStepEncoderPosition = encoderPosition;
    } else if (pickStep_currentlySelectedStepEncoderPosition >= encoderPosition) {
        pickStep_currentlySelectedStep -= pickStep_currentlySelectedStepEncoderPosition - encoderPosition;

        if (pickStep_currentlySelectedStep < 0) {
            pickStep_currentlySelectedStep = 0;
        }

        pickStep_currentlySelectedStepEncoderPosition = encoderPosition;
    }
}

void ui_loop_StepOptions() {
    if (buttonCancel.wasPressed()) {
        ui_state_PickStep();
        return;
    }
}

unsigned long uiLastPrintMillis = 0;

void ui_print() 
{
    if ((millis() - uiLastPrintMillis) <= 100) {
        return;
    }
    
    uiLastPrintMillis = millis();

    if (uiState == UI_STATE_SEQUENCE) {
        ui_print_Sequence();
    } else if (uiState == UI_STATE_PICK_STEP) {
        ui_print_PickStep();
    } else if (uiState == UI_STATE_STEP_OPTIONS) {
        ui_print_StepOptions();
    }
}



void ui_print_Sequence() {
    byte page = sequencer.page;
    byte limit = SEQUENCER_STEPS_PER_PAGE * page;
    byte start = limit - SEQUENCER_STEPS_PER_PAGE;

    String row1 = "SEQUENCE      P";
    row1 += page;
    String row2;

    for (byte i = start; i < limit; i++) {
        if (sequencer.currentStep == i) {
            row2 += 'X';
        } else {
            row2 += 'O';
        }
    }
    
    lcdPadPrint(row1, 0);
    lcdPadPrint(row2, 1);
}

void ui_print_PickStep() {
    byte page = sequencer.getPageOfStep(pickStep_currentlySelectedStep);
    byte limit = SEQUENCER_STEPS_PER_PAGE * page;
    byte start = limit - SEQUENCER_STEPS_PER_PAGE;

    String row1 = "PICK STEP     P";
    row1 += page;
    String row2;

    for (byte i = start; i < limit; i++) {
        if (pickStep_currentlySelectedStep == i) {
            row2 += 'Y';
        } else if (sequencer.currentStep == i) {
            row2 += 'X';
        } else {
            row2 += 'O';
        }
    }
    
    lcdPadPrint(row1, 0);
    lcdPadPrint(row2, 1);
}

void ui_print_StepOptions() {
    lcdPadPrint(" StepOptions", 1);
}

void lcdPadPrint(String text, byte row) {
    lcd.setCursor(0, row);

    byte length = lcd.print(text);

    for (byte i = length; i < 16; i++) {
        lcd.write(' ');
    }
}

void readEncoder()
{
    long newPosition = encoder.read();

    if (newPosition % ENCODER_PRECISION == 0) {
        long scaledNewPosition = newPosition / ENCODER_PRECISION;

        if (scaledNewPosition != encoderPosition) {
            encoderPosition = scaledNewPosition;
        }
    }
}

void readButtonAccept()
{
    buttonAccept.read();
}

void readButtonCancel()
{
    buttonCancel.read();
}
