#ifndef SN74HC595N_H
#define SN74HC595N_H

#define SN74HC595N_DELAY_AFTER_PIN_CHANGE_MICROSEC 1

class SN74HC595N
{
public:
    // number of parallel bits (has a serial output too)
    static const int OUTPUT_BITS;

private:
    const byte _pin_ser;   // serial input
    const byte _pin_srclk; // RISING edge: shift register clock
    const byte _pin_rclk;  // RISING edge: storage clock
    const byte _pin_srclr; // active LOW: clear shift register
    const byte _pin_oe;    // active LOW: output enable

public:
    SN74HC595N (const byte pin_ser, const byte pin_srclk, const byte pin_rclk, const byte pin_srclr, const byte pin_oe);

    void setup();

    void shift(const byte bit) const;

    // update output pins
    // please note that "serial output" pin is updated independantly of this
    void store() const;

    void clear() const;

    void enableOutput() const;
    void disableOutput() const;

private:
    inline static void SetPin(const byte pin, const byte state);
};

class SN74HC595N_TestLooper
{

private:
    const SN74HC595N& sipo;
    unsigned long test_interval_ms;

    unsigned long last_test;
    unsigned long current_index;

public:
    SN74HC595N_TestLooper(const SN74HC595N& _sipo, unsigned long test_interval_ms = 1000);
    void interval_loop();
};

#endif
