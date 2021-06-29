#ifndef PANEL_H
#define PANEL_H

/***************************************************************************/

#define PANEL_PROTOCOL_FRAME_TIMEOUT_USEC 13000
#define PANEL_PROTOCOL_SPA_BITS 70
#define PANEL_PROTOCOL_DSP_BITS 6
#define PANEL_PROTOCOL_TOTAL_BITS (PANEL_PROTOCOL_SPA_BITS+PANEL_PROTOCOL_DSP_BITS)

/***************************************************************************/

class PanelBits
{
private:
    typedef byte BitBuffer[PANEL_PROTOCOL_TOTAL_BITS];
    BitBuffer bit_frame;

public:
    PanelBits();
    bool operator !=(const PanelBits &b) const;

    void reset();
    void print() const;

    byte getBit(byte bit_index) const;
    void setBit(byte bit_index, byte bit_value);
};

/***************************************************************************/

class PanelState
{
public:
    PanelBits& ref_panel_bits;

    PanelState(PanelBits& _ref_panel_bits);

private:
    static const byte BITS_BUTTON[]; // bits 72-75:

public:
    typedef enum {             // Stored in BITS_BUTTON bits
        BUTTON_NONE = 0,       // 0000 > IDLE
		BUTTON_PROG = 8,       // 1000 > PROG
		BUTTON_JET1 = 9,       // 1001 > JET1
		BUTTON_JETS23 = 10,    // 1010 > JETS23
		BUTTON_LIGHTS = 11,    // 1011 > LIGHT
		BUTTON_CLOCK = 12,     // 1100 > CLOCK
		BUTTON_BUBBLES = 13,   // 1101 > AIR
		BUTTON_TEMP_UP = 14,   // 1110 > HOTTER
		BUTTON_TEMP_DOWN = 15  // 1111 > COLDER
    } ButtonBits;

    void set_button(ButtonBits button);
    ButtonBits get_button() const;
};

/***************************************************************************/

class PanelProtocol {

private:
    byte in_pin_clk, in_pin_spa, in_pin_dsp;

    ulong frame_start_usec;
    byte clock_count;

    PanelBits current_frame;

public:
    PanelBits incoming_frame;

    PanelProtocol(byte _in_pin_clk, byte _in_pin_spa, byte _in_pin_dsp);

    void reset_frame();
    void setup( void (*clk_int)() );

    void IRAM_ATTR input_clock_pin_changed();
    void clock_rising();

    void save_full_frame();
    void interval_loop();
};

#endif
