#!/usr/bin/env python3

# awk '{ print $1,$2,$3,$4 }' result/rising_clock.txt | ./check_bit_variation.py

import sys

class App:

    def __init__(self):
        self.changes = None
        self.last = None
        self.length = None

    def detect(self, line):
        if len(line) != self.length:
            raise Exception("Line length differ !")
        for i in range(self.length):
            if line[i] != self.last[i]:
                self.changes[i] = 1

    def display(self):
        result = list(self.last)
        for i in range(self.length):
            if self.changes[i]:
                result[i] = 'X'
        print("".join(result))

    def run(self):
        for line in sys.stdin:
            line = line.strip()
            if self.last is None:
                self.last = line
                self.length = len(line)
                self.changes = [ 0 for i in range(self.length) ]
                continue
            self.detect(line)
            self.last = line
        self.display()

if __name__ == '__main__':
    app = App()
    app.run()
