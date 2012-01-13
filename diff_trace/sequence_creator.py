#!/usr/bin/python

import itertools

def power_set(alist):

    return chain.from_iterable(itertools.combinations(alist, i) for i in range(len(alist) +1))

print power_set([1,2,3])
