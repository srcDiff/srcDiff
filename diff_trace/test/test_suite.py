#!/usr/bin/python

import subprocess
import sys
import string

srcml2src = "srcml2src"
xsltproc = "xsltproc"
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

        if xpath == "" :
            continue

        command = [srcml2src, "--xpath", xpath]
        
        difftrace_xpath_result = run(command, srcDiff)

        command = [srcml2src, "--units"]

        num_units = int(run(command, difftrace_xpath_result))

        if num_units != 1 :
            print "Non unique XPath: " + xpath

        command = [srcml2src, "--xml", "--unit", str(1)]

        difftrace_xpath_result = run(command, difftrace_xpath_result)

        command = [srcml2src, "--xslt", strip_units]

        difftrace_xpath_results.append(run(command, difftrace_xpath_result))
    
    return difftrace_xpath_results

def create_difftrace_xpath_list(filename) :

    command = [difftrace, filename]

    difftrace_xpath_list = string.split(run(command, ""), "\n")

    return difftrace_xpath_list

def create_srcDiff_xpath_results(srcDiff) :

    command = [srcml2src, "--xpath", srcDiff_xpath]

    srcDiff_results = run(command, srcDiff)

    command = [srcml2src, "--units"]

    num_units = int(run(command, srcDiff_results))

    srcDiff_xpath_results = []

    for i in range(1, num_units + 1) :

        srcDiff_xpath_results.append(create_srcDiff_xpath_result(srcDiff_results, i))

    return srcDiff_xpath_results

def create_srcDiff_xpath_result(srcDiff_results, unit) :

    command = [srcml2src, "--xml", "--unit", str(unit)]

    srcDiff_xpath_result = run(command, srcDiff_results)

    command = [srcml2src, "--xslt", strip_units]

    srcDiff_xpath_result = run(command, srcDiff_xpath_result)

    return srcDiff_xpath_result

def diff_xpath_results(srcDiff_xpath_results, difftrace_xpath_results) :

    diff_error_list = []

    srcDiff_xpath_results_length = len(srcDiff_xpath_results)
    difftrace_xpath_results_length = len(difftrace_xpath_results)

    num_results = min(srcDiff_xpath_results_length, difftrace_xpath_results_length)

    for i in range(num_results) :

        if srcDiff_xpath_results[i] !=  difftrace_xpath_results[i] :
            diff_error_list.append(i)

    diff_error_list.extend(range(num_results, srcDiff_xpath_results_length))
    diff_error_list.extend(range(num_results, difftrace_xpath_results_length))

    return diff_error_list

srcDiff_file = open(sys.argv[1], "r")
srcDiff = srcDiff_file.read()
srcDiff_file.close()

difftrace_xpath_results = create_difftrace_xpath_results(srcDiff, create_difftrace_xpath_list(sys.argv[1]))
srcDiff_xpath_results = create_srcDiff_xpath_results(srcDiff)

diff_error_list =  diff_xpath_results(srcDiff_xpath_results, difftrace_xpath_results)

srcDiff_xpath_results_length = len(srcDiff_xpath_results)
difftrace_xpath_results_length = len(difftrace_xpath_results)

for i in diff_error_list :

    print str(i)

    # put in check not out of bounds
    print "srcDiff: '" + srcDiff_xpath_results[i] + "'"
    print "difftrace: '" + difftrace_xpath_results[i] + "'"
