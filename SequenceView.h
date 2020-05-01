#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"

extern Controls controls;
extern UIState uiState;
extern Sequencer sequencer;

class SequenceView : public IView {
    public:
        void enable() override {
            // Save current encoder value upon entering this state, because 
            // rotation in this step changes ui state.
            encoderLastPosition = controls.encoderPosition;
        };

        void loop() override {
            if (controls.encoderPosition != encoderLastPosition) {
                uiState.changeToPickStep();
            }

            encoderLastPosition = controls.encoderPosition;
        };

        void print() override {
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
        };

    private:
        int encoderLastPosition;
};