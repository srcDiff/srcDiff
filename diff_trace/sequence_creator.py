#!/usr/bin/python

import itertools

def split_xpath(xpath) :

    return xpath.split("/")

def sequence_create(sequence, tag) :

    name = get_name(tag)
    predicates = get_predicates(tag)

    sets = power_set(predicates)

    sequence_list = sequence + name + "\n"

    for items in sets :

        sequence += sequence + name

        for item in items :

            sequence += item

        sequence += "\n"

    return sequence_list

def get_predicates(tag) :

    predicates = []

    for tag.find("[") != -1 :
        
        predicate = [ tag.find("[") : tag.find("]") + 1 ]

        tag = [ : tag.find("]") + 1 ]

    return predicates

def get_name(tag) :

    offset = tag.find("[")
    
    if offset == -1 :
        return tag
    else :
        return tag[: offset]

def power_set(alist):

    return list(itertools.chain.from_iterable(itertools.combinations(alist, i) for i in range(1, len(alist) +1)))

xpath = "/src:unit['a']/src:function[1]['g'][f']"

print create_sequence(1, xpath)
