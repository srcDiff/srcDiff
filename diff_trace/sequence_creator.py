import itertools

def power_set(alist) :

    sequence = []

    for i in range(1, len(alist)) :
        sequence.append(itertools.combinations(alist, i))
