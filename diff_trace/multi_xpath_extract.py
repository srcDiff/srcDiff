#!/usr/bin/python

import subprocess
import sys
import string

srcml2src_utility = "srcml2src"
strip_units = "strip_units.xsl"

def run(command, inputs) :

    process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    output = process.communicate(inputs)[0]

    return output


srcDiff_file = open(sys.argv[1], "r")
srcDiff = srcDiff_file.read()
srcDiff_file.close()

xpath_file = open(sys.argv[2], "r")
xpath_list = string.split(xpath_file.read(), "\n")

xpath_results = ""

for xpath in xpath_list :

    command = [srcml2src_utility, "--xpath", xpath]

    results = run(command, srcDiff)

    command = [srcml2src_utility, "--xslt", strip_units]

    xpath_results = xpath_results + run(command, results)
    
print xpath_results
