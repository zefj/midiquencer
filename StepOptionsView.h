#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"

extern Controls controls;
extern UIState uiState;

class StepOptionsView : public IView {
    public:
        void enable() override {};

        void loop() override {
            if (controls.buttonCancel.wasPressed()) {
                uiState.changeToPickStep();
                return;
            }
        };

        void print() override {
            lcdPadPrint(" StepOptions", 1);
        };

    private:
        int encoderLastPosition;
};