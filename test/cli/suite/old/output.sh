#!/bin/bash

# checks to make sure that output goes to a temp file when requested
# $1 takes the form of the output argument to use (either -o or --output)
check_file_output() {
    temp_output_file=$(localtemp "output_file")

    std_output=$(srcdiff $original $modified $1 $temp_output_file)
    assert_equal "$std_output" ""

    file_output=$(<"$temp_output_file")
    assert_equal "$expected_diff" "$file_output"
    
    rm $temp_output_file
}

flags=("-o" "--output")

for flag in "${flags[@]}"; do

    run_test "$flag causes srcDiff to write to a file" check_file_output $flag

    run_test "$flag with - as the output location still writes to stdout" '{
        std_output=$(srcdiff $original $modified $flag -)
        assert_equal "$std_output" "$expected_diff"
    }'

done
