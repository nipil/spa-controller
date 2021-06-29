#include <Arduino.h>

#include "comms/panel.h"

/***************************************************************************/

CommsBuffer::CommsBuffer()
{
    reset();
}

bool CommsBuffer::operator !=(const CommsBuffer &b) const
{
    return memcmp(bit_frame, b.bit_frame, sizeof(BitBuffer));
}

void CommsBuffer::reset()
{
    memset(bit_frame, 0, sizeof(bit_frame));
}

void CommsBuffer::print() const
{
    ulong current_time = millis();
    Serial.print("CommsBuffer: ");
    for (int i=0; i<sizeof(bit_frame); i++)
    {
        Serial.printf("%i", bit_frame[i]);
    }
    Serial.printf(" (%lu)", current_time);
    Serial.println();
}

inline byte CommsBuffer::getBit(byte bit_index) const
{
    if (bit_index >= COMMS_PANEL_NUM_CLOCK_EVENTS)
        throw bit_index;
    return bit_frame[bit_index];
}

inline void CommsBuffer::setBit(byte bit_index, byte bit_value)
{
    if (bit_index >= COMMS_PANEL_NUM_CLOCK_EVENTS)
        throw bit_index;
    bit_frame[bit_index] = bit_value;
}

/***************************************************************************/

CommsPanel::CommsPanel(byte _in_pin_clk, byte _in_pin_spa, byte _in_pin_dsp)
: in_pin_clk(_in_pin_clk), in_pin_spa(_in_pin_spa), in_pin_dsp(_in_pin_dsp)
{
    reset_frame();
}

void CommsPanel::setup( void (*clk_int)() )
{
    pinMode(in_pin_clk, INPUT_PULLDOWN);
    pinMode(in_pin_spa, INPUT_PULLDOWN);
    pinMode(in_pin_dsp, INPUT_PULLUP);

    attachInterrupt(in_pin_clk, clk_int, CHANGE);
}

void CommsPanel::reset_frame()
{
    frame_start_usec = micros();
    clock_count = 0;
    current_frame.reset();
}

void IRAM_ATTR CommsPanel::input_clock_pin_changed()
{
    int in_clock = digitalRead(in_pin_clk);
    if (in_clock)
    {
        clock_rising();
    }
}

void CommsPanel::clock_rising() {
    ulong current_usec = micros();

    // check for frame timeout
    if (current_usec - frame_start_usec > COMMS_PANEL_FRAME_TIMEOUT_USEC)
    {
        // only process full frames
        if (clock_count == COMMS_PANEL_NUM_CLOCK_EVENTS)
            save_full_frame();
        // full frame or not, reset/discard temp buffer
        reset_frame();
    }

    // put bits in cache
    if (clock_count < COMMS_PANEL_SPA_BITS)
    {
        current_frame.setBit(clock_count, digitalRead(in_pin_spa));
    }
    else
    {
        current_frame.setBit(clock_count, digitalRead(in_pin_dsp));
    }

    // count bits
    clock_count++;
}

void CommsPanel::save_full_frame()
{
    bool print = incoming_frame != current_frame;
    incoming_frame = current_frame;
    if (print)
        current_frame.print();
}

void CommsPanel::interval_loop()
{
}
