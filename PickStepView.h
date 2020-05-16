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
            byte limit = SEQUENCER_STEPS_PER_PAGE * page;
            byte start = limit - SEQUENCER_STEPS_PER_PAGE;

            byte stepOffset = SEQUENCER_STEPS_PER_PAGE * (page - 1);

            printPage(page);

            for (byte i = start; i < limit; i++) {
                int step = i - stepOffset;
                
                if (step < 0) {
                    step = i;
                }

                printStepRect(step, sequencer.currentStep == i);

                if (uiState.currentlySelectedStep == i) {
                    printStepUnderline(step);
                }
            }
        };

    private:
        // int encoderLastPosition;

        void printPage(byte page) {
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(
                90, // magic number :)
                display.height() - (STEP_ICON_SIZE * 2) - 4
            );
            
            display.print("PAGE ");
            display.print(page);
        }

        void printStepRect(byte step, bool fill) {
            if (fill) {
                display.fillCircle(
                    step * (STEP_ICON_SIZE + 2) + (STEP_ICON_SIZE / 2),
                    display.height() - STEP_ICON_SIZE,
                    STEP_ICON_SIZE / 2,
                    SSD1306_WHITE
                );
            } else {
                display.drawCircle(
                    step * (STEP_ICON_SIZE + 2) + (STEP_ICON_SIZE / 2),
                    display.height() - STEP_ICON_SIZE,
                    STEP_ICON_SIZE / 2,
                    SSD1306_WHITE
                );
            }
        }

        void printStepUnderline(byte step) {
            display.drawLine(
                step * (STEP_ICON_SIZE + 2),
                display.height() - 4,
                step * (STEP_ICON_SIZE + 2) + STEP_ICON_SIZE,
                display.height() - 4,
                SSD1306_WHITE
            );
        }
};