#!/usr/bin/python

import itertools
import sys

def split_xpath(xpath) :

    tags = []
    while not(xpath == "") :

        xpath = xpath[ 1 : ]

        tag = ""
        if not(xpath.find("[") == -1) :

            tag = xpath[ : xpath.find("[") ]
            xpath = xpath[ xpath.find("[") : ]

        else :

            tag = xpath[ : xpath.find("/") ]
            xpath = xpath[ xpath.find("/") : ]

        while not(xpath == "") and not(xpath[0] == "/") :

            start = xpath.find("[")
            end = find_end_bracket(xpath, start)
            tag += xpath[start : end + 1]
            xpath = xpath[end + 1 : ]

        tags.append(tag)

    return tags

def sequence_tag(tag) :

    name = get_name(tag)
    predicates = get_predicates(tag)

    sequence_list = str(len(predicates) + 1) + "\t" + name

    for predicate in predicates :

        sequence_list += "\t" + name + predicate


    sequence_list += "\n"

    return sequence_list

def sequence_tag_old(sequence, tag) :

    name = get_name(tag)
    predicates = get_predicates(tag)

    sets = power_set(predicates)

    for items in sets :

        sequence_list += str(sequence) + " " + name

        for item in items :
            
            sequence_list += item

        sequence_list += "\n"

    return sequence_list

def clean_predicate(predicate) :

    predicate = predicate.replace("'", "")

    return predicate

def get_predicates(tag) :

    predicates = []

    while not(tag.find("[") == -1) :

        start = tag.find("[")

        end = find_end_bracket(tag, start)

        predicates.append(clean_predicate(tag[ start : end + 1 ]))

        tag = tag[ end + 1 :]

    return predicates

def find_end_bracket(tag, start) :

    bracket_count = 0
    for i in range(start, len(tag)) :

        if tag[i] == "[" :
            bracket_count += 1

        if tag[i] == "]" :
            bracket_count -= 1

        if bracket_count == 0 :
            return i

    return -1


def get_name(tag) :

    offset = tag.find("[")
    
    if offset is -1 :
        return tag
    else :
        return tag[: offset]

def power_set(alist):

    return list(itertools.chain.from_iterable(itertools.combinations(alist, i) for i in range(len(alist) + 1)))

def sequence_xpath(sequence, xpath) :

    sequence_list = ""

    eventID = 1

    for tag in split_xpath(xpath) :

        sequence_list += str(sequence) + "\t" + eventID + "\t" + sequence_tag(tag)
        eventID += 1

    return sequence_list

def sequence_xpath_old(sequence, xpath) :

    sequence_list = ""

    first = 0

    for tag in split_xpath(xpath) :

        sequence_list += sequence_tag(sequence, tag)

    return sequence_list

def sequence_xpaths(difftrace, sequence_data) :

    sequence = 1
    for xpath in difftrace :
        sequence_data.write(sequence_xpath(sequence, xpath))
        sequence += 1

    return

difftrace_file = open(sys.argv[1], "r")
difftrace = difftrace_file.read().split("\n")
difftrace_file.close()

sequence_data = open(sys.argv[2], "w")

sequence_xpaths(difftrace, sequence_data)

