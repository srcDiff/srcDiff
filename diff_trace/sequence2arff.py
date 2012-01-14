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

def create_sequence_list(num_sequence) :

    return range(1, num_sequence + 1)

def write_sequence(arff_file, sequence_list) :

    arff_file.write("@attribute sequence { ")

    sequence_length = len(sequence_list)

    if sequence_length > 0 :
        arff_file.write(str(sequence_list[0]))
    
        for sequence in sequence_list[ 1 : ] :

            arff_file.write(", " + str(sequence))

    arff_file.write(" }\n")

    return
