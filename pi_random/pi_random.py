#!/usr/bin/env python3

from random import random


def calculate_pi(n):
    count = 0
    for _ in range(n):
        x = random() - 0.5
        y = random() - 0.5
        if x*x + y*y < 0.25:
            count += 1
    apprx_pi = 4*count / n
    return apprx_pi



print(calculate_pi(10000000))


