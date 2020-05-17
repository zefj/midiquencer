#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"
#include "Renderer.h"

extern Controls controls;
extern UIState uiState;
extern Sequencer sequencer;

extern Adafruit_SSD1306 display;

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

            if (controls.buttonCancel.pressedFor(BUTTON_LONG_PRESS)) {
                sequencer.reset();
            }
        };

        void print() override {
            renderSequencer(sequencer.page);
            renderHeader("SEQUENCER");
        };

    private:
        int encoderLastPosition;
};