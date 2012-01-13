#!/usr/bin/python

import itertools

def powerset(iterable):
    "powerset([1,2,3]) --> () (1,) (2,) (3,) (1,2) (1,3) (2,3) (1,2,3)"
    s = list(iterable)
    return chain.from_iterable(itertools.combinations(s, r) for r in range(len(s)+1))

print power_set([1,2,3])
