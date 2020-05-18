#ifndef UISTATE_H
#define UISTATE_H

#define UI_STATE_SEQUENCE 0
#define UI_STATE_PICK_STEP 1
#define UI_STATE_STEP_OPTIONS 2
#define UI_STATE_SEQUENCER_OPTIONS 4

// TODO: move this away from here
#define STEP_OPTIONS_CHOOSE_OPTION 0
#define STEP_OPTIONS_ENABLE 1
#define STEP_OPTIONS_NOTE 2
#define STEP_OPTIONS_VELOCITY 3

#define SEQUENCER_OPTIONS_CHOOSE_OPTION 0
#define SEQUENCER_OPTIONS_BANK 1
#define SEQUENCER_OPTIONS_CHANNEL 2
#define SEQUENCER_OPTIONS_BPM 3
#define SEQUENCER_OPTIONS_STEPS 4

class UIState {
    public:
        byte state = UI_STATE_SEQUENCE;
        uint8_t currentlySelectedStep = 0;
        // 0-based
        uint8_t bank = 0;

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

        void changeToSequencerOptions() {
                Serial.println("Changing to sequencer options");
            queueChangeState(UI_STATE_SEQUENCER_OPTIONS);
        }
    private:
        byte nextState;

        void queueChangeState(byte state) {
            nextState = state;
        }
};

#endif