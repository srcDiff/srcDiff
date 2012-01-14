#!/usr/bin/python

import sys
import itertools

def get_num_sequences(sequence_data) :

    num_sequences = sequence_data[len(sequence_data) - 2].split("\t")[0]

    return int(num_sequences)

def collect_structures(sequence_data) :

    structures = []

    for line in sequence_data :

        structures.append(line.split("\t")[1])

    return structures

def unique_structures(structures) :

    u_structures = set()

    for structure in structures :

        if structure not in u_structures :
            u_structures = u_structures.union(structure)

    return list(u_structures)

def create_sequence_list(num_sequence) :

    return range(1, num_sequence + 1)

def write_sequences(arff_file, sequence_list) :

    arff_file.write("@attribute sequence { ")

    sequence_length = len(sequence_list)

    if sequence_length > 0 :
        arff_file.write(str(sequence_list[0]))
    
        for sequence in sequence_list[ 1 : ] :

            arff_file.write(", " + str(sequence))

    arff_file.write(" }\n")

    return

def write_structures(arff_file, structures) :

    arff_file.write("@attribute structure { ")

    structure_length = len(structures)

    if structure_length > 0 :
        arff_file.write(str(structures[0]))
    
        for structure in structures[ 1 : ] :

            arff_file.write(", " + str(structure))

    arff_file.write(" }\n\n")

    return

def write_data(arff_file, sequence_data) :

    arff_file.write("@data\n\n")

    for line in sequence_data :

        data = line.split("\t")
        arff_file.write(data[0])
        arff_file.write(", '")
        arff_file.write(data[1])
        arff_file.write("'\n")

    return

def create_arff(sequence_filename, arff_filename) :

    sequence_file = open(sequence_filename, "r")
    sequence_data = sequence_file.read().split("\n")
    sequence_file.close()

    sequence_data = sequence_data[ : len(sequence_data) - 1]

    arff_file = open(arff_filename, "w")

    arff_file.write("@relation xpath_sequence_mining\n")

    num_sequences = get_num_sequences(sequence_data)
    sequence_list = create_sequence_list(num_sequences)
    write_sequences(arff_file, sequence_list)

    structures = collect_structures(sequence_data)
    structures = unique_structures(structures)

    write_structures(arff_file, structures)

    write_data(arff_file, sequence_data)

    return


create_arff(sys.argv[1], sys.argv[2])
