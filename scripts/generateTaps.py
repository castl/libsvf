#!/usr/bin/python

# This file script was used to create the array of "magic"
# maximal taps for the LFSR in matrix_correlate.hpp. It is
# not used during compilation or runtime, only kept for 
# future development purposes, if necessary

import sys

tf = file("taps.txt", "r")

for i in range(3, 65):
    bits = int(tf.readline().strip())
    assert bits == i
    taps = map(lambda x: int(x), tf.readline().strip().split(","))
    magic = 0
    for t in taps:
        magic += 2**(t-1)
    #print "%s: 0x%xul, %s" % (bits, magic, taps)
    print "    0x%016xul,"% magic
