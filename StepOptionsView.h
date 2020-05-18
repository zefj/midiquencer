#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"
#include "Renderer.h"

#define MIN_VELOCITY 0
#define MAX_VELOCITY 127

extern Controls controls;
extern UIState uiState;

// TODO refactor this with switch cases and the encoder helpers like SequencerOptionsView
class StepOptionsView : public IView {
    public:
        StepOptions* options;

        void enable() override {
            option = STEP_OPTIONS_CHOOSE_OPTION;
            // Set to enable so that the option is reset after returning to this view
            highlightedOption = STEP_OPTIONS_ENABLE;

            lastEncoderPosition = controls.encoderPosition;

            // There is no way for the bank to change while in options, so this can remain in enable()
            options = &sequencer.stepOptions[uiState.bank][uiState.currentlySelectedStep];
        };

        void loop() override {
            if (option == STEP_OPTIONS_CHOOSE_OPTION) {
                if (controls.buttonCancel.wasReleased()) {
                    uiState.changeToPickStep();
                    return;
                }

                if (controls.buttonAccept.wasReleased()) {
                    if (highlightedOption == STEP_OPTIONS_ENABLE) {

                        // You cannot go into STEP_OPTIONS_ENABLE        
                        if (controls.buttonAccept.wasReleased()) {
                            options->enabled = !options->enabled;
                            option = STEP_OPTIONS_CHOOSE_OPTION;
                        }

                        if (controls.buttonCancel.wasReleased()) {
                            option = STEP_OPTIONS_CHOOSE_OPTION;
                        }

                    } else {
                        option = highlightedOption;
                    }
                }
            } else if (
                option == STEP_OPTIONS_NOTE
                || option == STEP_OPTIONS_VELOCITY
            ) {
                if (controls.buttonAccept.wasReleased()) {
                    option = STEP_OPTIONS_CHOOSE_OPTION;
                }

                if (controls.buttonCancel.wasReleased()) {
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

                    // underflow
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

                    // underflow
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
        int16_t lastEncoderPosition = 0;
        byte option = STEP_OPTIONS_CHOOSE_OPTION;
        byte highlightedOption = STEP_OPTIONS_ENABLE;
};