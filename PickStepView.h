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

        int16_t lastEncoderPosition = 0;

        void loop() override {
            if (controls.buttonCancel.wasReleased()) {
                uiState.changeToSequence();
                return;
            }

            if (controls.buttonAccept.wasReleased()) {
                uiState.changeToStepOptions();
                return;
            }

            uiState.currentlySelectedStep = controls.linearRotationHelper(uiState.currentlySelectedStep, 0, sequencer.stepsAmount - 1);
        };

        void print() override {
            byte page = sequencer.getPageOfStep(uiState.currentlySelectedStep);

            renderSequencer(page);
            renderHeader("PICK STEP");
            
            byte stepOffset = SEQUENCER_STEPS_PER_PAGE * (page - 1);
            printStepUnderline(uiState.currentlySelectedStep - stepOffset);
        };
};