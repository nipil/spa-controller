#ifndef PANEL_H
#define PANEL_H

/***************************************************************************/

#define COMMS_PANEL_FRAME_TIMEOUT_USEC 13000
#define COMMS_PANEL_SPA_BITS 70
#define COMMS_PANEL_DSP_BITS 6
#define COMMS_PANEL_NUM_CLOCK_EVENTS (COMMS_PANEL_SPA_BITS+COMMS_PANEL_DSP_BITS)

/***************************************************************************/

class CommsBuffer
{
private:
    typedef byte BitBuffer[COMMS_PANEL_NUM_CLOCK_EVENTS];
    BitBuffer bit_frame;

public:
    CommsBuffer();
    bool operator !=(const CommsBuffer &b) const;

    void reset();
    void print() const;

    byte getBit(byte bit_index) const;
    void setBit(byte bit_index, byte bit_value);
};

/***************************************************************************/

class CommsPanel {

private:
    byte in_pin_clk, in_pin_spa, in_pin_dsp;

    ulong frame_start_usec;
    byte clock_count;

    CommsBuffer current_frame;

public:
    CommsBuffer incoming_frame;

    CommsPanel(byte _in_pin_clk, byte _in_pin_spa, byte _in_pin_dsp);

    void reset_frame();
    void setup( void (*clk_int)() );

    void IRAM_ATTR input_clock_pin_changed();
    void clock_rising();

    void save_full_frame();
    void interval_loop();
};

#endif
