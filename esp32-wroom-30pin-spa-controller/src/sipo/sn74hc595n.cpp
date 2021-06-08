#include <Arduino.h>

#include "sipo/sn74hc595n.h"

// number of parallel bits (has a serial output too)
const int SN74HC595N::OUTPUT_BITS = 8;

// inline function
void SN74HC595N::SetPin(const byte pin, const byte state)
{
    digitalWrite(pin, state ? HIGH : LOW);
    delayMicroseconds(SN74HC595N_DELAY_AFTER_PIN_CHANGE_MICROSEC);
}

SN74HC595N::SN74HC595N(const byte pin_ser, const byte pin_srclk, const byte pin_rclk, const byte pin_srclr, const byte pin_oe)
: _pin_ser(pin_ser), _pin_srclk(pin_srclk), _pin_rclk(pin_rclk), _pin_srclr(pin_srclr), _pin_oe(pin_oe)
{
}

void SN74HC595N::setup()
{
    pinMode(_pin_ser, OUTPUT);
    SetPin(_pin_ser, LOW);

    pinMode(_pin_srclk, OUTPUT);
    SetPin(_pin_srclk, LOW);

    pinMode(_pin_rclk, OUTPUT);
    SetPin(_pin_rclk, LOW);

    // SRCLR pin is active low
    // default state : do not clear
    pinMode(_pin_srclr, OUTPUT);
    SetPin(_pin_srclr, HIGH);

    // OE pin is active low
    // default state: output disabled
    pinMode(_pin_oe, OUTPUT);
    SetPin(_pin_oe, HIGH);
}

void SN74HC595N::shift(const byte bit) const
{
    // Copy data to SER pin
    SetPin(_pin_ser, bit ? HIGH : LOW);

    // SRCLK pin triggers on rising edge
    SetPin(_pin_srclk, LOW);
    SetPin(_pin_srclk, HIGH);
}

void SN74HC595N::store() const
{
    // RCLK pin triggers on rising edge
    SetPin(_pin_rclk, LOW);
    SetPin(_pin_rclk, HIGH);
}

void SN74HC595N::clear() const
{
    // SRCLR pin is active low
    SetPin(_pin_srclr, LOW);
    SetPin(_pin_srclr, HIGH);
}

void SN74HC595N::enableOutput() const
{
    // OE pin is active low
    SetPin(_pin_oe, LOW);
}

void SN74HC595N::disableOutput() const
{
    // OE pin is active low
    SetPin(_pin_oe, HIGH);
}


SN74HC595N_TestLooper::SN74HC595N_TestLooper(const SN74HC595N& _sipo, unsigned long _test_interval_ms)
: sipo(_sipo), test_interval_ms(_test_interval_ms), last_test(0), current_index(0)
{
}

void SN74HC595N_TestLooper::interval_loop()
{
    // if not ready yet
    int current = millis();
    if (current - last_test < test_interval_ms)
        return;

    // Log current state
    last_test = current;
    Serial.print("SN74HC595N_TestLooper: interval_loop index=");
    Serial.println(current_index);

    // Test choice: enable output once and for all
    sipo.enableOutput();

    // Test choice: clear before setting bits
    sipo.clear();

    // Test choice: push one more bit to test output serial value too
    const int count = sipo.OUTPUT_BITS + 1;

    // Test choice: only one bit is different than the others, and chosen bit is low
    byte bit, bits[count];
    for (int i = 0; i < count; i++)
    {
        if (i == current_index)
            bit = 0;
        else
            bit = 1;
        bits[i] = bit;
        sipo.shift(bit);
    }

    // update OUTPUT_BITS (please note that "serial output" pin is updated independantly of this)
    sipo.store();

    // only log after fast operation is finished, so that serial line is not slowing the process
    Serial.print("SN74HC595N_TestLooper: pushed bits from left to right, ");
    for (int i = 0; i < count; i++)
    {
       Serial.print(bits[i]);
       Serial.print(" ");
    }
    Serial.println();

    // prepare for next iteration
    current_index = (current_index + 1) % count;
}
