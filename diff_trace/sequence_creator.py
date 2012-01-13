#!/usr/bin/python

import itertools

def power_set(alist):

    return list(itertools.chain.from_iterable(itertools.combinations(alist, i) for i in range(1, len(alist) +1)))

print power_set([1,2,3])
