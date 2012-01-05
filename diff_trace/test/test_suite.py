#!/usr/bin/python

import subprocess
import sys
import string

srcml2src_utility = "srcml2src"
strip_units = "strip_units.xsl"
difftrace = "../difftrace"

srcDiff_xpath = "//diff:*[not(diff:common)]/node()"

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

def create_xpath_list(filename) :

    command = [difftrace, filename]

    xpath_list = string.split(run(command, ""), "\n")

    return xpath_list

def create_srcDiff_xpath_list(srcDiff) :

    command = [srcml2src_utility, "--xpath", srcDiff_xpath]

    results = run(command, srcDiff)

    command = [srcml2src_utility, "--xslt", strip_units]

    srcDiff_xpath_list = run(command, results)

    return srcDiff_xpath_list

srcDiff_file = open(sys.argv[1], "r")
srcDiff = srcDiff_file.read()
srcDiff_file.close()

print create_xpath_results(srcDiff, create_xpath_list(sys.argv[1]))
