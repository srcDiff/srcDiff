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

    tags = xpath.split("/src:")

    for tag in tags :
        if tag != "" :
            if tag.find("[") != -1 :
                sequence_data.write(str(sequence_number) + "\t" + operation + "=src:" + tag[0 : tag.find("[")] + "\n");
                sequence_data.write(str(sequence_number) + "\t" + operation + "=src:" + clean_tag(tag) + "\n");
            else :
                sequence_data.write(str(sequence_number) + "\t" + operation + "=src:" + tag + "\n");

def clean_tag(tag) :
    return tag.replace("\'", "")

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
