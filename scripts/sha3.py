#!/usr/bin/env python2.7

from __future__ import print_function
import sha3, sys

BUF_SIZE = 65536

def eprint(*args, **kwargs):
  print(*args, file=sys.stderr, **kwargs)

if len(sys.argv) != 2:
  eprint("usage: sha3_512.py (filename)")
  exit(1)

H = sha3.sha3_512()

filename = sys.argv[1]
with open(filename) as f:
  while True:
    data = f.read(BUF_SIZE)
    if not data:
      break
    H.update(data)

print(H.hexdigest())
