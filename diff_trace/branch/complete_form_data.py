#!/usr/bin/python

##
# Split XPath into the data for Weka's
# General Sequence Pattern Algorithm

import sys

def wrap_tag(tag) :
    return "\'" + tag + "\'"

# main
sequence_data = open(sys.argv[1], "r")
gsp_data = open(sys.argv[2], "w")

gsp_data.write("@relation source_changes\n")

# count variable
sequence_number = 0

# tags
tags = []
unique_tags = set()

# count the number of entries
for data_line in sequence_data :

    sequence_number = data_line.split("\t")[0]
    tag = data_line.split("\t")[1].split("\n")[0]

    if tag not in unique_tags :
        tags.append(tag)
        unique_tags = unique_tags.union([ tag ])

sequence_data.close()

line_count = 0

for change_number in range(int(sequence_number)) :

    line_count += 1

    if line_count == 1 :
        gsp_data.write("@attribute change { " + str(change_number + 1))
    else :
        gsp_data.write(", " + str(change_number + 1))

if line_count > 0 :
    gsp_data.write(" }\n")

line_count = 0

for structure in tags :

    line_count += 1

    if line_count == 1 :
        gsp_data.write("@attribute structure { " + wrap_tag(structure))
    else :
        gsp_data.write(", " + wrap_tag(structure))

if line_count > 0 :
    gsp_data.write(" }\n")

sequence_data = open(sys.argv[1], "r")

gsp_data.write("@data\n")

for data_line in sequence_data :

    gsp_data.write(data_line.split("\t")[0] + "," + wrap_tag(data_line.split("\t")[1].split("\n")[0]) + "\n")

gsp_data.close()
sequence_data.close()
