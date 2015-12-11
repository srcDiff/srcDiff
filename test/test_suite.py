#!/usr/bin/env python
#
# test_suite.py
#
# Original by Michael L. Collard
# Updated/Modified for srcdiff by Michael John Decker

import sys
import os.path
import re
import subprocess
import difflib
from datetime import datetime, time
import commands

error_filename = "srcDiffTestReport"
error_filename_extension = ".txt"

SHELL_ROWS, SHELL_COLUMNS = commands.getoutput('stty size').split(' ')

FIELD_WIDTH_LANGUAGE   = 12
FIELD_WIDTH_URL        = 20
FIELD_WIDTH_TEST_CASES = int(SHELL_COLUMNS) - (FIELD_WIDTH_LANGUAGE + 1) - (FIELD_WIDTH_URL + 1)

sperrorlist = []

srcml_client = "srcml"
switch_utility = "../bin/switch_differences"
srcdiff_utility = "../bin/srcdiff"

# extracts a particular unit from a srcML file
def safe_communicate(command, inp) :

	try :
		return subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate(inp)[0]
	except OSError, (errornum, strerror) :
		try :
			return subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate(inp)[0]
		except OSError, (errornum, strerror) :
			sperrorlist.append((command, inp, errornum, strerror))
			raise

# extracts a particular unit from a srcML file
def safe_communicate_file(command, filename) :

	newcommand = command[:]
	newcommand.append(filename)
	try :
		return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
	except OSError, (errornum, strerror) :
		try :
			return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
		except OSError, (errornum, strerror) :
			sperrorlist.append((command, filename, errornum, strerror))
			raise

# extracts a particular unit from a srcML file
def safe_communicate_two_files(command, filename_one, filename_two, url) :

	newcommand = command[:]
	newcommand.append(filename_one)
	newcommand.append(filename_two)

	try :
		return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
	except OSError, (errornum, strerror) :
		try :
			return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
		except OSError, (errornum, strerror) :
			sperrorlist.append((command, url, errornum, strerror))
			raise

# extracts a particular unit from a srcML file
def extract_unit(src, count) :

	command = [srcml_client, "--unit=" + str(count), "--output-xml"]

	return safe_communicate(command, src)

def name2filestr(src_filename) :
	file = open(src_filename).read()
	
	return file

# converts a srcML file back to text
def extract_source(srcDiff, operation) :

	# run the srcML extractor
	command = [srcml_client, "--revision", operation]

	return safe_communicate(command, srcDiff)

# switch diff order
def switch_differences(srcML) :

	# run the srcml processor
	command = [switch_utility]

	return safe_communicate(command, srcML)

# converts from unix to dos line endings
def unix2dos(srctext) :

	# run the srcml processor
	command = ['unix2dos']

	return safe_communicate(command, srctext)

# find differences of two files
def linediff(xml_filename1, xml_filename2) :

	if xml_filename1 != xml_filename2 :
		return list(difflib.unified_diff(xml_filename1.splitlines(1), xml_filename2.splitlines(1)))
	else :
		return ""

# find differences of two files
def srcdiff(source_file_version_one, source_file_version_two, encoding, language, url, filename, prefixlist) :

	command = [globals()["srcdiff_utility"], "-f", filename]

	extension = "cpp"
	if language == "Java" :
		extension = "java"

	temp_file = open("temp_file_one." + extension, "w")
	temp_file.write(source_file_version_one)
	temp_file.close()

	temp_file = open("temp_file_two." + extension, "w")
	temp_file.write(source_file_version_two)
	temp_file.close()

	return safe_communicate_two_files(command, "temp_file_one." + extension, "temp_file_two." + extension, url).replace(" revision=\"0.8.0\"", "")

def get_srcml_attribute(xml_file, command) :

	last_line = safe_communicate([srcml_client, command], xml_file)

	return last_line.strip()

def get_srcml_attribute_file(xml_file, command) :

	last_line = safe_communicate_file([srcml_client, "--quiet", command], xml_file)

	return last_line.strip()

# filename attribute
def get_filename(xml_file) :

	return get_srcml_attribute(xml_file, "--show-filename")

# xmlns attribute
def get_full_xmlns(xml_file) :

	l = []
	for a in get_srcml_attribute(xml_file, "--info").split() :
		if a[0 :5] == "xmlns" :
			l.append("--" + a.replace('"', ""))
	
	return l

# xmlns attribute
def default_xmlns(l) :

	newl = []
	for a in l :
		url = a.split('=')[1]
		if url == 'http ://www.sdml.info/srcML/src' or url == 'http ://www.sdml.info/srcML/cpp' or url == 'http ://www.sdml.info/srcML/srcerr' :
			newl.append(a)
	return newl

def nondefault_xmlns(l) :

	newl = []
	for a in l :
		url = a.split('=')[1]
		if not(url == 'http ://www.sdml.info/srcML/src' or url == 'http ://www.sdml.info/srcML/cpp' or url == 'http ://www.sdml.info/srcML/srcerr') :
			newl.append(a)
	return newl

