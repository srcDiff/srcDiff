#!/usr/bin/env python3
#
# test_suite.py
#
# Original by Michael L. Collard
# Updated/Modified for srcdiff by Michael John Decker

## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

import json
import sys
import os.path
import re
import subprocess
import difflib
import argparse
from datetime import datetime, time

try:
    SHELL_ROWS, SHELL_COLUMNS = subprocess.check_output(['stty', 'size']).split()
except (subprocess.CalledProcessError, FileNotFoundError):
    SHELL_ROWS, SHELL_COLUMNS = 15, 100

FIELD_WIDTH_LANGUAGE   = 12
FIELD_WIDTH_URL        = 20
FIELD_WIDTH_TEST_CASES = int(SHELL_COLUMNS) - (FIELD_WIDTH_LANGUAGE + 1) - (FIELD_WIDTH_URL + 1)
sperrorlist = []

if os.path.exists("../../bin"):
    executable_path = "../../bin"
else:
    raise Exception("Could not find directory for compiled executables! Are you running from the build directory?")

switch_utility = f"{executable_path}/switch_differences"
archive_reader = f"{executable_path}/archive_reader"
srcdiff_utility = f"{executable_path}/srcdiff"


def main():
    error_filename = "reports/srcDiffTestReport"
    error_filename_extension = ".txt"
    current_time = datetime.now()
    error_filename = error_filename + "_" + current_time.isoformat().replace(":", "-") + error_filename_extension
    tee = Tee(error_filename)
    parser = build_parser()
    args = arg_catcher()
    source_dir = "suite"

    # necessary variables for argument holding
    error_list = []
    speclang_list = []
    specname_list = []
    m_list = []
    total_count = 0
    error_count = 0
    # parse arguments
    parser.parse_args(namespace=args)
    if args.lang:
        speclang_list = args.lang[:]
    if args.spec:
        specname_list = args.spec[:]
        for item in args.spec:
            m_list.append(re.compile(item + "$"))
    specnum = args.num
    doseol = args.dos
    print("Testing:")
    print()
    try:
        error_list, total_count, error_count = run_tests(source_dir, speclang_list, specname_list, specnum, m_list, error_list, doseol)
        ki = False
    except KeyboardInterrupt:
        ki = True
    
    if ki:
        print("\nTesting stopped by keyboard")
    
    print_report(error_list, total_count, error_count)

    tee.close()
    test_cleanup()

def safe_communicate(command, input=None):
    """
    Runs `command`, sending `input` to its stdin. Returns a CompletedProcess instance
    which is then accessed, or raises an exception if there was an error with the process.
    """

    try:
        return subprocess.run(command, input=input, encoding="utf-8", cwd=os.getcwd(), capture_output=True, check=True).stdout
    except subprocess.CalledProcessError as e:
            error_num, str_error = e.args
            sperrorlist.append((command, input, error_num, str_error))
            

def safe_communicate_files(command, filenames):
    """
    Runs `cmd` after appending `filename` to `command`. Returns the stdout of the process
    in string form, or raises a subprocess error.
    """

    cmd = command[:]
    for name in filenames:
        cmd.append(name)
    try:
        return safe_communicate(cmd)
    except subprocess.CalledProcessError as e:
        error_num, str_error = e.args
        sperrorlist.append((command, input, error_num, str_error))

def extract_unit(file, count):
    """
    Extracts a particular unit, specified by `count`, from a srcML archive.
    NOTE: Units are numbered starting from one.
    """

    command = [archive_reader, "--unit=" + str(count)]

    return safe_communicate_files(command, [file])

def convert_filename_to_string(src_filename):
    """
    Returns the contents of `src_filename` as a string.
    """
    with open(src_filename) as file:
        return file.read()

def extract_source(file, unit, revision):
    """
    Gets the source for a specific revision of a unit that srcDiff outputted
    """

    return safe_communicate_files([
        archive_reader, "--revision=" + str(revision), "--unit=" + str(unit), "--output-src"
    ], [file])

