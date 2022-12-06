#!/usr/bin/env python3

import sys
import random
import string

len_a = 12
len_elements = [40, 40, 40, 40, 40, 40, 40, 576, 576, 576, 576, 1500]

def randStr(N, chars = string.hexdigits):
    return ''.join(random.choice(chars) for _ in range(N))

file_name = sys.argv[1]

with open(file_name, 'w') as f:
    f.write("int len_a = ")
    f.write(str(len_a))
    f.write(";\n")

    f.write("int len_elements[] = {")
    for i in range(len_a):
        f.write(str(len_elements[i]))
        f.write(", ")
    f.write("};\n")

    f.write("char *a[] = {")
    for i in range(len_a):
        f.write("\"")
        f.write(randStr(len_elements[i]))
        f.write("\", ")
    f.write("};\n")
