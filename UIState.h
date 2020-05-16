#ifndef UISTATE_H
#define UISTATE_H

#define UI_STATE_SEQUENCE 0
#define UI_STATE_PICK_STEP 1
#define UI_STATE_STEP_OPTIONS 2

class UIState {
    public:
        byte state = UI_STATE_SEQUENCE;
        int currentlySelectedStep = 0;

        void loop() {
            state = nextState;
        }

        void changeToSequence() {
                Serial.println("Changing to sequence");
            queueChangeState(UI_STATE_SEQUENCE);
        }

        void changeToPickStep() {
                Serial.println("Changing to pick step");
            queueChangeState(UI_STATE_PICK_STEP);
        }

        void changeToStepOptions() {
                Serial.println("Changing to step options");
            queueChangeState(UI_STATE_STEP_OPTIONS);
        }
    private:
        byte nextState;

        void queueChangeState(byte state) {
            nextState = state;
        }
};

#endif