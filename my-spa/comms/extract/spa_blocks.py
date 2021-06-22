#!/usr/bin/env python3

# cat result/changing_bits.txt | ./spa_blocks.py

import sys

BIT_BLOCKS = [ 1, 7, 7, 7, 7, 2, 2, 2, 1, 8, 8, 8, 6, 2, 2, 6 ]

def blockize(bits):
    total = 0
    strings = []
    for l in BIT_BLOCKS:
        strings.append(bits[total:total+l])
        total = total + l
    return strings

def process(bits):
    strings = blockize(bits)
    print(BIT_BLOCKS)
    print(bits)
    print(" ".join(strings))
    print("\n".join(strings))

def main():
    for line in sys.stdin:
        toks = line.split(' ')
        print(toks[0].strip())
        process(toks[1].strip())
        print(toks[2].strip())
        process(toks[3].strip())

if __name__ == '__main__':
    main()
