#include <Arduino.h>

#include "comms/panel.h"

/***************************************************************************/

const byte PanelState::BITS_BUTTON[] = { 72, 73, 74, 75 };

PanelState::PanelState(PanelBits& _ref_panel_bits)
: ref_panel_bits(_ref_panel_bits)
{

}

void PanelState::set_button(ButtonBits button)
{
    // check bits
    switch (button)
    {
        case BUTTON_NONE:
        case BUTTON_PROG:
        case BUTTON_JET1:
        case BUTTON_JETS23:
        case BUTTON_LIGHTS:
        case BUTTON_CLOCK:
        case BUTTON_BUBBLES:
        case BUTTON_TEMP_UP:
        case BUTTON_TEMP_DOWN:
            break;

        default:
            Serial.print("PanelState::set_button: unknown button value ");
            Serial.println(button);
            throw button;
    }

    // build bits from integer
    for (int i=0; i < sizeof(BITS_BUTTON); i++)
    {
        byte bit = (button >> (sizeof(BITS_BUTTON) - 1 - i)) & 1;
        ref_panel_bits.setBit(BITS_BUTTON[i], bit);
    }
}

PanelState::ButtonBits PanelState::get_button() const
{
    // build integer value from bits
    byte value = 0;
    for (int i=0; i < sizeof(BITS_BUTTON); i++)
    {
        value = value + (ref_panel_bits.getBit(BITS_BUTTON[i]) << (sizeof(BITS_BUTTON) - 1 - i));
    }
    // check bits
    switch (value)
    {
        case BUTTON_NONE:
        case BUTTON_PROG:
        case BUTTON_JET1:
        case BUTTON_JETS23:
        case BUTTON_LIGHTS:
        case BUTTON_CLOCK:
        case BUTTON_BUBBLES:
        case BUTTON_TEMP_UP:
        case BUTTON_TEMP_DOWN:
            return (ButtonBits) value;

        default:
            Serial.print("PanelState::get_button: unknown button value ");
            Serial.println(value);
            return BUTTON_NONE;
    }
}

/***************************************************************************/

PanelBits::PanelBits()
{
    reset();
}

bool PanelBits::operator !=(const PanelBits &b) const
{
    return memcmp(bit_frame, b.bit_frame, sizeof(BitBuffer));
}

void PanelBits::reset()
{
    memset(bit_frame, 0, sizeof(bit_frame));
}

void PanelBits::print() const
{
    ulong current_time = millis();
    Serial.print("PanelBits: ");
    for (int i=0; i<sizeof(bit_frame); i++)
    {
        Serial.printf("%i", bit_frame[i]);
    }
    Serial.printf(" (%lu)", current_time);
    Serial.println();
}

inline byte PanelBits::getBit(byte bit_index) const
{
    if (bit_index >= PANEL_PROTOCOL_TOTAL_BITS)
        throw bit_index;
    return bit_frame[bit_index];
}

inline void PanelBits::setBit(byte bit_index, byte bit_value)
{
    if (bit_index >= PANEL_PROTOCOL_TOTAL_BITS)
        throw bit_index;
    bit_frame[bit_index] = bit_value;
}

/***************************************************************************/

PanelProtocol::PanelProtocol(byte _in_pin_clk, byte _in_pin_spa, byte _in_pin_dsp)
: in_pin_clk(_in_pin_clk), in_pin_spa(_in_pin_spa), in_pin_dsp(_in_pin_dsp)
{
    reset_frame();
}

void PanelProtocol::setup( void (*clk_int)() )
{
    pinMode(in_pin_clk, INPUT_PULLDOWN);
    pinMode(in_pin_spa, INPUT_PULLDOWN);
    pinMode(in_pin_dsp, INPUT_PULLUP);

    attachInterrupt(in_pin_clk, clk_int, CHANGE);
}

void PanelProtocol::reset_frame()
{
    frame_start_usec = micros();
    clock_count = 0;
    current_frame.reset();
}

void IRAM_ATTR PanelProtocol::input_clock_pin_changed()
{
    int in_clock = digitalRead(in_pin_clk);
    if (in_clock)
    {
        clock_rising();
    }
}

void PanelProtocol::clock_rising() {
    ulong current_usec = micros();

    // check for frame timeout
    if (current_usec - frame_start_usec > PANEL_PROTOCOL_FRAME_TIMEOUT_USEC)
    {
        // only process full frames
        if (clock_count == PANEL_PROTOCOL_TOTAL_BITS)
            save_full_frame();
        // full frame or not, reset/discard temp buffer
        reset_frame();
    }

    // put bits in cache
    if (clock_count < PANEL_PROTOCOL_SPA_BITS)
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

void PanelProtocol::save_full_frame()
{
    bool print = incoming_frame != current_frame;
    incoming_frame = current_frame;
    if (print)
    {
        current_frame.print();
    }
}

void PanelProtocol::interval_loop()
{
}
