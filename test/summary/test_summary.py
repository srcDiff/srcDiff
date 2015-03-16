#!/usr/bin/python
import sys
import os
import subprocess
import difflib

source_dir = "suite"
FILE_WIDTH = 10
test_count = 1
errors = []

def get_next_test(test_file) :

	line = test_file.readline()
	while line != None and line != "" and "ORIGINAL" not in line :
		line = test_file.readline()

	if line == None or line == "" :
		return None, None, None

	original = ""
	line = test_file.readline()
	while "END_ORIGINAL" not in line :
		original = original + line
		line = test_file.readline()

	line = test_file.readline()
	while "MODIFIED" not in line :
		line = test_file.readline()

	modified = ""
	line = test_file.readline()
	while "END_MODIFIED" not in line :
		modified = modified + line
		line = test_file.readline()

	line = test_file.readline()
	while "SUMMARY" not in line :
		line = test_file.readline()

	summary = ""
	line = test_file.readline()
	while "END_SUMMARY" not in line :
		summary = summary + line
		line = test_file.readline()

	return original, modified, summary

def run_test(original, modified, summary) :

	original_file = open("original.cpp", "w")
	original_file.write(original)
	original_file.close();

	modified_file = open("modified.cpp", "w")
	modified_file.write(modified)
	modified_file.close();

	command = ["srcdiff", "--summary=text", "original.cpp", "modified.cpp"]

	try :
		output = subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
	except OSError, (errornum, strerror) :
		try :
			output = subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
		except OSError, (errornum, strerror) :
			sperrorlist.append((command, inp, errornum, strerror))
			raise

	diff = list(difflib.unified_diff(output.splitlines(1), summary.splitlines(1)))

	if diff != [] :
		print "\033[0;31m" + str(globals()['test_count']) + "\033[0m",
		globals['errors'].append(diff)
	else :
		print "\033[0;33m" + str(globals()['test_count']) + "\033[0m",

 
def run_test_file(file_name) :

	print file_name.ljust(globals()['FILE_WIDTH']), " ",
	globals()['test_count'] = 1

	test_file = open(os.path.join(globals()['source_dir'], file_name), "r");

	original, modified, summary = get_next_test(test_file)
	while original != None :
			run_test(original, modified, summary)
			original, modified, summary = get_next_test(test_file)

	test_file.close()

for root, dirs, files in os.walk(source_dir, topdown=True) :

	for name in files :

		run_test_file(name)


for error in errors :
	print diff[0], diff[1], "".join(diff[3:])
