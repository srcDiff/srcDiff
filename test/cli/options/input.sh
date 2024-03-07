#!/bin/bash

expect_failure "srcDiff fails when given no arguments" 'srcdiff'

expect_failure "srcDiff fails when given no input arguments" 'srcdiff -n'

expect_failure "srcDiff fails when given one input file" '{
    srcdiff one.cpp
}'

expect_failure "srcDiff fails when given three input files" '{
    srcdiff one.cpp two.cpp three.cpp
}'
