#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"
#include "Renderer.h"

#define MIN_VELOCITY 0
#define MAX_VELOCITY 127

extern Controls controls;
extern UIState uiState;

class StepOptionsView : public IView {
    public:
        StepOptions* options;

        void enable() override {
            option = STEP_OPTIONS_CHOOSE_OPTION;
            // Set to enable so that the option is reset after returning to this view
            highlightedOption = STEP_OPTIONS_ENABLE;

            lastEncoderPosition = controls.encoderPosition;

            options = &sequencer.steps[uiState.currentlySelectedStep];
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
                            options->enabled = !options->enabled;
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
                    options->note += controls.encoderPosition - lastEncoderPosition;

                    if (options->note >= 127) {
                        options->note = 127;
                    }        

                    lastEncoderPosition = controls.encoderPosition;
                } else if (lastEncoderPosition > controls.encoderPosition) {
                    byte lastNote = options->note;
                    options->note -= lastEncoderPosition - controls.encoderPosition;

                    // overflow
                    if (options->note > lastNote) {
                        options->note = 0;
                    }

                    lastEncoderPosition = controls.encoderPosition;
                }
            } else if (option == STEP_OPTIONS_VELOCITY) {
                if (lastEncoderPosition <= controls.encoderPosition) {
                    options->velocity += controls.encoderPosition - lastEncoderPosition;

                    if (options->velocity >= 127) {
                        options->velocity = 127;
                    }

                    lastEncoderPosition = controls.encoderPosition;
                } else if (lastEncoderPosition > controls.encoderPosition) {
                    byte lastVelocity = options->velocity;
                    options->velocity -= lastEncoderPosition - controls.encoderPosition;

                    // overflow
                    if (options->velocity > lastVelocity) {
                        options->velocity = 0;
                    }

                    lastEncoderPosition = controls.encoderPosition;
                }
            }
        }

        void print() override {
            renderStepOptions(options, option, highlightedOption);
        };

    private:
        int lastEncoderPosition = 0;
        byte option = STEP_OPTIONS_CHOOSE_OPTION;
        byte highlightedOption = STEP_OPTIONS_ENABLE;
};