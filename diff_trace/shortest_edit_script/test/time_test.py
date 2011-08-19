#!/usr/bin/python
#
# timetest.py
#
# Michael J. Decker
# mjd52@zips.uakron.edu

import subprocess
import time

test_count = 0

test_file_list = "test_file_list.txt"
output_test_data_file = "output_test_data.txt"

libdiff = "../shortest_edit_script_u"
libdiff_length = "../shortest_edit_script_length"
libmba = "../diff_mba_srcML"
diff = "diff"

def time_test(old_sequence, new_sequence) :

        # first one is a bit slower
        diff_time = time_process([diff, old_sequence, new_sequence])[1];

        libdiff_time = time_process([libdiff, old_sequence, new_sequence])[1];
        libmba_time = time_process([libmba, old_sequence, new_sequence])[1];
        diff_time = time_process([diff, old_sequence, new_sequence])[1];

        return libdiff_time, libmba_time, diff_time

def time_process(command) :

        start_time = time.time()
	p = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	last_line = p.communicate()[0]

        end_time = time.time()
        elapsed_time = end_time - start_time

        if p < 0 :
                print "ERROR: " + command

	return last_line, elapsed_time

def file_length(file_name) :

        file = open(file_name, "r")
        file_lines = file.read().splitlines()
        file.close()

        file_length = len(file_lines)

        return file_length

def run_all_time_tests() :

        # get the list of test files
        test_list = open(test_file_list, "r")
        test_files = test_list.read().splitlines()
        test_list.close()

        output_file = open(output_test_data_file, "w")

        # output column labels
        output_file.write("System\tOld File Length\tOld File\tNew File Length\tNew File\tNumber of Differences\tLibdiff Time\tLibmba Time\tDiff Time\n")

        # run all the tests
        for line in test_files :

                # get both files
                test_line = line.split("\t")
                system = test_line[0]
                old_file = "test_files/" + test_line[1]
                new_file = "test_files/" + test_line[2]

                # get the length of the files
                old_file_length = file_length(old_file)
                new_file_length = file_length(new_file)

                # run algorithms
                times = time_test(old_file, new_file)

                # get the number of differences
                number_of_differences = time_process([libdiff_length, old_file, new_file])[0]

                output_file.write(system + "\t" + str(old_file_length) + "\t" + old_file + "\t"
                                  + str(new_file_length) + "\t" + new_file + "\t"
                                  + number_of_differences
                                  + "\t" + str(times[0]) + "\t" + str(times[1]) + "\t" + str(times[2]) + "\n")

        output_file.close()

run_all_time_tests()
