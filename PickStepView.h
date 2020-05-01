#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"

extern Controls controls;
extern UIState uiState;

class PickStepView : public IView {
    public:
        void enable() override {
            // Save current encoder value upon entering this state, because 
            // rotation in this step changes ui state.
            encoderLastPosition = controls.encoderPosition;
        };

        int currentlySelectedStep = 0;
        int currentlySelectedStepEncoderPosition = 0;

        void loop() override {
            if (controls.buttonCancel.wasPressed()) {
                uiState.changeToSequence();
                return;
            }

            if (controls.buttonAccept.wasPressed()) {
                uiState.changeToStepOptions();
                return;
            }

            if (currentlySelectedStepEncoderPosition <= controls.encoderPosition) {
                currentlySelectedStep += controls.encoderPosition - currentlySelectedStepEncoderPosition;

                if (currentlySelectedStep >= SEQUENCER_STEPS_AMOUNT) {
                    currentlySelectedStep = SEQUENCER_STEPS_AMOUNT - 1; // steps are 0-based
                }        

                currentlySelectedStepEncoderPosition = controls.encoderPosition;
            } else if (currentlySelectedStepEncoderPosition >= controls.encoderPosition) {
                currentlySelectedStep -= currentlySelectedStepEncoderPosition - controls.encoderPosition;

                if (currentlySelectedStep < 0) {
                    currentlySelectedStep = 0;
                }

                currentlySelectedStepEncoderPosition = controls.encoderPosition;
            }
        };

        void print() override {
            byte page = sequencer.getPageOfStep(currentlySelectedStep);
            byte limit = SEQUENCER_STEPS_PER_PAGE * page;
            byte start = limit - SEQUENCER_STEPS_PER_PAGE;

            String row1 = "PICK STEP     P";
            row1 += page;
            String row2;

            for (byte i = start; i < limit; i++) {
                if (currentlySelectedStep == i) {
                    row2 += 'Y';
                } else if (sequencer.currentStep == i) {
                    row2 += 'X';
                } else {
                    row2 += 'O';
                }
            }
            
            lcdPadPrint(row1, 0);
            lcdPadPrint(row2, 1);
        };

    private:
        int encoderLastPosition;
};