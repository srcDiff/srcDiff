#!/usr/bin/env python
#
# update.py
#
# Michael L. Collard

import sys
import os.path
import re
import subprocess
import difflib
from datetime import datetime, time

maxcount = 700
error_filename = "srcMLTestReport"
error_filename_extension = ".txt"

FIELD_WIDTH_LANGUAGE = 5
FIELD_WIDTH_DIRECTORY = 27
MAX_COUNT = 29
sperrorlist = []

srcmltranslator = os.environ.get("SRC2SRCML")
if srcmltranslator == "" or srcmltranslator == None:
	srcmltranslator = "../bin/src2srcml"

srcmlutility = os.environ.get("SRCML2SRC")
if srcmlutility == "" or srcmlutility == None:
	srcmlutility = "../bin/srcml2src"

# srcExpr translator
srcexprtranslator = '/home/collard/srcML/trunk/srcexpr/src/srcexpr'

# extracts a particular unit from a srcML file
def safe_communicate(command, inp):

	try:
		return subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate(inp)[0]
	except OSError, (errornum, strerror):
		try:
			return subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate(inp)[0]
		except OSError, (errornum, strerror):
			sperrorlist.append((command, xml_filename, errornum, strerror))
			raise

# extracts a particular unit from a srcML file
def safe_communicate_file(command, filename):

	newcommand = command[:]
	newcommand.append(filename)
	try:
		return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
	except OSError, (errornum, strerror):
		try:
			return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0]
		except OSError, (errornum, strerror):
			sperrorlist.append((command, xml_filename, errornum, strerror))
			raise

# extracts a particular unit from a srcML file
def extract_unit(src, count):

	command = [srcmlutility, "--unit=" + str(count), "--xml"]

	return safe_communicate(command, src)

def name2filestr(src_filename):
	file = open(src_filename).read()
	
	return file

# converts a srcML file back to text
def srcml2src(srctext, encoding):

	# run the srcml processor
	command = [srcmlutility]
	command.append("--src-encoding=" + encoding)

	return safe_communicate(command, srctext)

# converts from unix to dos line endings
def unix2dos(srctext):

	# run the srcml processor
	command = ['unix2dos']

	return safe_communicate(command, srctext)

# find differences of two files
def xmldiff(xml_filename1, xml_filename2):

	if xml_filename1 != xml_filename2:
		return list(difflib.unified_diff(xml_filename1.splitlines(1), xml_filename2.splitlines(1)))
	else:
		return ""

# find differences of two files
def src2srcML(text_file, encoding, language, directory, filename, prefixlist):

	command = [srcmltranslator, "-l", language, "--encoding=" + encoding]

	if directory != "":
		command.extend(["--directory", directory])

       	if filename != "":
		command.extend(["--filename", filename])

	command.extend(prefixlist)

	#print command

	# run the srcml processor
	command.append("--src-encoding=" + encoding)

	command.append("--quiet")

	return safe_communicate(command, text_file)

# additional processing stages
def srcML2srcMLStages(srcmlfile, otherxmlns):

	newfile = srcmlfile
	for a in otherxmlns:
		url = a.split('=')[1]
		if url == 'http://www.sdml.info/srcExpr':
			command = [srcexprtranslator]
			newfile = safe_communicate(command, newfile)
		
	return newfile

#
def getsrcmlattribute(xml_file, command):

	last_line = safe_communicate([srcmlutility, command], xml_file)

	return last_line.strip()

def getsrcmlattributefile(xml_file, command):

	last_line = safe_communicate_file([srcmlutility, command], xml_file)

	return last_line.strip()

# directory attribute
def getdirectory(xml_file):

	return getsrcmlattribute(xml_file, "-d")

# language attribute
def getlanguage(xml_file):
	
	return getsrcmlattribute(xml_file, "-l")

# xml encoding
def getencoding(xml_file):

	return getsrcmlattribute(xml_file, "-x")

# version attribute
def getversion(xml_file):

	return getsrcmlattribute(xml_file, "-x")

# filename attribute
def getfilename(xml_file):

	return getsrcmlattribute(xml_file, "-f")

# xmlns attribute
def getfullxmlns(xml_file):

	l = []
	for a in getsrcmlattribute(xml_file, "--info").split():
		if a[0:5] == "xmlns":
			l.append("--" + a.replace('"', ""))
	
	return l

