#include <Arduino.h>

#include "comms/panel.h"

CommsPanel::CommsPanel(byte _in_pin_clk, byte _in_pin_spa, byte _in_pin_dsp)
: in_pin_clk(_in_pin_clk), in_pin_spa(_in_pin_spa), in_pin_dsp(_in_pin_dsp)
{
    reset_frame();
    memset(incoming_frame, 0, sizeof(incoming_frame)*sizeof(incoming_frame[0]));
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
    memset(current_frame, 0, sizeof(current_frame)*sizeof(current_frame[0]));
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
        current_frame[clock_count] = digitalRead(in_pin_spa);
    }
    else
    {
        current_frame[clock_count] = digitalRead(in_pin_dsp);
    }

    // count bits
    clock_count++;
}

void CommsPanel::save_full_frame()
{
    memcpy(incoming_frame, current_frame, sizeof(incoming_frame)*sizeof(incoming_frame[0]));
}

void CommsPanel::print_incoming_frame()
{
    for (int i=0; i<sizeof(incoming_frame); i++)
    {
        Serial.printf("%i", incoming_frame[i]);
    }
}

void CommsPanel::interval_loop()
{
    static ulong last_display = 0;
    ulong current_time = millis();
    if (current_time - last_display > 1000)
    {
        last_display = current_time;
        Serial.printf("%010lu:", current_time);
        print_incoming_frame();
        Serial.println("");
    }
}
