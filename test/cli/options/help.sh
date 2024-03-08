#!/bin/bash

flags=("-h" "--help")

for flag in "${flags[@]}"; do

    run_test "$flag produces help text containing the word \"options\"" '{
        help=$(srcdiff $flag)
        assert_contains "$help" "options"
    }'

done
