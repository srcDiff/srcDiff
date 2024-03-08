#!/bin/bash

# srcDiff requires an even number of input files (i.e. a whole number of pairs)

expect_failure "srcDiff fails when given no arguments" 'srcdiff'

expect_failure "srcDiff fails when given no input arguments" 'srcdiff -n'

expect_failure "srcDiff fails when given one input file" '{
    srcdiff one.cpp
}'

expect_failure "srcDiff fails when given three input files" '{
    srcdiff one.cpp two.cpp three.cpp
}'

run_test "srcDiff can diff a pair of input files" '{
    actual_diff=$(srcdiff $original $modified)
    assert_equal "$actual_diff" "$expected_diff"
}'

run_test "srcDiff can diff two pairs of input files" '{
    actual_diff_twice=$(srcdiff $original $modified $original $modified)
    assert_equal "$actual_diff_twice" "$expected_diff_twice"
}'
