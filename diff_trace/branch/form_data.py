#!/usr/bin/python

##
# Split XPath into the data for Weka's
# General Sequence Pattern Algorithm

import sys

# split an xpath for a delete or insert
def split_xpath_difference(sequence_number, xpath_difference, sequence_data) :

    operation = xpath_difference.split("\t")[0]
    xpath = xpath_difference.split("\t")[1].split("\n")[0]

    split_xpath(sequence_number, operation, xpath, sequence_data)

# split apart an xpath expression
def split_xpath(sequence_number, operation, xpath, sequence_data) :

    tags = xpath.split("/")

    for tag in tags :
        if tag != "" :

            name = get_name(tag)
            predicate_list(create_clean_predicate_list(tag))
            predicate_list_length = len(predicate_list_length)

            for i in range(predicate_list_length) :

                sequence_data.write(str(sequence_number) + "\t" + name + predicate_list[i] + "\n");

                for j in range(1, predicate_list_length) :

            sequence_data.write(str(sequence_number) + "\t" + tag + "\n");

def clean_tag(tag) :
    return tag.replace("\'", "")

def create_clean_predicate_list(tag):

    predicate_list = []
    while tag.find("[") != -1 :

        predicate_list.append(clean_tag(tag[tag.find("[") : tag.find("]") + 1]))

    return predicate_list

def get_name(tag):

    if tag.find("[") != -1 :
        return tag[0 : tag.find("[")]

    return tag

# main
diff_list = open(sys.argv[1], "r")
sequence_data = open(sys.argv[2], "w")

# count variable
sequence_number = 0

for xpath_difference in diff_list :

    if xpath_difference != "\n" :
        sequence_number += 1

        split_xpath_difference(sequence_number, xpath_difference, sequence_data)

sequence_data.close()
diff_list.close()
