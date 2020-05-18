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
            lastEncoderPosition = controls.encoderPosition;
        };

        void loop() override {
            if (controls.encoderPosition != lastEncoderPosition) {
                uiState.changeToPickStep();
            }

            lastEncoderPosition = controls.encoderPosition;

            if (controls.buttonAccept.wasPressed()) {
                releasedLongPress = false;
            }

            if (!releasedLongPress && controls.buttonAccept.wasReleased()) {
                if (sequencer.isPlaying) {
                    sequencer.stop();
                } else {
                    sequencer.play();
                }
            } else if (controls.buttonAccept.pressedFor(BUTTON_LONG_PRESS)) {
                sequencer.reset();
                // Ignore the next button release event if it's a long press release.
                // This is to ensure the action is not triggered after a long press.
                releasedLongPress = true;
            }

            if (controls.buttonCancel.wasReleased()) {
                uiState.changeToSequencerOptions();
            }
        };

        void print() override {
            renderSequencer(sequencer.page);
            renderHeader("SEQUENCER");
        };

    private:
        bool releasedLongPress;
        int16_t lastEncoderPosition = 0;
};