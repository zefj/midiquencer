#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"

#define STEP_OPTIONS_CHOOSE_OPTION 0
#define STEP_OPTIONS_ENABLE 1
#define STEP_OPTIONS_NOTE 2
#define STEP_OPTIONS_VELOCITY 3

#define MIN_VELOCITY 0
#define MAX_VELOCITY 127

extern Controls controls;
extern UIState uiState;

class StepOptionsView : public IView {
    public:
        byte enabled;
        byte note;
        byte velocity;
        
        void enable() override {
            // Set to enable so that the option is reset after returning to this view
            highlightedOption = STEP_OPTIONS_ENABLE;

            lastEncoderPosition = controls.encoderPosition;
            // prepopulate settings
            enabled = sequencer.steps[uiState.currentlySelectedStep][0];
            note = sequencer.steps[uiState.currentlySelectedStep][1];
            velocity = sequencer.steps[uiState.currentlySelectedStep][2];
        };

        void loop() override {
            if (option == STEP_OPTIONS_CHOOSE_OPTION) {
                if (controls.buttonCancel.wasPressed()) {
                    uiState.changeToPickStep();
                    return;
                }

                if (controls.buttonAccept.wasPressed()) {
                    if (highlightedOption == STEP_OPTIONS_ENABLE) {

                        // You cannot go into STEP_OPTIONS_ENABLE        
                        if (controls.buttonAccept.wasPressed()) {
                            enabled = !enabled;
                            saveStepOptions();
                            option = STEP_OPTIONS_CHOOSE_OPTION;
                        }

                        if (controls.buttonCancel.wasPressed()) {
                            option = STEP_OPTIONS_CHOOSE_OPTION;
                        }

                    } else if (highlightedOption == STEP_OPTIONS_NOTE) {
                        option = STEP_OPTIONS_NOTE;
                    } else if (highlightedOption == STEP_OPTIONS_VELOCITY) {
                        option = STEP_OPTIONS_VELOCITY;
                    }
                }
            } else if (
                option == STEP_OPTIONS_NOTE
                || option == STEP_OPTIONS_VELOCITY
            ) {
                if (controls.buttonAccept.wasPressed()) {
                    enabled = !enabled;
                    saveStepOptions();
                    option = STEP_OPTIONS_CHOOSE_OPTION;
                }

                if (controls.buttonCancel.wasPressed()) {
                    option = STEP_OPTIONS_CHOOSE_OPTION;
                }
            }

            handleEncoderRotation();
        };

        void handleEncoderRotation() {
            if (option == STEP_OPTIONS_CHOOSE_OPTION) {
                // There is no encoder position arithmetics here 
                if (lastEncoderPosition < controls.encoderPosition) {
                    highlightedOption += 1;                    
                } else if (lastEncoderPosition > controls.encoderPosition) {
                    highlightedOption -= 1;                    
                }

                if (highlightedOption < STEP_OPTIONS_ENABLE) {
                    highlightedOption = STEP_OPTIONS_ENABLE;
                }

                if (highlightedOption > STEP_OPTIONS_VELOCITY) {
                    highlightedOption = STEP_OPTIONS_VELOCITY;
                }

                lastEncoderPosition = controls.encoderPosition;
            } else if (option == STEP_OPTIONS_NOTE) {
                if (lastEncoderPosition <= controls.encoderPosition) {
                    note += controls.encoderPosition - lastEncoderPosition;

                    if (note >= 127) {
                        note = 127;
                    }        

                    lastEncoderPosition = controls.encoderPosition;
                } else if (lastEncoderPosition > controls.encoderPosition) {
                    byte lastNote = note;
                    note -= lastEncoderPosition - controls.encoderPosition;

                    // overflow
                    if (note > lastNote) {
                        note = 0;
                    }

                    lastEncoderPosition = controls.encoderPosition;
                }
            } else if (option == STEP_OPTIONS_VELOCITY) {
                if (lastEncoderPosition <= controls.encoderPosition) {
                    velocity += controls.encoderPosition - lastEncoderPosition;

                    if (velocity >= 127) {
                        velocity = 127;
                    }        

                    lastEncoderPosition = controls.encoderPosition;
                } else if (lastEncoderPosition > controls.encoderPosition) {
                    byte lastVelocity = velocity;
                    velocity -= lastEncoderPosition - controls.encoderPosition;

                    // overflow
                    if (velocity > lastVelocity) {
                        velocity = 0;
                    }

                    lastEncoderPosition = controls.encoderPosition;
                }
            }
        }

        void print() override {
            int currentlySelectedStep = uiState.currentlySelectedStep;
            byte* stepSettings = sequencer.steps[currentlySelectedStep];

            char row1Buffer[16];

            if (option == STEP_OPTIONS_NOTE) {
                lcdPrint((char*)"SET NOTE", 0);
                sprintf(row1Buffer, "%d, >%d %d", enabled, note, velocity);
            } else if (option == STEP_OPTIONS_VELOCITY) {
                lcdPrint((char*)"SET VELOCITY", 0);
                sprintf(row1Buffer, "%d, %d >%d", enabled, note, velocity);
            } else if (option == STEP_OPTIONS_CHOOSE_OPTION) {
                if (highlightedOption == STEP_OPTIONS_ENABLE) {
                    lcdPrint((char*)"1. ENABLE", 0);
                } else if (highlightedOption == STEP_OPTIONS_NOTE) {
                    lcdPrint((char*)"2. NOTE", 0);
                } else if (highlightedOption == STEP_OPTIONS_VELOCITY) {
                    lcdPrint((char*)"3. VELOCITY", 0);
                }

                sprintf(row1Buffer, "%d, %d %d", stepSettings[0], stepSettings[1], stepSettings[2]);
            }

            lcdPrint(row1Buffer, 1);
        };

    private:
        int lastEncoderPosition = 0;
        byte option = STEP_OPTIONS_CHOOSE_OPTION;
        byte highlightedOption = STEP_OPTIONS_ENABLE;

        void saveStepOptions() {
            sequencer.steps[uiState.currentlySelectedStep][0] = enabled;
            sequencer.steps[uiState.currentlySelectedStep][1] = note;
            sequencer.steps[uiState.currentlySelectedStep][2] = velocity;
        }
};