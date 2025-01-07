#!/bin/bash

from_file_test() {
    local from_file=$(localtemp "from_file_single")
    echo "$original|$modified" > $from_file
    local actual_diff="$(srcdiff -q --files-from $from_file)"
    assert_equal "$actual_diff" "$expected_diff"
    rm $from_file
}

multiple_from_file_test() {
    local from_file=$(localtemp "from_file_multiple")
    echo -e "$original|$modified\n$original|$modified" > $from_file
    local actual_diff="$(srcdiff -q --files-from $from_file)"
    assert_equal "$actual_diff" "$expected_diff_twice"
    rm $from_file
}

run_test "--files-from lets input files be specified by a file" from_file_test

run_test "--files-from lets multiple input file pairs be specified by a file" multiple_from_file_test
