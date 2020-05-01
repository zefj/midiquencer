#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 1, d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String notes[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

#include "Controls.h"
Controls controls;
#include "UIState.h"
UIState uiState;
#include "UI.h"
UI ui;
#include "Sequencer.h"
Sequencer sequencer;

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    lcd.begin(16, 2);

    sequencer.begin();
    controls.begin();
    ui.begin();
}

void loop()
{
    sequencer.loop();
    controls.loop();
    ui.loop();
}