#!/bin/bash

run_test "-h produces expected help text" '{
    help=$(srcdiff -h)
    assert_contains "$help" "options"
    echo "$help"  # for the log
}'
