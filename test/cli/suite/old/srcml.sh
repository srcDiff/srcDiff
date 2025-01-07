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
    original_no_ext=$(localtemp "original_no_ext")
    echo -e "int i;\n" > $original_no_ext
    modified_no_ext=$(localtemp "modified_no_ext")
    echo -e "int j;\n" > $modified_no_ext

    run_test "$flag sets the source language" '{
        actual_diff=$(srcdiff $flag C# $original_no_ext $modified_no_ext)
        assert_contains "$actual_diff" "language=\"C#\""
    }'

    rm $original_no_ext
    rm $modified_no_ext

done

orig_java=$(localtemp "orig.java")
echo "public class Man {}" > $orig_java
mod_java=$(localtemp "mod.java")
echo "public class Main {}" > $mod_java

run_test "language is detected from file extension by default" '{
    actual_diff=$(srcdiff $orig_java $mod_java)
    assert_contains "$actual_diff" "language=\"Java\""
}'

rm $orig_java
rm $mod_java

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

original_with_tab=$(localtemp "original_with_tab.cpp")
echo -e "\tint i = 0;" > $original_with_tab
modified_with_tab=$(localtemp "modified_with_tab.cpp")
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

if0_code=$(cat <<-END
#if 0
std::cout << "hello world";
#endif
END
)

preprocessor_if0=$(localtemp "if0.cpp")
echo "$if0_code" > $preprocessor_if0

if0_not_marked_up=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$preprocessor_if0"><cpp:if>#<cpp:directive>if</cpp:directive> <expr><literal type="number">0</literal></expr></cpp:if>
std::cout &lt;&lt; "hello world";
<cpp:endif>#<cpp:directive>endif</cpp:directive></cpp:endif>
</unit>
END
)

if0_marked_up=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$preprocessor_if0" options="CPP_MARKUP_IF0"><cpp:if>#<cpp:directive>if</cpp:directive> <expr><literal type="number">0</literal></expr></cpp:if>
<expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"hello world"</literal></expr>;</expr_stmt>
<cpp:endif>#<cpp:directive>endif</cpp:directive></cpp:endif>
</unit>
END
)

run_test "#if 0 sections are not marked up by default" '{
    diff=$(srcdiff $preprocessor_if0 $preprocessor_if0)
    assert_equal "$diff" "$if0_not_marked_up"
}'

run_test "#if 0 sections are marked up if --cpp-markup-if0 is specified" '{
    diff=$(srcdiff --cpp-markup-if0 $preprocessor_if0 $preprocessor_if0)
    assert_equal "$diff" "$if0_marked_up"
}'


else_code=$(cat <<-END
#if A
std::cout << "hello world";
#else
std::cout << "goodbye";
#endif
END
)

preprocessor_else=$(localtemp "else.cpp")
echo "$else_code" > $preprocessor_else

else_marked_up=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$preprocessor_else"><cpp:if>#<cpp:directive>if</cpp:directive> <expr><name>A</name></expr></cpp:if>
<expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"hello world"</literal></expr>;</expr_stmt>
<cpp:else>#<cpp:directive>else</cpp:directive></cpp:else>
<expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"goodbye"</literal></expr>;</expr_stmt>
<cpp:endif>#<cpp:directive>endif</cpp:directive></cpp:endif>
</unit>
END
)

else_not_marked_up=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$preprocessor_else" options="CPP_TEXT_ELSE"><cpp:if>#<cpp:directive>if</cpp:directive> <expr><name>A</name></expr></cpp:if>
<expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"hello world"</literal></expr>;</expr_stmt>
<cpp:else>#<cpp:directive>else</cpp:directive></cpp:else>
std::cout &lt;&lt; "goodbye";
<cpp:endif>#<cpp:directive>endif</cpp:directive></cpp:endif>
</unit>
END
)

run_test "#else sections are marked up by default" '{
    diff=$(srcdiff $preprocessor_else $preprocessor_else)
    assert_equal "$diff" "$else_marked_up"
}'

run_test "#else sections are not marked up if --cpp-text-else is specified" '{
    diff=$(srcdiff --cpp-text-else $preprocessor_else $preprocessor_else)
    assert_equal "$diff" "$else_not_marked_up"
}'


rm $preprocessor_else
rm $preprocessor_if0
