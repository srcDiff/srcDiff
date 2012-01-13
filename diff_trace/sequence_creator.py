#!/usr/bin/python

import itertools

def split_xpath(xpath) :

    
    tags = xpath.split("/") [ 1 :]

    return tags

def sequence_tag(sequence, tag) :

    name = get_name(tag)
    predicates = get_predicates(tag)

    sets = power_set(predicates)

    sequence_list = str(sequence) + "\t" + name + "\n"

    for items in sets :

        sequence_list += str(sequence) + "\t" + name

        for item in items :
            
            sequence_list += item

        sequence_list += "\n"

    return sequence_list

def get_predicates(tag) :

    predicates = []

    while not(tag.find("[") == -1) :

        predicates.append(tag[ tag.find("[") : tag.find("]") + 1 ])

        tag = tag[ tag.find("]") + 1 :]

    return predicates

def get_name(tag) :

    offset = tag.find("[")
    
    if offset is -1 :
        return tag
    else :
        return tag[: offset]

def power_set(alist):

    return list(itertools.chain.from_iterable(itertools.combinations(alist, i) for i in range(1, len(alist) +1)))

def sequence_xpath(sequence, xpath) :

    sequence_list = ""

    first = 0

    for tag in split_xpath(xpath) :

        sequence_list +=  sequence_tag(sequence, tag)

    return sequence_list

sequence = 1
xpath = "/src:unit['a']/src:function['h']['g']['f']"

print sequence_xpath(sequence, xpath)
