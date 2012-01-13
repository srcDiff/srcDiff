#!/usr/bin/python

import itertools

def powerset(alist):

    return chain.from_iterable(itertools.combinations(alist, i) for i in range(len(alist) +1))

print power_set([1,2,3])
