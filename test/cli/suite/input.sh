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

simple1="$(localtemp simple_folder_1)"
mkdir "$simple1"
echo "int i = 0;" > "$simple1/a.cpp"
echo "std::cout << 1;" > "$simple1/b.cpp"

simple2="$(localtemp simple_folder_2)"
mkdir "$simple2"
echo "int j = 0;" > "$simple2/a.cpp"
echo "std::cout << 2;" > "$simple2/b.cpp"

simple_output=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="a.cpp"><decl_stmt><decl><type><name>int</name></type> <name><diff:delete type="replace">i</diff:delete><diff:insert type="replace">j</diff:insert></name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="b.cpp"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="number"><diff:delete type="replace">1</diff:delete><diff:insert type="replace">2</diff:insert></literal></expr>;</expr_stmt>
</unit>
END
)

run_test "srcDiff can match files between folders" '{
    matched=$(srcdiff -q "$simple1" "$simple2")
    assert_equal "$matched" "$simple_output"
}'

mkdir "$simple1/deeper"
mv "$simple1/a.cpp" "$simple1/deeper/a.cpp"
mv "$simple1/b.cpp" "$simple1/deeper/b.cpp"

mkdir "$simple2/deeper"
mv "$simple2/a.cpp" "$simple2/deeper/a.cpp"
mv "$simple2/b.cpp" "$simple2/deeper/b.cpp"

deeper_output=$(echo "$simple_output" | sed 's/filename="/filename="deeper\//')

run_test "srcDiff can match files between nested folders" '{
    matched=$(srcdiff -q "$simple1" "$simple2")
    matched=$(normalize_path "$matched")
    assert_equal "$matched" "$deeper_output"
}'

echo "int x;" > "$simple2/deeper/0.cpp"
echo "int y;" > "$simple2/deeper/by.cpp"
echo "int z;" > "$simple2/deeper/c.cpp"

new_files_output=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="|deeper/0.cpp"><diff:insert><decl_stmt><decl><type><name>int</name></type><diff:ws> </diff:ws><name>x</name></decl>;</decl_stmt><diff:ws>
</diff:ws></diff:insert></unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="deeper/a.cpp"><decl_stmt><decl><type><name>int</name></type> <name><diff:delete type="replace">i</diff:delete><diff:insert type="replace">j</diff:insert></name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="deeper/b.cpp"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="number"><diff:delete type="replace">1</diff:delete><diff:insert type="replace">2</diff:insert></literal></expr>;</expr_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="|deeper/by.cpp"><diff:insert><decl_stmt><decl><type><name>int</name></type><diff:ws> </diff:ws><name>y</name></decl>;</decl_stmt><diff:ws>
</diff:ws></diff:insert></unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="|deeper/c.cpp"><diff:insert><decl_stmt><decl><type><name>int</name></type><diff:ws> </diff:ws><name>z</name></decl>;</decl_stmt><diff:ws>
</diff:ws></diff:insert></unit>
END
)

run_test "srcDiff will show unmatchable files in folders as new" '{
    matched=$(srcdiff -q "$simple1" "$simple2")
    matched=$(normalize_path "$matched")
    assert_equal "$matched" "$new_files_output"
}'

with_standalone_files_output=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$original|$modified"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"<diff:delete type="replace">hi</diff:delete><diff:insert type="replace">hello</diff:insert>"</literal></expr>;</expr_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="|deeper/0.cpp"><diff:insert><decl_stmt><decl><type><name>int</name></type><diff:ws> </diff:ws><name>x</name></decl>;</decl_stmt><diff:ws>
</diff:ws></diff:insert></unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="deeper/a.cpp"><decl_stmt><decl><type><name>int</name></type> <name><diff:delete type="replace">i</diff:delete><diff:insert type="replace">j</diff:insert></name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="deeper/b.cpp"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="number"><diff:delete type="replace">1</diff:delete><diff:insert type="replace">2</diff:insert></literal></expr>;</expr_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="|deeper/by.cpp"><diff:insert><decl_stmt><decl><type><name>int</name></type><diff:ws> </diff:ws><name>y</name></decl>;</decl_stmt><diff:ws>
</diff:ws></diff:insert></unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="|deeper/c.cpp"><diff:insert><decl_stmt><decl><type><name>int</name></type><diff:ws> </diff:ws><name>z</name></decl>;</decl_stmt><diff:ws>
</diff:ws></diff:insert></unit>
END
)

run_test "srcDiff will show accept directory and file pairs at the same time" '{
    matched=$(srcdiff -q "$original" "$modified" "$simple1" "$simple2")
    matched=$(normalize_path "$matched")
    assert_equal "$matched" "$with_standalone_files_output"
}'
