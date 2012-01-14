#!/usr/bin/python

import sys

def get_num_sequence(sequence_data) :

    num_sequence = sequence_data[len(sequence_data) - 1].split[0]

    return num_sequence

def collect_structures(sequence_data) :

    structures = []

    for line in sequence_data :
        structures.append(line.split("\t")[1])


    return structures

