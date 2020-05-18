#include "IView.h"

#include "Controls.h"
#include "Sequencer.h"
#include "Renderer.h"

#define MIN_VELOCITY 0
#define MAX_VELOCITY 127

extern Controls controls;
extern UIState uiState;

class SequencerOptionsView : public IView {
    public:
        void enable() override {
            option = SEQUENCER_OPTIONS_CHOOSE_OPTION;
            // Set to enable so that the option is reset after returning to this view
            highlightedOption = SEQUENCER_OPTIONS_BANK;

            lastEncoderPosition = controls.encoderPosition;
        };

        void loop() override {
            switch (option) {
                case SEQUENCER_OPTIONS_CHOOSE_OPTION:
                    if (controls.buttonCancel.wasReleased()) {
                        uiState.changeToSequence();
                    }

                    if (controls.buttonAccept.wasReleased()) {
                        option = highlightedOption;
                    }
                    break;
                case SEQUENCER_OPTIONS_BANK:
                case SEQUENCER_OPTIONS_CHANNEL:
                case SEQUENCER_OPTIONS_BPM:
                case SEQUENCER_OPTIONS_STEPS:
                    if (controls.buttonAccept.wasReleased()) {
                        option = SEQUENCER_OPTIONS_CHOOSE_OPTION;
                    }

                    if (controls.buttonCancel.wasReleased()) {
                        option = SEQUENCER_OPTIONS_CHOOSE_OPTION;
                    }
                    break;
            }

            handleEncoderRotation();
        };

        void handleEncoderRotation() {
            switch (option) {
                case SEQUENCER_OPTIONS_CHOOSE_OPTION:
                {
                    int16_t newHighlightedOption = controls.linearRotationHelper(highlightedOption, SEQUENCER_OPTIONS_BANK, SEQUENCER_OPTIONS_STEPS);
                    highlightedOption = (uint8_t)newHighlightedOption;
                    break;
                }
                case SEQUENCER_OPTIONS_BANK:
                {
                    // banks are 0-based
                    int16_t newBank = controls.linearRotationHelper(uiState.bank, 0, BANKS_AMOUNT - 1);
                    uiState.bank = (uint8_t)newBank;
                    break;
                }
                case SEQUENCER_OPTIONS_BPM:
                {
                    int16_t newBpm = controls.linearRotationHelper(sequencer.bpm, MIN_BPM, MAX_BPM);
                    sequencer.setBPM((uint8_t)newBpm);
                    break;
                }
                case SEQUENCER_OPTIONS_CHANNEL:
                {   
                    uint8_t* channel = &sequencer.bankChannels[uiState.bank];
                    *channel = (uint8_t)controls.linearRotationHelper(*channel, 1, 16);
                    break;
                }
                case SEQUENCER_OPTIONS_STEPS:
                {
                    // Just a simple increment/decrement since there aren't too many options, 
                    // so we don't have to account for missed steps
                    uint8_t newStepsAmount = sequencer.stepsAmount;

                    if (lastEncoderPosition < controls.encoderPosition) {
                        newStepsAmount = sequencer.stepsAmount * 2;                    
                    } else if (lastEncoderPosition > controls.encoderPosition) {
                        newStepsAmount = sequencer.stepsAmount / 2;
                    }

                    if (newStepsAmount > MAX_STEPS) {
                        newStepsAmount = MAX_STEPS;
                    }

                    if (newStepsAmount < MIN_STEPS) {
                        newStepsAmount = MIN_STEPS;
                    }
        
                    sequencer.setSteps(newStepsAmount);
                    lastEncoderPosition = controls.encoderPosition;
                    break;
                }
            }
        }

        void print() override {
            // renderStepOptions(options, option, highlightedOption);
            renderSequencerOptions(
                option,
                highlightedOption
            );

            Serial.print("HL: ");
            Serial.print(highlightedOption);
            Serial.print(" OPT: ");
            Serial.print(option);
            Serial.print(" BANK: ");
            Serial.print(uiState.bank);
            Serial.print(" CHANNEL: ");
            Serial.print(sequencer.bankChannels[uiState.bank]);
            Serial.print(" STEPS: ");
            Serial.print(sequencer.stepsAmount);
            Serial.print(" BPM: ");
            Serial.print(sequencer.bpm);
            Serial.println();
        };

    private:
        int16_t lastEncoderPosition = 0;
        byte option = SEQUENCER_OPTIONS_CHOOSE_OPTION;
        byte highlightedOption = SEQUENCER_OPTIONS_BANK;
};