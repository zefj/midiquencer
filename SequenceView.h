#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"

extern Controls controls;
extern UIState uiState;
extern Sequencer sequencer;

extern Adafruit_SSD1306 display;

const byte STEP_ICON_SIZE = (display.width() / SEQUENCER_STEPS_PER_PAGE) - 2;

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

            if (controls.buttonAccept.wasPressed()) {
                if (sequencer.isPlaying) {
                    sequencer.stop();
                } else {
                    sequencer.play();
                }
            }
        };

        void print() override {
            byte page = sequencer.page;
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
            }
        };

    private:
        int encoderLastPosition;

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
};