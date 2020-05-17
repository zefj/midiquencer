#ifndef UI_H
#define UI_H

#include "UIState.h"
#include "SequenceView.h"
#include "PickStepView.h"
#include "StepOptionsView.h"

#define FPS 60
#define PRINT_FREQUENCY 1 / FPS * 1000

extern UIState uiState;

class UI
{
    public:
        void begin() {
            IView &view = getCurrentView();
            view.enable();
            previousView = &view;
        }
   
        void loop() {
            uiState.loop();
            
            IView &view = getCurrentView();

            if (&view != previousView) {
                view.enable();
            }

            view.loop();
            print();

            previousView = &view;
        }

        void print() {
            if ((millis() - lastPrintMillis) <= PRINT_FREQUENCY) {
                return;
            }
            
            lastPrintMillis = millis();

            IView &view = getCurrentView();

            display.clearDisplay();

            view.print();

            display.display();
        }
    private:
        long lastPrintMillis = 0;
        IView *previousView;

        SequenceView view_Sequence;
        PickStepView view_PickStep;
        StepOptionsView view_StepOptions;

        IView& getCurrentView() {
            switch (uiState.state) {
                case UI_STATE_SEQUENCE:
                    return view_Sequence;
                    break;
                case UI_STATE_PICK_STEP:
                    return view_PickStep;
                    break;
                case UI_STATE_STEP_OPTIONS:
                    return view_StepOptions;
                    break;
            }
        }
};

#endif
