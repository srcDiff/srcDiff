#!/usr/bin/python

import subprocess
import sys
import string
import os
import os.path
import re

srcml2src = "../../../../bin/srcml2src"
xsltproc = "xsltproc"
strip_units = "strip_units.xsl"
difftrace = "../difftrace"
suite_directory = "suite"

srcDiff_xpath = "//diff:*[not(self::diff:common)]/node()"

run_exceptions = []

xpath_errors = []

file_count = 0

test_count = 0

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

        difftrace_xpath_results.append(string.strip(run(command, difftrace_xpath_result), string.whitespace))
    
    return difftrace_xpath_results

def create_difftrace_xpath_list(srcDiff) :

    command = [difftrace]

    difftrace_xpath_list = string.split(run(command, srcDiff), "\n")

    return difftrace_xpath_list

def create_srcDiff_xpath_results(srcDiff) :

    command = [srcml2src, "--xpath", srcDiff_xpath]

    srcDiff_results = run(command, srcDiff)

    command = [srcml2src, "--units"]

    num_units = int(run(command, srcDiff_results))

    srcDiff_xpath_results = []

    for i in range(1, num_units + 1) :

        srcDiff_xpath_result = create_srcDiff_xpath_result(srcDiff_results, i);

        if srcDiff_xpath_result == "" :
            continue

        srcDiff_xpath_results.append(srcDiff_xpath_result)

    return srcDiff_xpath_results

def create_srcDiff_xpath_result(srcDiff_results, unit) :

    command = [srcml2src, "--xml", "--unit", str(unit)]

    srcDiff_xpath_result = run(command, srcDiff_results)

    command = [srcml2src, "--xslt", strip_units]

    srcDiff_xpath_result = string.strip(run(command, srcDiff_xpath_result), string.whitespace)

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

def output_diff_xpath_results(srcDiff_filename, srcDiff_xpath_results, difftrace_xpath_results, diff_error_list) :

    srcDiff_xpath_results_length = len(srcDiff_xpath_results)
    difftrace_xpath_results_length = len(difftrace_xpath_results)

    test_case_results = str(globals()['test_count']) + ". " + srcDiff_filename + ":\t"

    num_results = min(srcDiff_xpath_results_length, difftrace_xpath_results_length)
    
    for i in range(num_results) :

        globals()['test_count'] += 1

        if i in diff_error_list :

            test_case_results += "\033[31m" + str(i + 1) + "\033[0m "

            diff_error = "srcDiff:\t'" + srcDiff_xpath_results[i] + "'"
            diff_error += "\n"
            diff_error += "difftrace:\t'" + difftrace_xpath_results[i] + "'"

            globals()['xpath_errors'].append(diff_error)

        else :

            test_case_results += "\033[32m" + str(i + 1) + "\033[0m "

    for i in range(num_results, srcDiff_xpath_results_length) :

        globals()['test_count'] += 1

        test_case_results += "\033[31m" + str(i + 1) + "\033[0m "

        diff_error = "srcDiff:\t'" + srcDiff_xpath_results[i] + "'"
        diff_error += "\n"
        diff_error += "difftrace:\t''"

        globals()['xpath_errors'].append(diff_error)

    for i in range(num_results, difftrace_xpath_results_length) :

        globals()['test_count'] += 1

        test_case_results += "\033[31m" + str(i + 1) + "\033[0m "

        diff_error = "srcDiff:\t''"
        diff_error += "\n"
        diff_error += "difftrace:\t'" + difftrace_xpath_results[i] + "'"

        globals()['xpath_errors'].append(diff_error)

    print test_case_results

    return

def test_difftrace_on_srcDiff_file(path, srcDiff_filename) :

    srcDiff_file = open(path + "/" + srcDiff_filename, "r")
    srcDiff = srcDiff_file.read()
    srcDiff_file.close()

    try :

        difftrace_xpath_results = create_difftrace_xpath_results(srcDiff, create_difftrace_xpath_list(srcDiff))
        srcDiff_xpath_results = create_srcDiff_xpath_results(srcDiff)

        diff_error_list =  diff_xpath_results(srcDiff_xpath_results, difftrace_xpath_results)

        output_diff_xpath_results(srcDiff_filename, srcDiff_xpath_results, difftrace_xpath_results, diff_error_list)

    except Exception :
        globals()['run_exceptions'].append(str(globals()['file_count']) + ": " + srcDiff_filename)

    return

def process_dir(path, filename_regex) :

    for srcDiff_filename in os.listdir(path) :

        if srcDiff_filename[0] == "." :
            continue

        if os.path.isdir(path + "/" + srcDiff_filename) :
            continue

        if filename_regex != None and re.match(filename_regex, srcDiff_filename) == None:
            continue

        globals()['file_count'] += 1

        test_difftrace_on_srcDiff_file(path, srcDiff_filename)

    for srcDiff_filename in os.listdir(path) :

        if srcDiff_filename[0] == "." :
            continue

        if os.path.isfile(path + "/" + srcDiff_filename) :
            continue

        if filename_regex != None and re.match(filename_regex, srcDiff_filename) == None:
            continue

        process_dir(path + "/" + srcDiff_filename, filename_regex)

    return

filename_regex = None

if len(sys.argv) > 1 :
    filename_regex = sys.argv[1]

process_dir(suite_directory, filename_regex)

error_count = len(xpath_errors)

print
print "Test Summary:\t" + str(error_count) + " errors out of " + str(test_count) + " cases"
print

if error_count > 0 :

    print "Errors"

    for i in range(error_count) :
        print str(i + 1) + ":\t" + xpath_errors[i]
        print "\n"

run_exception_count = len(run_exceptions)

if run_exception_count > 0 :

    print "Run Exceptions"
    print

    for i in range(run_exception_count) :
        print run_exceptions[i]