# xmlns attribute
def defaultxmlns(l):

	newl = []
	for a in l:
		url = a.split('=')[1]
		if url == 'http://www.sdml.info/srcML/src' or url == 'http://www.sdml.info/srcML/cpp' or url == 'http://www.sdml.info/srcML/srcerr' or url == 'http://www.sdml.info/srcML/literal' or url == 'http://www.sdml.info/srcML/operator':
			newl.append(a)
	return newl

def nondefaultxmlns(l):

	newl = []
	for a in l:
		url = a.split('=')[1]
		if not(url == 'http://www.sdml.info/srcML/src' or url == 'http://www.sdml.info/srcML/cpp' or url == 'http://www.sdml.info/srcML/srcerr' or url == 'http://www.sdml.info/srcML/literal' or url == 'http://www.sdml.info/srcML/operator'):
			newl.append(a)
	return newl

# version of src2srcml
def src2srcmlversion():
	last_line = safe_communicate([srcmltranslator, "-V"], "")

	return last_line.splitlines()[0].strip()

# version of srcml2src
def srcml2srcversion():

	last_line = safe_communicate([srcmlutility, "-V"], "")

	return last_line.splitlines()[0].strip()

# number of nested units
def getnested(xml_file):

	snumber = safe_communicate([srcmlutility, "-n"], xml_file)

	if snumber != "":
		return int(snumber)
	else:
		return 0

class Tee(object):
    def __init__(self, name):
        self.file = open(name, "w")
        self.stdout = sys.stdout
        sys.stdout = self

    def __del__(self):
        sys.stdout = self.stdout
        self.file.close()

    def write(self, data):
        self.file.write(data)
        self.stdout.write(data)

Tee(error_filename)

print "Testing:"
print 

print src2srcmlversion()
print srcml2srcversion()
print

# Handle optional dos line endings
doseol = False
if len(sys.argv) > 1 and sys.argv[1] == "--dos":
        sys.argv.pop(0)
        doseol = True

specname = ""
if len(sys.argv) > 1:
	specname = sys.argv[1]

if specname != "":
	print specname

specnum = 0
speclang = ""
if len(sys.argv) == 3:
	result = sys.argv[2]
	if result == "C++" or result == "C" or result == "Java" or result == "C++0x":
		speclang = result
	else:
		specnum = int(sys.argv[2])
elif len(sys.argv) == 2:
	result = sys.argv[1]
	if result == "C++" or result == "C" or result == "Java" or result == "C++0x":
		speclang = result
		specname = ""
	else:
		specname = result
elif len(sys.argv) > 2:
	specnum = int(sys.argv[2])

	if len(sys.argv) > 3:
		speclang = sys.argv[3]

# base directory
base_dir = "../suite"

errorlist = []

#if not(os.path.isfile(srcmltranslator)):
#	print srcmltranslator + " does not exist."
#	exit

m = re.compile(specname + "$")

# source directory
source_dir = base_dir

# total number of errors
error_count = 0

# total test cases
total_count = 0

dre = re.compile("directory=\"([^\"]*)\"", re.M)
lre = re.compile("language=\"([^\"]*)\"", re.M)
vre = re.compile("src-version=\"([^\"]*)\"", re.M)
ere = re.compile("encoding=\"([^\"]*)\"", re.M)
nre = re.compile("units=\"([^\"]*)\"", re.M)

