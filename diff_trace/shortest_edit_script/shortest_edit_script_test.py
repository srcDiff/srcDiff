#!/usr/bin/python
#
# shortest_edit_script_test.py
#
# Test random outputs comparing to diff
#
# Michael J. Decker
# mjd52@zips.uakron.edu

import os
import random

test_file_one = "shortest_edit_script_test_file_one"
test_file_two = "shortest_edit_script_test_file_two"
output_file_fcomp = "shortest_edit_script_fcomp"
output_file_libdiff = "shortest_edit_script_libdiff"

error_file_stem_one = "shortest_edit_script_test_error_one"
error_file_stem_two = "shortest_edit_script_test_error_two"

fcomp = "./fcomp"
libdiff = "./shortest_edit_script_u"

def make_random_file(filename) :

    file = open(filename, "w")

    for i in range(1000) :
        line = str(random.randint(0, 10)) + "\n"
        file.write(line)

    file.close()
        
    return


count_errors = 0
for i in range(100) :

    print "Random Test: " + str(i + 1)

    # make files
    make_random_file(test_file_one)
    make_random_file(test_file_two)

    # run algorithms
    os.system(fcomp + " " + test_file_one + " " + test_file_two + " > " + output_file_fcomp )
    os.system(libdiff + " " + test_file_one + " " + test_file_two + " > " + output_file_libdiff )

    # compare results
    file = open(output_file_fcomp, "r")
    output_fcomp = file.read()
    file.close()

    file = open(output_file_libdiff, "r")
    output_libdiff = file.read()
    file.close()

    if output_fcomp != output_libdiff :
        count_errors += 1
        os.system("cp " + test_file_one + " " + error_file_stem_one + str(count_errors))
        os.system("cp " + test_file_two + " " + error_file_stem_two + str(count_errors))


print
print "Error Count: " + str(count_errors)

os.system("rm -f " + test_file_one + " " + test_file_two + " " + output_file_fcomp + " " + output_file_libdiff)
