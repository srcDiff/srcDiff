#!/usr/bin/env python3
#
# test_suite.py
#
# Original by Michael L. Collard
# Updated/Modified for srcdiff by Michael John Decker

import json
import sys
import os.path
import re
import subprocess
import difflib
import argparse
from datetime import datetime, time

error_filename = "test_reports/srcDiffTestReport"
error_filename_extension = ".txt"

try:
    SHELL_ROWS, SHELL_COLUMNS = subprocess.check_output(['stty', 'size']).split()
except (subprocess.CalledProcessError, FileNotFoundError):
    SHELL_ROWS, SHELL_COLUMNS = 15, 100

FIELD_WIDTH_LANGUAGE   = 12
FIELD_WIDTH_URL        = 20
FIELD_WIDTH_TEST_CASES = int(SHELL_COLUMNS) - (FIELD_WIDTH_LANGUAGE + 1) - (FIELD_WIDTH_URL + 1)
sperrorlist = []

if os.path.exists("../build/bin/Debug"):
    executable_path = "../build/bin/Debug"
elif os.path.exists("../build/bin"):
    executable_path = "../build/bin"
elif os.path.exists("../bin"):
    executable_path = "../bin"
else:
    raise "Could not find directory for compiled executables!"

switch_utility = f"{executable_path}/switch_differences"
archive_reader = f"{executable_path}/archive_reader"
srcdiff_utility = f"{executable_path}/srcdiff"

def safe_communicate(command, input):
    """
    Runs `command` and then sends `input` to its stdin, catching errors and 
    returning its stdout
    """
    try :
        return subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate(input.encode())[0].decode('utf-8')
    except OSError as e:
        errornum, strerror = e.args
        try :
            return subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate(input)[0].decode('utf-8')
        except OSError as e:
            errornum, strerror = e.args
            sperrorlist.append((command, input, errornum, strerror))
            raise
            

def safe_communicate_file(command, filename):
    """
    runs `command` with `filename` appended to it, sends nothing to stdin, returns
    its stdout
    """

    newcommand = command[:]
    newcommand.append(filename)
    try:
        return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0].decode('utf-8')
    except OSError as e:
        errornum, strerror = e.args
        try:
            return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0].decode('utf-8')
        except OSError as e:
            errornum, strerror = e.args
            sperrorlist.append((command, filename, errornum, strerror))
            raise


def safe_communicate_two_files(command, filename_one, filename_two, url):
    """
    runs `command` with two filenames appended to it, sends nothing to stdin,
    returns its stdout. `url` is just logged if there is an error
    """

    newcommand = command[:]
    newcommand.append(filename_one)
    newcommand.append(filename_two)

    try:
        return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0].decode('utf-8')
    except OSError as e:
        errornum, strerror = e.args
        try:
            return subprocess.Popen(newcommand, stdout=subprocess.PIPE, stdin=subprocess.PIPE).communicate()[0].decode('utf-8')
        except OSError as e:
            errornum, strerror = e.args
            sperrorlist.append((command, url, errornum, strerror))
            raise

def extract_unit(file, count):
    """
    extracts a particular unit, specified by `count`, from a srcML archive.
    units are numbered starting from one
    """

    command = [archive_reader, "--unit=" + str(count)]

    return safe_communicate_file(command, file)

def convert_filename_to_string(src_filename):
    "returns the contents of the file at `src_filename` as a string"
    with open(src_filename) as file:
        return file.read()

def extract_source(file, unit, revision):
    "gets the source for a specific revision of a unit that srcDiff outputted"

    return safe_communicate_file([
        archive_reader, "--revision=" + str(revision), "--unit=" + str(unit), "--output-src"
    ], file)

def switch_differences(srcML):
    "switch diff order"

    return safe_communicate([switch_utility], srcML)

def convert_unix_to_dos(srctext):
    "converts from unix to dos line endings - unix2dos must be installed (?)"

    # run the srcml processor
    command = ['unix2dos']

    return safe_communicate(command, srctext)

def linediff(xml_filename1, xml_filename2):
    """
    compute a simple line-based diff of two files - this is used to show where
    srcDiff's output differed from the expected output when tested
    """

    if xml_filename1 != xml_filename2:
        return list(difflib.unified_diff(xml_filename1.splitlines(1), xml_filename2.splitlines(1)))
    else:
        return ""

