//_VARIANT_ARDUINO_STM32_
#define DEBUG 1
// #include <LiquidCrystal.h>

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 1, d7 = 0;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET  -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
    #if DEBUG
    Serial.begin(9600);
    while (!Serial);
    #endif

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(250); // Pause for 2 seconds

    // Clear the buffer
    display.clearDisplay();

    sequencer.begin();
    controls.begin();
    ui.begin();

    registerTimers();
}

void loop()
{
    controls.loop();
    ui.loop();
}

void registerTimers()
{
    HardwareTimer* timer = new HardwareTimer(TIM2);
    // Pause the timer while we're configuring it
    timer->pause();

    // Set up period
    // Calls ISR every millisecond
    timer->setOverflow(1000, MICROSEC_FORMAT); 

    // Set up an interrupt on channel 1
    timer->setMode(1, TIMER_OUTPUT_COMPARE);
    // timer->setMode(2, TIMER_OUTPUT_COMPARE);
    timer->attachInterrupt(1, sequencerISR);
    // timer->attachInterrupt(2, timerISR2);
    
    // Refresh the timer's count, prescale, and overflow
    timer->refresh();

    // Start the timer counting
    timer->resume();
}

void sequencerISR()
{
    if (!sequencer.isPlaying) {
        return;
    }

    if (!sequencer.shouldBeat()) {
        return;
    }

    sequencer.handleBeat();
}