# version of srcml2src
def srcml2src_version() :

	last_line = safe_communicate([srcml_client, "-V"], "")

	return last_line.splitlines()[0].strip()

# number of nested units
def get_nested(xml_file) :

	snumber = safe_communicate([srcml_client, "-n"], xml_file)

	if snumber != "" :
		return int(snumber)
	else :
		return 0

def get_line_count(count) :

	# adjust for count width
	if count > 99 :
		line_count = 3
	elif count > 9 :
		line_count = 2
	else :
		line_count = 1

	# space after count
	line_count += 1

	return line_count

class Tee(object) :
	def __init__(self, name) :
		self.file = open(name, "w")
		self.stdout = sys.stdout
		sys.stdout = self

	def __del__(self) :
		sys.stdout = self.stdout
		self.file.close()

	def write(self, data) :
		self.file.write(data)
		self.stdout.write(data)

Tee(error_filename)

print "Testing:"
print 

print srcml2src_version()
print

# Handle optional dos line endings
doseol = False
if len(sys.argv) > 1 and sys.argv[1] == "--dos" :
		sys.argv.pop(0)
		doseol = True

specname = ""
if len(sys.argv) > 1 :
	specname = sys.argv[1]

if specname != "" :
	print specname

specnum = 0
speclang = ""
if len(sys.argv) == 3 :
	result = sys.argv[2]
	if result == "C++" or result == "C" or result == "C#" or result == "Objective-C" or result == "Java" :
		speclang = result
	else :
		specnum = int(sys.argv[2])
elif len(sys.argv) == 2 :
	result = sys.argv[1]
	if result == "C++" or result == "C" or result == "C#" or result == "Objective-C" or result == "Java" :
		speclang = result
		specname = ""
	else :
		specname = result
elif len(sys.argv) > 2 :
	specnum = int(sys.argv[2])

	if len(sys.argv) > 3 :
		speclang = sys.argv[3]

# base directory
base_dir = "suite"

errorlist = []

m = re.compile(specname + "$")

# source directory
source_dir = base_dir

# total number of errors
error_count = 0

# total test cases
total_count = 0

ure = re.compile("url=\"([^\"]*)\"", re.M)
fre = re.compile("filename=\"([^\"]*)\"", re.M)
lre = re.compile("language=\"([^\"]*)\"", re.M)
vre = re.compile("src-version=\"([^\"]*)\"", re.M)
ere = re.compile("encoding=\"([^\"]*)\"", re.M)
nre = re.compile("units=\"([^\"]*)\"", re.M)