def srcdiff(source_file_version_one, source_file_version_two, language, filename):
    """
    saves the two source code strings in the specified `language` to files and
    then runs srcdiff on them, setting the filename attribute in the result to
    `filename`, and returning srcDiff's output
    """
    command = [srcdiff_utility, "-f", filename]

    extension = "cpp"
    if language == "Java":
        extension = "java"

    temp_file = open("temp_file_one." + extension, "w")
    temp_file.write(source_file_version_one)
    temp_file.close()

    temp_file = open("temp_file_two." + extension, "w")
    temp_file.write(source_file_version_two)
    temp_file.close()

    return safe_communicate_two_files(command, "temp_file_one." + extension, "temp_file_two." + extension, url)

def find_difference(unitxml, unitsrcml, test_number, character_count):
    """
    takes `unitxml` and `unitsrcml`, comparing them with the linediff function 
    then returning the result. `test_number` and `character_count` are to aid output.
    returns `result` and `character_count` for use in the main loop.
    """
    result = linediff(unitxml, unitsrcml)
    character_count += get_character_count(test_number)
    if character_count > FIELD_WIDTH_TEST_CASES:
        print("\n", "".rjust(FIELD_WIDTH_LANGUAGE), "...".ljust(FIELD_WIDTH_URL))
        character_count = get_character_count(test_number)
    return (result, character_count)

def update_error_list(result, error_count, errorlist, url, language, num, num_nested_units):
    """
    takes `result`, `url`, `language`, `num`, and appends a tuple to the errorlist with
    each item, then incrementing the error count. returns `error_count` for use in the
    main loop.
    """
    if result != "":
        error_count += 1            
        errorlist.append((url + " " + language, num, result))

        # part of list of nested unit number in output
        print(f"\033[0;31m{str(num)}\033[0m", end=' ')
    elif num_nested_units != 0:

        # part of list of nested unit number in output
        print(f"\033[0;33m{str(num)}\033[0m", end=' ')
    return error_count

def convert_to_srcdiff(unit_text_version_one, unit_text_version_two, lang, filename):
    """
    takes `unit_text_version_one`, `unit_text_version_two`, `lang`, and `filename` and
    sends them to the `srcdiff` function. returns unitsrcml for use in the main loop.
    """
    # convert text to srcML
    unitsrcmlraw = srcdiff(
        unit_text_version_one,
        unit_text_version_two,
        lang,
        filename
    )
    
    # additional, later stage processing (?)
    unitsrcml = unitsrcmlraw

    return unitsrcml

def get_character_count(count):
    
    return len(str(count)) + 1

def test_cleanup():
    """
    removes any temporary files generated by the program.
    """
    extensions = [".cpp", ".java"]
    for ext in extensions:
        if os.path.exists(f"temp_file_one.{ext}"):
            os.remove(f"temp_file_one.java.{ext}")
        if os.path.exists(f"temp_file_two.{ext}"):
            os.remove(f"temp_file_two.{ext}")

class Tee:
    """
    Overrides `sys.stdout` so that standard output is written to `self.file` in
    addition to working normally.
    """
    def __init__(self, name):
        self.file = open(name, "w")
        self.stdout = sys.stdout
        sys.stdout = self
    
    def close(self):
        sys.stdout = self.stdout
        self.file.close()

    def __del__(self):
        self.close()

    def write(self, data):
        self.file.write(data)
        self.stdout.write(data)
    
    def flush(self):
        self.file.flush()
        self.stdout.flush()


current_time = datetime.now()
error_filename = error_filename + "_" + current_time.isoformat().replace(":", "-") + error_filename_extension
tee = Tee(error_filename)

print("Testing:")
print()

# Handle optional dos line endings
doseol = False
if len(sys.argv) > 1 and sys.argv[1] == "--dos":
        sys.argv.pop(0)
        doseol = True


# if the user specifies a test name (the url attributes on archives are
# considered to be the test names) and/or test number and/or test language,
# parse those from the command line so that only the corresponding tests will
# be run
specname = ""
if len(sys.argv) > 1:
    specname = sys.argv[1]

if specname != "":
    print(specname)

specnum = 0
speclang = ""
if len(sys.argv) == 3:
    result = sys.argv[2]
    if result == "C++" or result == "C" or result == "C#" or result == "Objective-C" or result == "Java":
        speclang = result
    else:
        specnum = int(sys.argv[2])
elif len(sys.argv) == 2:
    result = sys.argv[1]
    if result == "C++" or result == "C" or result == "C#" or result == "Objective-C" or result == "Java":
        speclang = result
        specname = ""
    else:
        specname = result
elif len(sys.argv) > 2:
    specnum = int(sys.argv[2])

    if len(sys.argv) > 3:
        speclang = sys.argv[3]

errorlist = []

m = re.compile(specname + "$")

# source directory
source_dir = "suite"

# total number of errors
error_count = 0

