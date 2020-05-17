#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"
#include "Renderer.h"

extern Controls controls;
extern UIState uiState;

class PickStepView : public IView {
    public:
        void enable() override {
            // Save current encoder value upon entering this state, because 
            // rotation in this step changes ui state.
            lastEncoderPosition = controls.encoderPosition;
        };

        int lastEncoderPosition = 0;

        void loop() override {
            if (controls.buttonCancel.wasPressed()) {
                uiState.changeToSequence();
                return;
            }

            if (controls.buttonAccept.wasPressed()) {
                uiState.changeToStepOptions();
                return;
            }

            if (lastEncoderPosition <= controls.encoderPosition) {
                uiState.currentlySelectedStep += controls.encoderPosition - lastEncoderPosition;

                if (uiState.currentlySelectedStep >= SEQUENCER_STEPS_AMOUNT) {
                    uiState.currentlySelectedStep = SEQUENCER_STEPS_AMOUNT - 1; // steps are 0-based
                }

                lastEncoderPosition = controls.encoderPosition;
                // TODO: > instead of >=
            } else if (lastEncoderPosition >= controls.encoderPosition) {
                uiState.currentlySelectedStep -= lastEncoderPosition - controls.encoderPosition;

                if (uiState.currentlySelectedStep < 0) {
                    uiState.currentlySelectedStep = 0;
                }

                lastEncoderPosition = controls.encoderPosition;
            }
        };

        void print() override {
            byte page = sequencer.getPageOfStep(uiState.currentlySelectedStep);

            renderSequencer(page);
            renderHeader("PICK STEP");
            
            byte stepOffset = SEQUENCER_STEPS_PER_PAGE * (page - 1);
            printStepUnderline(uiState.currentlySelectedStep - stepOffset);
        };
};