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

def create_difftrace_xpath_results(srcDiff, difftrace_xpath_list) :

    difftrace_xpath_results = []

    for xpath in difftrace_xpath_list :

        command = [srcml2src_utility, "--xpath", xpath]
        
        results = run(command, srcDiff)

        command = [srcml2src_utility, "--xslt", strip_units]

        difftrace_xpath_results.append(run(command, results))
    
    return difftrace_xpath_results

def create_difftrace_xpath_list(filename) :

    command = [difftrace, filename]

    difftrace_xpath_list = string.split(run(command, ""), "\n")

    return difftrace_xpath_list

def create_srcDiff_xpath_results(srcDiff) :

    command = [srcml2src_utility, "--xpath", srcDiff_xpath]

    srcDiff_results = run(command, srcDiff)

    command = [srcml2src_utility "--units"]

    num_units = run(command, srcDiff_results)

    srcDiff_xpath_results = []

    for i in range(1, num_units + 1) :

        srcDiff_xpath_results.append(create_srcDiff_xpath_result(srcDiff_results, i))

    return srcDiff_xpath_results

def create_srcDiff_xpath_result(srcDiff_results, unit) :

    command = [srcml2src_utility, "--unit", unit]

    srcDiff_xpath_result = run(command, srcDiff_results)

    command = [srcml2src_utility, "--xslt", split_units]

    srcDiff_xpath_result = run(command, srcDiff_xpath_results)

    return srcDiff_xpath_result

def diff_xpath_results(srcDiff_xpath_results, difftrace_xpath_results) :

    return 

srcDiff_file = open(sys.argv[1], "r")
srcDiff = srcDiff_file.read()
srcDiff_file.close()

difftrace_xpath_results = create_difftrace_xpath_results(srcDiff, create_difftrace_xpath_list(sys.argv[1]))
srcDiff_xpath_results = create_srcDiff_xpath_results(srcDiff)


