#!/bin/python3

from Crypto.PublicKey import RSA
import sys

public_key = RSA.importKey(open(sys.argv[1], 'r').read())
file = open(sys.argv[2], "w")
file.write(format(public_key.p, 'x'))
file.write("\n")
file.write(format(public_key.q, 'x'))
file.write("\n")
file.write(format(public_key.n, 'x'))
file.write("\n")
file.write(format(public_key.e, 'x'))
file.write("\n")
file.write(format(public_key.d, 'x'))
file.write("\n")
file.close()

file = open(sys.argv[2] + ".pub", "w")
file.write(format(public_key.n, 'x'))
file.write("\n")
file.write(format(public_key.e, 'x'))
file.write("\n")
file.close()