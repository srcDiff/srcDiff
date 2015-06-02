#!/usr/bin/python
import sys
import os
import subprocess
import difflib
import re

source_dir = "suite"
FILE_WIDTH = 24
test_number = 1
test_count = 0
errors = []

file_pattern = None
test_number_filter = 0

def get_next_test(test_file) :

	line = test_file.readline()
	while line != None and line != "" and "ORIGINAL" not in line :
		line = test_file.readline()

	if line == None or line == "" :
		return None, None, None

	original = ""
	line = test_file.readline()
	while "END_ORIGINAL" not in line :
		original += line
		line = test_file.readline()

	line = test_file.readline()
	while "MODIFIED" not in line :
		line = test_file.readline()

	modified = ""
	line = test_file.readline()
	while "END_MODIFIED" not in line :
		modified += line
		line = test_file.readline()

	line = test_file.readline()
	while "SUMMARY" not in line :
		line = test_file.readline()

	summary = ""
	line = test_file.readline()
	while "END_SUMMARY" not in line :

		while "#" in line :
			line = line.replace("#", "\x1b[1m", 1)
			line = line.replace("#", "\x1b[0m", 1)

		summary += line
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

	os.remove("original.cpp")
	os.remove("modified.cpp")

	diff = list(difflib.unified_diff(output.splitlines(1), summary.splitlines(1)))

	if diff != [] :
		print "\033[0;31m" + str(globals()['test_number']) + "\033[0m",
		globals()['errors'].append(diff)
	else :
		print "\033[0;33m" + str(globals()['test_number']) + "\033[0m",

def run_test_file(file_name) :

	print file_name[:len(file_name) - 4].ljust(globals()['FILE_WIDTH']), " ",
	globals()['test_number'] = 1

	test_file = open(os.path.join(globals()['source_dir'], file_name), "r");

	original, modified, summary = get_next_test(test_file)
	while original != None :

			if globals()['test_number_filter'] == 0 or globals()['test_number_filter'] == globals()['test_number'] :
				run_test(original, modified, summary)

			globals()['test_number'] += 1
			globals()['test_count'] += 1
			original, modified, summary = get_next_test(test_file)

	test_file.close()

	print

# main
os.environ['COLUMNS'] = "130"

argc = len(sys.argv)
if argc > 2 or (argc > 1 and not(sys.argv[1][0].isdigit())) :
	file_pattern = re.compile(sys.argv[1])

if argc > 2 :
	test_number_filter = int(sys.argv[2])
elif argc > 1 and sys.argv[1][0].isdigit() :
	test_number_filter = int(sys.argv[1])

for root, dirs, files in os.walk(source_dir, topdown=True) :

	for name in files :

		if file_pattern and not(file_pattern.match(name)) :
			continue

		run_test_file(name)

print
print len(errors), "errors out of", test_count, "tests"
for error in errors :
	print error[0], error[1], "".join(error[3:])
