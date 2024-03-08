# creating very simple temp files to use for diffs:

original=$(mktemp --suffix .cpp)
echo "std::cout << \"hi\";" > $original
modified=$(mktemp --suffix .cpp)
echo "std::cout << \"hello\";" > $modified

expected_diff=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$original|$modified"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"<diff:delete type="replace">hi</diff:delete><diff:insert type="replace">hello</diff:insert>"</literal></expr>;</expr_stmt>
</unit>
END
)

expected_diff_twice=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$original|$modified"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"<diff:delete type="replace">hi</diff:delete><diff:insert type="replace">hello</diff:insert>"</literal></expr>;</expr_stmt>
</unit><unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="$original|$modified"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"<diff:delete type="replace">hi</diff:delete><diff:insert type="replace">hello</diff:insert>"</literal></expr>;</expr_stmt>
</unit>
END
)

expected_diff_archive=$(cat <<-END
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0">

<unit revision="1.0.0" language="C++" filename="$original|$modified"><expr_stmt><expr><name><name>std</name><operator>::</operator><name>cout</name></name> <operator>&lt;&lt;</operator> <literal type="string">"<diff:delete type="replace">hi</diff:delete><diff:insert type="replace">hello</diff:insert>"</literal></expr>;</expr_stmt>
</unit>

</unit>
END
)

if0_code=$(cat <<-END
#if 0
std::cout << "hello world";
#endif
END
)

else_code=$(cat <<-END
#if A
std::cout << "hello world";
#else
std::cout << "goodbye";
#endif
END
)
