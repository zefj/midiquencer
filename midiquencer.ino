#include <JC_Button.h>
#include <LiquidCrystal.h>

// #define ENCODER_OPTIMIZE_INTERRUPTS
// todo probably not needed
// #define ENCODER_USE_INTERRUPTS
#include <Encoder.h>

#define STEPS_NUMBER 32

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 1, d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned char bpm = 40;
unsigned char currentStep = 0;
unsigned long lastBeatMillis = 0;

// enabled, note, velocity
char steps[32][3] = {
    {1, 0, 0},
    {1, 63, 120},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 72, 69},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 50, 43},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0}
};

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
char uiState = UI_STATE_SEQUENCE;

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    lcd.begin(16, 2);

    buttonAccept.begin();
    buttonCancel.begin();
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

    if (!shouldBeat()) {
        return;
    }

    handleBeat();
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

char pickStep_currentlySelectedStep = 0;
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

        if (pickStep_currentlySelectedStep > 31) {
            pickStep_currentlySelectedStep = 31;
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

char getPageOfStep(char *step) {
    if (step < 16) {
        return 1;
    }

    return 2;
}

void ui_print_Sequence() {
    char page = getPageOfStep(currentStep);
    char start = 0;
    char limit = 16;

    if (page == 2) {
        start = 16;
        limit = 32;
    }

    String row1;
    if (page == 1) {
        row1 = "SEQUENCE      P1";
    } else {
        row1 = "SEQUENCE      P2";
    }

    String row2;

    for (int i = start; i < limit; i++) {
        if (currentStep == i) {
            row2 += 'X';
        } else {
            row2 += 'O';
        }
    }
    
    lcdPadPrint(row1, 0);
    lcdPadPrint(row2, 1);
}

void ui_print_PickStep() {
    char page = getPageOfStep(pickStep_currentlySelectedStep);
    char start = 0;
    char limit = 16;

    if (page == 2) {
        start = 16;
        limit = 32;
    }

    String row1;
    if (page == 1) {
        row1 = "PICK STEP     P1";
    } else {
        row1 = "PICK STEP     P2";
    }

    String row2;

    for (int i = start; i < limit; i++) {
        if (pickStep_currentlySelectedStep == i) {
            row2 += 'Y';
        } else if (currentStep == i) {
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

void lcdPadPrint(String text, char row) {
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

//todo remember to calculate it once after tempo change
int getBeatInterval()
{
    return (int)(60.0 / (float)bpm * 1000.0);
}

bool shouldBeat()
{
    return millis() - lastBeatMillis > getBeatInterval();
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

    if (currentStep > STEPS_NUMBER - 1) {
        currentStep = 0;
    }
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