def switch_differences(srcML):
    """
    Switches diff order.
    """

    return safe_communicate(switch_utility, srcML)

def convert_unix_to_dos(srctext):
    """
    Converts from UNIX to DOS line endings - unix2dos must be installed
    """

    # run the unix2dos processor

    return safe_communicate(['unix2dos'], srctext)

def color_diff(xml):
    """
    Get colorized source-code diff
    """

    temp_file = open('srcdiff.xml', 'w')
    temp_file.write(xml)
    temp_file.close()

    colorized_diff = safe_communicate([srcdiff_utility, 'srcdiff.xml', '-u'])

    os.remove('srcdiff.xml')

    return colorized_diff

def line_diff(xml_filename1, xml_filename2):
    """
    Computes a simple line-based diff of two files - this is used to show where
    srcDiff's output differed from the expected output when tested
    """

    if xml_filename1 != xml_filename2:
        return list(difflib.unified_diff(xml_filename1.splitlines(1), xml_filename2.splitlines(1)))
    else:
        return ""

def convert_to_srcdiff(source_file_version_one, source_file_version_two, language, filename, url, doseol):
    """
    Saves the two source code strings in the specified `language` to files and
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

    # handle dos line endings
    if doseol:
        convert_unix_to_dos(f"temp_file_one.{extension}")
        convert_unix_to_dos(f"temp_file_two.{extension}")
    
    return safe_communicate_files(command, [f"temp_file_one.{extension}", f"temp_file_two.{extension}"])

def find_difference(unitxml, unitsrcml, test_number, character_count):
    """
    Takes `unitxml` and `unitsrcml`, comparing them with the line_diff function 
    then returning the result. `test_number` and `character_count` are to aid output.
    returns `result` and `character_count` for use in the main loop.
    """
    result = line_diff(unitxml, unitsrcml)
    character_count += get_character_count(test_number)
    color_result = None
    if character_count > FIELD_WIDTH_TEST_CASES:
        print("\n", "".rjust(FIELD_WIDTH_LANGUAGE), "...".ljust(FIELD_WIDTH_URL), end=' ')
        character_count = get_character_count(test_number)

    if result != "":
        actual   = color_diff(unitxml)
        expected = color_diff(unitsrcml)
        color_result = actual + '\n----------\n' + expected
    
    return (result, character_count, color_result)

def update_error_list(result, color_result, error_count, error_list, url, language, num, num_nested_units):
    """
    Takes `result`, `url`, `language`, `num`, and appends a tuple to the error_list with
    each item, then incrementing the error count. returns `error_count` for use in the
    main loop.
    """

    if result != "":
        error_count += 1            
        error_list.append((url + " " + language, num, color_result, result))

        # part of list of nested unit number in output
        print(f"\033[0;31m{str(num)}\033[0m", end=' ')
    elif num_nested_units != 0:

        # part of list of nested unit number in output
        print(f"\033[0;33m{str(num)}\033[0m", end=' ')
    
    return error_count

def build_parser():
    parser = argparse.ArgumentParser(
        prog='srcDiff Testing Utility',
        description='Runs the test suite for srcDiff',
        epilog="Thanks!"
    )
    parser.add_argument('-l', '--lang', type=str, nargs="*", required=False,
                        help="specify the languages to test")
    parser.add_argument('-s', '--spec', type=str, nargs="*", required=False,
                        help="specify a list of tests")
    parser.add_argument('-n', '--num', type=int, required=False, default=0,
                        help="specify the test number to run")
    parser.add_argument('-d', '--dos', required=False, action="store_true",
                        help="optionally convert UNIX line endings to DOS line endings")
    return parser

def get_character_count(count):
    
    return len(str(count)) + 1

def run_tests(source_dir, speclang_list, specname_list, specnum: int, m_list, error_list, doseol):

    total_count = 0
    error_count = 0

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
                archive_info = json.loads(safe_communicate_files([archive_reader, "--info"], [xml_filename]))
                if archive_info == None:
                    print("Problem with", xml_filename)
                    continue

                url = archive_info["url"]
                # only process if url name matches or is not given
                if url not in specname_list and True not in [item.match(url) for item in m_list] and specname_list != []:
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
                            if language not in speclang_list and speclang_list != []:
                                continue
                        
                            # output language and url
                            print("\n", language.ljust(FIELD_WIDTH_LANGUAGE), url.ljust(FIELD_WIDTH_URL), end=' ')

                        # total count of test cases
                        total_count = total_count + 1

                        # convert the unit in xml to text
                        unit_text_version_one = extract_source(xml_filename, count, 0)
                        unit_text_version_two = extract_source(xml_filename, count, 1)
                        
                        unit_info = json.loads(
                            safe_communicate_files(
                                [archive_reader, "--info", f"--unit={count}"],
                                [xml_filename]
                            )
                        )

                        # convert the text to srcML
                        unitsrcml = convert_to_srcdiff(unit_text_version_one, unit_text_version_two, language, unit_info["filename"], url, doseol)

                        test_number = count * 2 - 1

                        # special case: interchange tests are not swappable
                        if url == "interchange":
                            test_number = count
                        
                        # find the difference
                        result, character_count, color_result = find_difference(unitxml, unitsrcml, test_number, character_count)

                        error_count = update_error_list(result, color_result, error_count, error_list, url, language, test_number, num_nested_units)

                        # special case: interchange tests are not swappable
                        if url == "interchange" or url == "super_list":
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

                        # convert text to srcML
                        unitsrcml = convert_to_srcdiff(unit_text_version_one, unit_text_version_two, language, unit_info["filename"], url, doseol)
                        
                        test_number += 1

                        # find the difference
                        result, character_count, color_result = find_difference(unitxml, unitsrcml, test_number, character_count)

                        error_count = update_error_list(result, color_result, error_count, error_list, url, language, count * 2, num_nested_units)
    
                    except subprocess.CalledProcessError as e:
                        error_num, str_error = e.args
                        continue

            except subprocess.CalledProcessError as e:
                error_num, str_error = e.args
                continue
    return (error_list, total_count, error_count)

def test_cleanup():
    """
    removes any temporary files generated by the program.
    """
    extensions = ["cpp", "java"]
    for ext in extensions:
        if os.path.exists(f"temp_file_one.{ext}"):
            os.remove(f"temp_file_one.{ext}")
        if os.path.exists(f"temp_file_two.{ext}"):
            os.remove(f"temp_file_two.{ext}")

def print_report(error_list, total_count, error_count):
    print("\n\nReport:\n")
    print()
    if error_count == 0:
        print(f"No errors out of {str(total_count)} cases")
    else:

        # break error_list into two, one with original name, one with a name with a dot in it
        oerrorlist = []
        xerrorlist = []
        for e in error_list:
            if str(e[0]).count(".") == 0:
                oerrorlist.append(e)
            else:
                xerrorlist.append(e)

        print(f"Errors: {str(error_count)} out of {str(total_count)}" + (" case" if str(total_count) == "1" else " cases"))
        print("\n")
        print("error_list:")
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

            print(e[0], e[1], othererror, "\n", e[2], "".join(e[3][3:]))

        for e in xerrorlist:
            print(e[0], e[1], "\n", e[2], "".join(e[3][3:]))

    # output tool errors counts
    print()
    if len(sperrorlist) == 0:
        print("No tool errors")
    else:
        print("Tool errors:  " + str(len(sperrorlist)))
        print("Tool error_list:")
        for e in sperrorlist:
            print(e[0], e[1], e[2], e[3])

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
class arg_catcher():
    # A very simple helper class; used in main to enable easy access of processed CLI
    pass

if __name__ == "__main__":
    main()