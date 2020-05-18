#ifndef UI_H
#define UI_H

#include "UIState.h"
#include "SequenceView.h"
#include "PickStepView.h"
#include "StepOptionsView.h"
#include "SequencerOptionsView.h"

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

            // Helper pixels to locale screen edges
            #if DEBUG
            display.drawPixel(0, 0, SSD1306_WHITE);
            display.drawPixel(0, display.height() - 1, SSD1306_WHITE);
            display.drawPixel(display.width() - 1, 0, SSD1306_WHITE);
            display.drawPixel(display.width() - 1, display.height() - 1, SSD1306_WHITE);
            #endif

            display.display();
        }
    private:
        long lastPrintMillis = 0;
        IView *previousView;

        SequenceView view_Sequence;
        PickStepView view_PickStep;
        StepOptionsView view_StepOptions;
        SequencerOptionsView view_SequencerOptions;

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
                    break;
                case UI_STATE_SEQUENCER_OPTIONS:
                    return view_SequencerOptions;
                    break;
            }
        }
};

#endif
