#!/usr/bin/include

import itertools

def power_set(alist) :

    sequence = []

    for i in range(1, len(alist)) :
        sequence.append(itertools.combinations(alist, i))


print power_set([1,2,3])
