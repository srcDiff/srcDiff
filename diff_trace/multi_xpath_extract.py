#!/usr/bin/python

import subprocess
import sys

def run(command, inputs) :

    process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    output = process.communicate(inputs)[0]

    return output


srcDiff_file = open(sys.argv[1])
xpath_file = open(sys.argv[2])