# total test cases
total_count = 0

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
                archive_info = json.loads(safe_communicate_file([archive_reader, "--info"], xml_filename))
                if archive_info == None:
                    print("Problem with", xml_filename)
                    continue

                url = archive_info["url"]

                # only process if url name matches or is not given
                if specname != "" and m.match(url) == None:
                    continue

                # number of nested units
                num_nested_units = int(archive_info["units"])
                
                if specnum == 0:
                    count = 0
                else:
                    count = specnum - 1
                    
                get_language = True

                character_count = 0

                while count == 0 or count < num_nested_units:

                    try: 

                        count = count + 1

                        if specnum != 0 and count > specnum:
                            break

                        # save the particular nested unit
                        if num_nested_units == 0:
                            unitxml = convert_filename_to_string(xml_filename)
                        else:
                            unitxml = extract_unit(xml_filename, count)

                        if get_language:

                            get_language = False

                            # language of the entire document with a default of C++
                            language = archive_info.get("language", "")
                            if len(language) == 0:
                                language = "C++"

                            # only process if language matches or is not given
                            if speclang != "" and language != speclang:
                                continue
                        
                            # output language and url
                            print("\n", language.ljust(FIELD_WIDTH_LANGUAGE), url.ljust(FIELD_WIDTH_URL), end=' ')

                        # total count of test cases
                        total_count = total_count + 1

                        # convert the unit in xml to text
                        unit_text_version_one = extract_source(xml_filename, count, 0)
                        unit_text_version_two = extract_source(xml_filename, count, 1)

                        # convert the unit in xml to text (if needed)
                        if doseol:
                                unittext = convert_unix_to_dos(unittext)
                        
                        unit_info = json.loads(
                            safe_communicate_file(
                                [archive_reader, "--info", f"--unit={count}"],
                                xml_filename
                            )
                        )

                        # convert the text to srcML
                        unitsrcml = convert_to_srcdiff(unit_text_version_one, unit_text_version_two, language, unit_info["filename"])

                        test_number = count * 2 - 1
                        if url == "interchange":
                            test_number = count
                        
                        # find the difference
                        result, character_count = find_difference(unitxml, unitsrcml, test_number, character_count)

                        error_count = update_error_list(result, error_count, errorlist, url, language, test_number, num_nested_units)

                        # special case: interchange tests are not swappable
                        if url == "interchange":
                            continue

                        # total count of test cases
                        total_count += 1

                        # convert the unit in xml to text
                        unitxml = switch_differences(unitxml)
                        temp_xml_path = "temp.xml"
                        with open(temp_xml_path, "w+", encoding="utf-8") as temp_xml:
                            temp_xml.write(unitxml)

                        temp_source = unit_text_version_one
                        unit_text_version_one = unit_text_version_two
                        unit_text_version_two = temp_source

                        os.remove(temp_xml_path)

                        # convert the unit in xml to text (if needed)
                        if doseol:
                            unittext = convert_unix_to_dos(unittext)

                        # convert text to srcML
                        unitsrcml = convert_to_srcdiff(unit_text_version_one, unit_text_version_two, language, unit_info["filename"])
                        
                        test_number += 1

                        # find the difference
                        result, character_count = find_difference(unitxml, unitsrcml, test_number, character_count)

                        error_count = update_error_list(result, error_count, errorlist, url, language, count * 2, num_nested_units)
    
                    except OSError as e:
                        errornum, strerror = e.args
                        continue

            except OSError as e:
                errornum, strerror = e.args
                continue

    ki = False
except KeyboardInterrupt:
    ki = True
    
print("\n\nReport:\n")
if ki:
    print("\nTesting stopped by keyboard")
    
# output error counts
# and delete error file

print()

if error_count == 0:
    print("No errors out of " + str(total_count) + " cases")
else:

    # break errorlist into two, one with original name, one with a name with a dot in it
    oerrorlist = []
    xerrorlist = []
    for e in errorlist:
        if str(e[0]).count(".") == 0:
            oerrorlist.append(e)
        else:
            xerrorlist.append(e)

    print("Errors:  " + str(error_count) + " out of " + str(total_count) + (" case" if str(total_count) == "1" else " cases"))
    print("\n")
    print("Errorlist:")
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

        print(e[0], e[1], othererror, "\n", "".join(e[2][3:]))

    for e in xerrorlist:
        print(e[0], e[1], "\n", "".join(e[2][3:]))

# output tool errors counts
print()
if len(sperrorlist) == 0:
    print("No tool errors")
else:
    print("Tool errors:  " + str(len(sperrorlist)))
    print("Tool Errorlist:")
    for e in sperrorlist:
        print(e[0], e[1], e[2], e[3])

tee.close()
test_cleanup()