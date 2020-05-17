#ifndef RENDERER_H
#define RENDERER_H

#include <Adafruit_SSD1306.h>
#include "Sequencer.h"

extern Sequencer sequencer;
extern Adafruit_SSD1306 display;

const byte STEP_ICON_SIZE = (display.width() / SEQUENCER_STEPS_PER_PAGE) - 2;

void _setCursorTextMiddle(char* text, uint8_t box_x_1, uint8_t box_y_1, uint8_t box_w, uint8_t box_h)
{
    int16_t x1, y1;
    uint16_t w, h;

    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    uint16_t cursor_x = box_x_1 + (box_w / 2) - (w / 2);
    uint16_t cursor_y = box_y_1 + (box_h / 2) - (h / 2);

    display.setCursor(cursor_x, cursor_y);
}

void _renderPage(byte page) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(
        90, // magic number :)
        display.height() - (STEP_ICON_SIZE * 2) - 1
    );
    
    display.print("PAGE ");
    display.print(page);
}

void _renderBpm() {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(
        12,
        display.height() - (STEP_ICON_SIZE * 2) - 1
    );
    
    display.print(sequencer.bpm);
    display.print(" BPM");
}

void _renderPlayPause() {
    if (sequencer.isPlaying) {
        display.fillTriangle(
            0, display.height() - (STEP_ICON_SIZE * 2) - 2,
            4, display.height() - (STEP_ICON_SIZE * 2) + 2,
            0, display.height() - (STEP_ICON_SIZE * 2) + 6,
            SSD1306_WHITE
        );
    } else {
        display.fillRect(
            0, display.height() - (STEP_ICON_SIZE * 2) - 2,
            3, 8,
            SSD1306_WHITE
        );

        display.fillRect(
            5, display.height() - (STEP_ICON_SIZE * 2) - 2,
            3, 8,
            SSD1306_WHITE
        );
    }
}

void _renderStep(byte step, bool fill) {
    if (fill) {
        display.fillCircle(
            step * (STEP_ICON_SIZE + 2) + (STEP_ICON_SIZE / 2),
            display.height() - STEP_ICON_SIZE + 2,
            STEP_ICON_SIZE / 2,
            SSD1306_WHITE
        );
    } else {
        display.drawCircle(
            step * (STEP_ICON_SIZE + 2) + (STEP_ICON_SIZE / 2),
            display.height() - STEP_ICON_SIZE + 2,
            STEP_ICON_SIZE / 2,
            SSD1306_WHITE
        );
    }
}
        
void printStepUnderline(byte step) {
    display.drawFastHLine(
        step * (STEP_ICON_SIZE + 2),
        display.height() - 1,
        STEP_ICON_SIZE,
        SSD1306_WHITE
    );
}

void renderHeader(char* text)
{
    display.fillRect(
        0,
        0,
        display.width(),
        17,
        SSD1306_WHITE
    );

    display.setTextSize(2);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(1, 1);
    display.print(text);
}

void _getNoteRepresentation(uint8_t midiNote, char* string)
{
    int8_t octave = (midiNote / 12);
    uint8_t noteIndex = midiNote - (octave * 12);
    const char* note = NOTES[noteIndex];

    strcpy(string, note);
    char octaveString[2];
    // -1 so that the octaves are in range {-1, 9}
    itoa(octave - 1, octaveString, 10);
    strcat(string, octaveString);
}

/**
 * Constants for a 4-box grid renderer.
 */
const uint8_t boxVerticalSpacer = 2;
const uint8_t boxHorizontalSpacer = 10;
const uint8_t boxLabelSpacer = 2;
const uint8_t boxWidth = display.width() / 2;
const uint8_t boxHeight = display.height() / 2;
const uint8_t rectWidth = boxWidth - boxVerticalSpacer;
const uint8_t rectHeight = boxHeight - boxHorizontalSpacer;
const uint8_t rectCoords[4][2] = {
    {0, 0},
    {boxWidth + boxVerticalSpacer, 0},
    // the +1 is an additional magic spacer to align the box and label to bottom edge
    {0, boxHeight + 1},
    {boxWidth + boxVerticalSpacer, boxHeight + 1},
};

void renderStepOptions(
    StepOptions* options,
    byte selectedOption,
    byte highlightedOption
) {
    if (highlightedOption == STEP_OPTIONS_ENABLE) {
        display.fillRoundRect(
            rectCoords[0][0], 
            rectCoords[0][1],
            rectWidth,
            rectHeight,
            8,
            SSD1306_WHITE
        );

        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
        display.drawRoundRect(
            rectCoords[0][0], 
            rectCoords[0][1],
            rectWidth,
            rectHeight,
            8,
            SSD1306_WHITE
        );

        display.setTextColor(SSD1306_WHITE);
    }
    
    display.setTextSize(2);
    char enabledString[1];
    itoa(options->enabled, enabledString, 10);
    _setCursorTextMiddle(enabledString, rectCoords[0][0], rectCoords[0][1], rectWidth, rectHeight);
    display.print(options->enabled);

    if (highlightedOption == STEP_OPTIONS_NOTE) {
        display.fillRoundRect(
            rectCoords[1][0], 
            rectCoords[1][1],
            rectWidth,
            rectHeight,
            selectedOption == STEP_OPTIONS_NOTE ? 0 : 8,
            SSD1306_WHITE
        );
       
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
        display.drawRoundRect(
            rectCoords[1][0], 
            rectCoords[1][1],
            rectWidth,
            rectHeight,
            8,
            SSD1306_WHITE
        );
       
        display.setTextColor(SSD1306_WHITE);
    }

    display.setTextSize(2);

    char noteString[4];
    _getNoteRepresentation(options->note, noteString);
    _setCursorTextMiddle(noteString, rectCoords[1][0], rectCoords[1][1], rectWidth, rectHeight);
    display.print(noteString);

    if (highlightedOption == STEP_OPTIONS_VELOCITY) {
        display.fillRoundRect(
            rectCoords[2][0], 
            rectCoords[2][1],
            rectWidth,
            rectHeight,
            selectedOption == STEP_OPTIONS_VELOCITY ? 0 : 8,
            SSD1306_WHITE
        );

        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
        display.drawRoundRect(
            rectCoords[2][0], 
            rectCoords[2][1],
            rectWidth,
            rectHeight,
            8,
            SSD1306_WHITE
        );

        display.setTextColor(SSD1306_WHITE);
    }

    display.setTextSize(2);
    char velocityString[3];
    itoa(options->velocity, velocityString, 10);
    _setCursorTextMiddle(velocityString, rectCoords[2][0], rectCoords[2][1], rectWidth, rectHeight);
    display.print(options->velocity);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(rectCoords[0][0], rectCoords[0][1] + rectHeight + boxLabelSpacer);
    display.print("ENABLE");
    display.setCursor(rectCoords[1][0], rectCoords[1][1] + rectHeight + boxLabelSpacer);
    display.print("NOTE");
    display.setCursor(rectCoords[2][0], rectCoords[2][1] + rectHeight + boxLabelSpacer);
    display.print("VEL");
}

void renderSequencer(byte page)
{
    byte limit = SEQUENCER_STEPS_PER_PAGE * page;
    byte start = limit - SEQUENCER_STEPS_PER_PAGE;
    byte stepOffset = SEQUENCER_STEPS_PER_PAGE * (page - 1);

    _renderPage(page);
    _renderPlayPause();
    _renderBpm();

    for (byte i = start; i < limit; i++) {
        int step = i - stepOffset;
        
        if (step < 0) {
            step = i;
        }

        _renderStep(step, sequencer.currentStep == i);
    }
}

#endif