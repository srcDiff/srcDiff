#!/usr/bin/python

import itertools

def power_set(alist) :

    sequence = []

    for i in range(1, len(alist)) :
        sequence.append(itertools.combinations(alist, i))

    return sequence

def powerset(iterable):
    "powerset([1,2,3]) --> () (1,) (2,) (3,) (1,2) (1,3) (2,3) (1,2,3)"
    s = list(iterable)
    return chain.from_iterable(combinations(s, r) for r in range(len(s)+1))

print power_set([1,2,3])
