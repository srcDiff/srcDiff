#!/bin/bash

run_test "Version flag produces expected output text" '{
    version=$(srcdiff -V)
    assert_contains "$version" "srcdiff"
    echo "$version"  # for the log
}'
