import itertools

def power_set(iterable):

    return chain.from_iterable(combinations(s, r) for r in range(len(s)+1))
