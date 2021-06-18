#!/usr/bin/env python3

# ./frame_changes.py ../capture/boot_buttons.vcd '(' '&' '$' 2>/dev/null

import re
import sys

class App:

    IDLE_TIME_US = 13000
    BIT_FRAME_LENGTH = 76

    def __init__(self, file_name, clk_char, spa_char, dsp_char, clock_edge):
        self.file_name = file_name

        if clock_edge:
            self.clock_edge_condition = self.rising_clock_edge
        else:
            self.clock_edge_condition = self.falling_clock_edge

        self.start_prev_value = None
        self.time_cur_value = None
        self.clk_char = clk_char
        self.clk_prev_state = None
        self.clk_cur_state = None
        self.spa_char = spa_char
        self.spa_prev_state = None
        self.spa_cur_state = None
        self.dsp_char = dsp_char
        self.dsp_prev_state = None
        self.dsp_cur_state = None

        self.reset()

        self.last_spa_bits = None
        self.last_dsp_bits = None

    def reset(self):
        # print("resetting bits")
        self.frame_started = False
        self.spa_bits = []
        self.dsp_bits = []

    def read(self, tokens):
        for tok in tokens:
            if tok[0] == '#':
                self.time_cur_value = int(tok[1:])
            if tok[1] == self.clk_char:
                self.clk_cur_state = int(tok[0])
            if tok[1] == self.spa_char:
                self.spa_cur_state = int(tok[0])
            if tok[1] == self.dsp_char:
                self.dsp_cur_state = int(tok[0])
        # print("CLK", self.clk_cur_state)
        # print("SPA", self.spa_cur_state)
        # print("DSP", self.dsp_cur_state)
        # print("TIME", self.time_cur_value)

    def store(self):
        self.clk_prev_state = self.clk_cur_state
        self.spa_prev_state = self.spa_cur_state
        self.dsp_prev_state = self.dsp_cur_state

    def falling_clock_edge(self):
        return self.clock_edge(0)

    def rising_clock_edge(self):
        return self.clock_edge(1)

    def clock_edge(self, current):
        return self.clk_prev_state != self.clk_cur_state and self.clk_cur_state == current

    def process(self):
        if not self.clock_edge_condition():
            return

        delta = self.time_cur_value - self.start_prev_value
        if delta > self.IDLE_TIME_US:
            self.start_prev_value = self.time_cur_value
            self.display_bits()
            self.reset()

        # print("clock edge")
        self.spa_bits.append(self.spa_cur_state)
        self.dsp_bits.append(self.dsp_cur_state)

    def display_bits(self):
        l = len(self.spa_bits)
        if l == 0:
            return
        elif l == self.BIT_FRAME_LENGTH:
            f = sys.stdout
        else:
            f = sys.stderr
        if self.last_spa_bits is None or self.last_spa_bits != self.spa_bits or self.last_dsp_bits is None or self.last_dsp_bits != self.dsp_bits:
            print("SPA", "".join([ str(bit) for bit in self.spa_bits]), "DSP", "".join([ str(bit) for bit in self.dsp_bits]), "USEC", self.start_prev_value, "LEN", l, file=f)

    def record(self, tokens):
        self.read(tokens)
        if self.time_cur_value == 0:
            self.start_prev_value = 0
        else:
            self.process()
        self.store()
        if len(self.spa_bits) == self.BIT_FRAME_LENGTH:
            self.display_bits()
            self.last_spa_bits = self.spa_bits
            self.last_dsp_bits = self.dsp_bits

    def run(self, max_samples = -1):
        with open(self.file_name) as f:
            count = max_samples
            for line in f:
                count = count - 1
                if count == 0:
                    break
                toks = line.strip().split(" ")
                if toks[0].startswith('#'):
                    self.record(toks)

if __name__ == '__main__':
    if len(sys.argv) == 5:
        max_samples = -1
    else:
        max_samples = int(sys.argv[5])
    app = App(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], 1)
    app.run(max_samples)