try :
			
	# process all files		
	for root, dirs, files in os.walk(source_dir, topdown=True) :		

		# process all files
		for name in files :
			try : 
	
				# only process xml files
				if os.path.splitext(name)[1] != ".xml" :
					continue

				# full path of the file
				xml_filename = os.path.join(root, name)
			
				# get all the info
				info = get_srcml_attribute_file(xml_filename, "--longinfo")
				if info == None :
					print "Problem with", xml_filename
					continue

				# url of the outer unit element
				ureinfo = ure.search(info)
				url = ureinfo.group(1)

				# only process if url name matches or is not given
				if specname != "" and m.match(url) == None :
					continue

				# encoding of the outer unit
				encoding = ere.search(info).group(1)
			
				# version of the outer unit
				version = ""
				vre_result = vre.search(info)
				if vre_result :
					version = vre_result.group(1)
		
				# number of nested units
				number = int(nre.search(info).group(1))
		
				if specnum == 0 :
					count = 0
				else :
					count = specnum - 1

				# read entire file into a string
				filexml = name2filestr(xml_filename)

				get_language = True				

				line_count = 0
				while count == 0 or count < number :

					try : 

						count = count + 1

						if specnum != 0 and count > specnum :
							break

						# save the particular nested unit
						if number == 0 :
							unitxml = filexml
						else :
							unitxml = extract_unit(filexml, count)

						if get_language :

							get_language = False

							# language of the entire document with a default of C++
							language = lre.search(unitxml).group(1)
							if len(language) == 0 :
								language = "C++"

							# only process if language matches or is not given
							if speclang != "" and language != speclang :
								continue
						
							# output language and url
							print
							print language.ljust(FIELD_WIDTH_LANGUAGE), url.ljust(FIELD_WIDTH_URL),

						# total count of test cases
						total_count = total_count + 1

						# convert the unit in xml to text
						unit_text_version_one = extract_source(unitxml, "0")
						unit_text_version_two = extract_source(unitxml, "1")

						# convert the unit in xml to text (if needed)
						if doseol :
								unittext = unix2dos(unittext)

						# convert the text to srcML
						unitsrcmlraw = srcdiff(unit_text_version_one, unit_text_version_two, encoding, language, url, get_filename(unitxml), default_xmlns(get_full_xmlns(unitxml)))

						# additional, later stage processing
						unitsrcml = unitsrcmlraw # srcML2srcMLStages(unitsrcmlraw, nondefault_xmlns(get_full_xmlns(unitxml)))

						test_number = count * 2 - 1
						if url == "interchange" :
							test_number = count
						
						# find the difference
						result = linediff(unitxml, unitsrcml)
						line_count += get_line_count(test_number)
						if line_count > FIELD_WIDTH_TEST_CASES :
							print "\n", "".rjust(FIELD_WIDTH_LANGUAGE), "...".ljust(FIELD_WIDTH_URL),
							line_count = get_line_count(test_number)

						if result != "" :

							error_count += 1			
							errorlist.append((url + " " + language, test_number, result))

							# part of list of nested unit number in output
							print "\033[0;31m" + str(test_number) + "\033[0m",

						elif number != 0 :

							# part of list of nested unit number in output
							print "\033[0;33m" + str(test_number) + "\033[0m",

						if url == "interchange" :
							continue

						# total count of test cases
						total_count += 1

						# convert the unit in xml to text
						unitxml = switch_differences(unitxml)

						unit_text_version_one = extract_source(unitxml, "0")
						unit_text_version_two = extract_source(unitxml, "1")

						# convert the unit in xml to text (if needed)
						if doseol :
								unittext = unix2dos(unittext)

						# convert the text to srcML
						unitsrcmlraw = srcdiff(unit_text_version_one, unit_text_version_two, encoding, language, url, get_filename(unitxml), default_xmlns(get_full_xmlns(unitxml)))

						# additional, later stage processing
						unitsrcml = unitsrcmlraw # srcML2srcMLStages(unitsrcmlraw, nondefault_xmlns(get_full_xmlns(unitxml)))
						
						test_number += 1

						# find the difference
						result = linediff(unitxml, unitsrcml)
						line_count += get_line_count(test_number)
						if line_count > FIELD_WIDTH_TEST_CASES :
							print "\n", "".rjust(FIELD_WIDTH_LANGUAGE), "...".ljust(FIELD_WIDTH_URL),
							line_count = get_line_count(test_number)

						if result != "" :

							error_count += 1						
							errorlist.append((url + " " + language, count * 2, result))

							# part of list of nested unit number in output
							print "\033[0;31m" + str(count * 2) + "\033[0m",

						elif number != 0 :

							# part of list of nested unit number in output
							print "\033[0;33m" + str(count * 2) + "\033[0m",
	
					except OSError, (errornum, strerror) :
						continue

			except OSError, (errornum, strerror) :
				continue

	ki = False
except KeyboardInterrupt :
	ki = True
	
print
print
print "Report:"
if ki :
	print
	print "Testing stopped by keyboard"
	
# output error counts
# and delete error file
print

f = open(error_filename, "w")

if error_count == 0 :
	print "No errors out of " + str(total_count) + " cases" 
else :

	# break errorlist into two, one with original name, one with a name with a dot in it
	oerrorlist = []
	xerrorlist = []
	for e in errorlist :
		if str(e[0]).count(".") == 0 :
			oerrorlist.append(e);
		else :
			xerrorlist.append(e);

	print "Errors:  " + str(error_count) + " out of " + str(total_count),
	if str(total_count) == "1" :
		print " case", 
	else :
		print " cases", 
	print "\n"
	print "Errorlist:"
	nxerrorlist = xerrorlist[:]
	for e in oerrorlist :
		othererror = ""
		for x in xerrorlist[:] :
			if str(e[0]).split(' ')[1] != str(x[0]).split(' ')[1] :
				continue

			if str(e[1]) != str(x[1]) :
				continue

			if str(x[0]).split('.')[0] == str(e[0]).split(' ')[0] :
				othererror = othererror + " " + str(x[0]).split(' ')[0].split('.')[1]

			xerrorlist.remove(x)

		print e[0], e[1], othererror, "\n", "".join(e[2][3:])

	for e in xerrorlist :
		print e[0], e[1], "\n", "".join(e[2][3:])

# output tool errors counts
print
if len(sperrorlist) == 0 :
	print "No tool errors"
else :
	print "Tool errors:  " + str(len(sperrorlist))
	print "Tool Errorlist:"
	for e in sperrorlist :
		f.write(str(e[0]) + " " + str(e[1]) + " " + str(e[2]) + " " + str(e[3]) + "\n")
		print e[0], e[1], e[2], e[3]

current_time = datetime.now()
os.rename(error_filename, error_filename + "_" + current_time.isoformat().replace(":", "-") + error_filename_extension)
f.close()

if os.path.exists("temp_file_one.cpp") :
		os.remove("temp_file_one.cpp")
if os.path.exists("temp_file_one.java") :
		os.remove("temp_file_one.java")
if os.path.exists("temp_file_two.cpp") :
		os.remove("temp_file_two.cpp")
if os.path.exists("temp_file_two.java") :
		os.remove("temp_file_two.java")


# output tool version
print
print srcml2src_version(), srcml_client

exit
