#!/bin/bash

flags=("-V" "--version")

for flag in "${flags[@]}"; do

    run_test "$flag flag produces version output containing the word \"srcdiff\"" '{
        version=$(srcdiff $flag)
        assert_contains "$version" "srcdiff"
    }'

done
