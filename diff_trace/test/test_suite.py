#!/usr/bin/python

import subprocess
import sys
import string

srcml2src_utility = "srcml2src"
strip_units = "strip_units.xsl"
difftrace = "../difftrace"

def run(command, inputs) :

    process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    output = process.communicate(inputs)[0]

    return output

def create_xpath_results(srcDiff, xpath_list) :

    xpath_results = ""

    for xpath in xpath_list :

        command = [srcml2src_utility, "--xpath", xpath]
        
        results = run(command, srcDiff)

        command = [srcml2src_utility, "--xslt", strip_units]

        xpath_results = xpath_results + run(command, results)
    
    return xpath_results

srcDiff_file = open(sys.argv[1], "r")
srcDiff = srcDiff_file.read()
srcDiff_file.close()

command = [difftrace, sys.argv[1]]
xpath_list = string.split(run(command, ""), "\n")

print create_xpath_results(srcDiff, xpath_list)