try:
			
	# process all files		
	for root, dirs, files in os.walk(source_dir, topdown=True):		

		# process all files
		for name in files:
			try: 
	
				# only process xml files
				if os.path.splitext(name)[1] != ".xml":
					continue

				# full path of the file
				xml_filename = os.path.join(root, name)
			
				# get all the info
				info = getsrcmlattributefile(xml_filename, "--longinfo")
				if info == None:
					print "Problem with", xml_filename
					continue

				# directory of the outer unit element
				dreinfo = dre.search(info)
				directory = dreinfo.group(1)

				# only process if directory name matches or is not given
				if specname != "" and m.match(directory) == None:
					continue
			
				# language of the entire document with a default of C++
				language = lre.search(info).group(1)
				if len(language) == 0:
					language = "C++"

				# only process if language matches or is not given
				if speclang != "" and language != speclang:
					continue
			
				# output language and directory
				print
				print language.ljust(FIELD_WIDTH_LANGUAGE), " ", directory.ljust(FIELD_WIDTH_DIRECTORY), " ",

				# encoding of the outer unit
				encoding = ere.search(info).group(1)
			
				# version of the outer unit
				version = ""
				vre_result = vre.search(info)
				if vre_result:
					version = vre_result.group(1)
		
				# number of nested units
				number = int(nre.search(info).group(1))
		
				if specnum == 0:
					count = 0
				else:
					count = specnum - 1

				# read entire file into a string
				filexml = name2filestr(xml_filename)

				while count == 0 or count < number:

					try: 

						count = count + 1

						if specnum!= 0 and count > specnum:
							break

						if count > maxcount:
							break

						# total count of test cases
						total_count = total_count + 1

						# save the particular nested unit
						if number == 0:
							unitxml = filexml
						else:
							unitxml = extract_unit(filexml, count)

						# convert the unit in xml to text
						unittext = srcml2src(unitxml, encoding)

						# convert the unit in xml to text (if needed)
                                                if doseol:
                                                        unittext = unix2dos(unittext)

						# convert the text to srcML
						unitsrcmlraw = src2srcML(unittext, encoding, language, directory, getfilename(unitxml), defaultxmlns(getfullxmlns(unitxml)))

						# additional, later stage processing
						unitsrcml = unitsrcmlraw # srcML2srcMLStages(unitsrcmlraw, nondefaultxmlns(getfullxmlns(unitxml)))
						
						# find the difference
						result = xmldiff(unitxml, unitsrcml)
						if count == MAX_COUNT:
							print "\n", "".rjust(FIELD_WIDTH_LANGUAGE), " ", "...".ljust(FIELD_WIDTH_DIRECTORY), " ",
						if result != "":
							error_count += 1
							
							errorlist.append((directory + " " + language, count, result))

							# part of list of nested unit number in output
							print "\033[0;31m" + str(count) + "\033[0m",
						elif number != 0:
							# part of list of nested unit number in output
							print "\033[0;33m" + str(count) + "\033[0m",
	
					except OSError, (errornum, strerror):
						continue

			except OSError, (errornum, strerror):
				continue

	ki = False
except KeyboardInterrupt:
	ki = True
	
print
print
print "Report:"
if ki:
	print
	print "Testing stopped by keyboard"
	
# output error counts
# and delete error file
print

#for oldfile in os.listdir("."):
#	if re.match(error_filename + ".+", oldfile):
#		os.remove(oldfile)



f = open(error_filename, "w")

if error_count == 0:
	print "No errors out of " + str(total_count) + " cases" 
else:
	# break errorlist into two, one with original name, one with a name with a dot in it
	oerrorlist = []
	xerrorlist = []
	for e in errorlist:
		if str(e[0]).count(".") == 0:
			oerrorlist.append(e);
		else:
			xerrorlist.append(e);

	print "Errors:  " + str(error_count) + " out of " + str(total_count),
	if str(total_count) == "1":
		print " case", 
	else:
		print " cases", 
	print "\n"
	print "Errorlist:"
	nxerrorlist = xerrorlist[:]
	for e in oerrorlist:
		othererror = ""
		for x in xerrorlist[:]:
			if str(e[0]).split(' ')[1] != str(x[0]).split(' ')[1]:
				continue

			if str(e[1]) != str(x[1]):
				continue

			if str(x[0]).split('.')[0] == str(e[0]).split(' ')[0]:
				othererror = othererror + " " + str(x[0]).split(' ')[0].split('.')[1]

			xerrorlist.remove(x)

		print e[0], e[1], othererror, "\n", "".join(e[2][3:])

	for e in xerrorlist:
		print e[0], e[1], "\n", "".join(e[2][3:])

# output tool errors counts
print
if len(sperrorlist) == 0:
	print "No tool errors"
else:
	print "Tool errors:  " + str(len(sperrorlist))
	print "Tool Errorlist:"
	for e in sperrorlist:
		f.write(str(e[0]) + " " + str(e[1]) + " " + str(e[2]) + " " + str(e[3]) + "\n")
		print e[0], e[1], e[2], e[3]

current_time = datetime.now()
os.rename(error_filename, error_filename + "_" + current_time.isoformat().replace(":", "-") + error_filename_extension)
f.close()


# output tool version
print
print src2srcmlversion(), srcmltranslator
print srcml2srcversion(), srcmlutility

exit
