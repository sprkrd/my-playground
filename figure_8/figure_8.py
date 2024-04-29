#!/usr/bin/env python3


def successors(state):
    # abc
    # def
    a,b,c,d,e,f = state
    return [
        (d,a,c,e,b,f),
        (b,e,c,a,d,f),
        (a,e,b,d,f,c),
        (a,c,f,d,b,e),
        (d,a,b,e,f,c),
        (b,c,f,a,d,e)
    ]


def state2str(state):
    a,b,c,d,e,f = state
    return f"{a}{b}{c}\n{d}{e}{f}"


def reachable(s, seen=None):
    if seen is None:
        seen = set()
    seen.add(s)
    for succ in successors(s):
        if succ not in seen:
            reachable(succ, seen)
    return seen


# r = reachable((1,2,3,4,5,6))
# print(len(r))
# for s in r:
    # print(state2str(s), end="\n\n")


print(state2str((1,2,3,4,5,6)), end="\n\n")

for s in successors((1,2,3,4,5,6)):
    print(state2str(s), end="\n\n")

