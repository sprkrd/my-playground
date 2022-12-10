#!/usr/bin/env python3

import sys

text = sys.stdin.read().strip()


b = text.encode("utf8")
x = b.decode("u16")
program = f"exec(bytes(\"{x}\",\"u16\")[2:])"

print(len(text),"->",len(program))

print(program)

