#ifndef ROTARY_ENCODER
#define ROTARY_ENCODER

/**
 * Based on https://www.best-microcontroller-projects.com/rotary-encoder.html
 * 
 * Register CHANGE interrupts on both encoder pins, call read() in the ISR.
 * 
 * TODO:
 *  * speed (https://github.com/MajicDesigns/MD_REncoder/blob/master/src/MD_REncoder.cpp)
 *  * logarithmic readings based on rotation speed
 *  * maybe half-step support?
 */ 
class RotaryEncoder
{
    public:
        RotaryEncoder(uint8_t PIN_CLK, uint8_t PIN_DATA)
        : _PIN_CLK(PIN_CLK), _PIN_DATA(PIN_DATA) {};

        void read()
        {
            prevNextCode <<= 2; // shift to make room for current step
            if (digitalRead(_PIN_DATA)) prevNextCode |= 0x02;
            if (digitalRead(_PIN_CLK)) prevNextCode |= 0x01;
            prevNextCode &= 0x0f; // keep the last 4 bits as we only care for 2 steps at a time

            // If valid then store as 16 bit data.
            if  (validStateTable[prevNextCode] ) {
                store <<= 4;
                store |= prevNextCode; // push current state to store
          
                if ((store & 0xff) == 0x2b) {
                    counter--;
                    return;
                };

                if ((store & 0xff) == 0x17) {
                    counter++;
                    return;
                }
            }
        }

        uint16_t getPosition()
        {
            return counter;
        }
    private:
        uint8_t _PIN_CLK;
        uint8_t _PIN_DATA;

        const int8_t validStateTable[16] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};
        volatile uint16_t store = 0;
        volatile uint8_t prevNextCode = 0;
        volatile int8_t counter = 0;
};

#endif