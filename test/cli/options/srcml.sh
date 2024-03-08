#!/bin/bash

archive_flags=("-n" "--archive")

for flag in "${archive_flags[@]}"; do

    run_test "$flag causes an archive to be output" '{
        actual_diff=$(srcdiff $flag $original $modified)
        assert_equal "$actual_diff" "$expected_diff_archive"
    }'

done

# TODO: src-encoding, xml-encoding

language_flags=("-l" "--language")

for flag in "${language_flags[@]}"; do
    original_no_ext=$(mktemp)
    echo -e "int i;\n" > $original_no_ext
    modified_no_ext=$(mktemp)
    echo -e "int j;\n" > $modified_no_ext

    run_test "$flag sets the source language" '{
        actual_diff=$(srcdiff $flag C++ $original_no_ext $modified_no_ext)
        assert_contains "$actual_diff" "language=\"C++\""
    }'

    rm $original_no_ext
    rm $modified_no_ext

done

filename_flags=("-f" "--filename")

for flag in "${filename_flags[@]}"; do

    run_test "$flag sets the filename attribute" '{
        actual_diff=$(srcdiff $flag myfile.cpp $original $modified)
        assert_contains "$actual_diff" "filename=\"myfile.cpp\""
    }'

done

# TODO: register-ext once that is working (https://github.com/srcML/srcML/issues/1995)

run_test "--url sets the url attribute" '{
    actual_diff=$(srcdiff --url http://myurl.org/ -n $original $modified)
    assert_contains "$actual_diff" "url=\"http://myurl.org/\""
}'

version_flags=("-s" "--src-version")

for flag in "${version_flags[@]}"; do

    run_test "$flag sets the version attribute" '{
        actual_diff=$(srcdiff $flag 2.1 -n $original $modified)
        assert_contains "$actual_diff" "version=\"2.1\""
    }'

done

run_test "--xmlns adds a namespace attribute" '{
    actual_diff=$(srcdiff --xmlns:myprefix=http://myuri.org $original $modified)
    assert_contains "$actual_diff" "xmlns:myprefix=\"http://myuri.org\""
}'

# TODO: test setting default namespace once that works (https://github.com/srcML/srcML/issues/1993)

run_test "--position adds position attributes" '{
    actual_diff=$(srcdiff --position $original $modified)
    assert_contains "$actual_diff" "start="
    assert_contains "$actual_diff" "end="
}'

original_with_tab=$(mktemp)
echo -e "\tint i = 0;" > $original_with_tab
modified_with_tab=$(mktemp)
echo -e "\tint j = 0;" > $modified_with_tab

run_test "--tabs defaults to 8" '{
    actual_diff=$(srcdiff --position $original_with_tab $modified_with_tab)
    assert_contains "$actual_diff" "pos:tabs=\"8\""
    assert_contains "$actual_diff" "start=\"1:9\""
}'

run_test "--tabs can be set to 6" '{
    actual_diff=$(srcdiff --position --tabs 6 $original_with_tab $modified_with_tab)
    assert_contains "$actual_diff" "pos:tabs=\"6\""
    assert_contains "$actual_diff" "start=\"1:7\""
}'

rm $original_with_tab
rm $modified_with_tab

run_test "--no-xml-decl removes the XML declaration" '{
    actual_diff=$(srcdiff --no-xml-decl $original $modified)
    assert_not_contains "$actual_diff" "<?xml"
}'
