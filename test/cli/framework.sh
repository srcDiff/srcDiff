#!/bin/bash

# make srcdiff build available:

export PATH="../../build/bin:$PATH"

# if srcdiff is not found:
if ! [ -x "$(command -v srcdiff)" ]; then

    # if srcdiff.exe is found:
    if [ -x "$(command -v srcdiff.exe)" ]; then

        # the .exe file extension must be manually specified when running a
        # windows executable from WSL's bash; see protracted discussion:
        # https://github.com/microsoft/WSL/issues/2003
        alias srcdiff="srcdiff.exe";

        # without this, aliases aren't effective in scripts
        shopt -s expand_aliases;

    else
        
        echo "Could not find srcdiff! Exiting." > /dev/stderr;
        exit 1;

    fi
fi

# set up testing functions:

# create output log file and force it to be empty
LOG=test_log.txt
touch $LOG
echo "" > $LOG

# redirect stdout and stderr to file
capture_output() {
    # create temp file descriptors 3 and 4 to hold the original standard output
    # and error, then set standard output and error to append to the log file
    exec 3>&1 1>>$LOG
    exec 4>&2 2>>$LOG
}

# release stdout and stderr
uncapture_output() {
    # restore standard output and error from the temp file descriptors created
    # in capture_output, closing the temp fds while doing so
    exec 1>&3 3>&-
    exec 2>&4 4>&-
}

# variables to keep track of what's passed and failed
passed=0
failed=0
failures=()

# this should be set to the name of the current test file while tests are being run
current_file=""

# base test running function.
# $1: indicates whether to expect the test code to succeed (1) or fail (0)
# $2: description of test
# $3: shell script for test
_run_test() {
    local expect_success="$1"
    shift
    local description="$1"
    shift
    local test_case="$@"

    # get the next line number of the log file so it can be referenced in messages
    local log_file_length=$(wc -l < $LOG)
    ((log_file_length++))

    capture_output

    echo -e "\n\nTesting: \"$description\"\n\n"

    (
        set -e
        eval "$test_case"
    )

    local exit_code=$?

    uncapture_output

    # if the code block was supposed to error out but succeeded, or was supposed
    # to succeed but errored out, then the test automatically failed
    if ([ $expect_success -eq 1 ] && [ $exit_code -ne 0 ]) ||
            ([ $expect_success -eq 0 ] && [ $exit_code -eq 0 ]); then
        echo "Test failed: $description (exit code $exit_code) (see $LOG:$log_file_length)"
        ((failed++))
        failures+="$description ($current_file) ($LOG:$log_file_length)"
    else
        echo "Test passed: $description (exit code $exit_code)"
        ((passed++))
    fi
}

# run test and expect success
run_test() {
    _run_test 1 "$@"
}

# run test and expect failure
expect_failure() {
    _run_test 0 "$@"
}

# case insensitive argument equality comparison
assert_equal() {
    if [ "${1,,}" != "${2,,}" ]; then
        echo "ERROR: \`$1\` is not equal to \`$2\`"
        exit 1
    else
        echo "SUCCESS: \`$1\` is equal to \`$2\`"
    fi
}

# assert the first argument contains the second argument as a substring.
# case insensitive
assert_contains() {
    if [[ "${1,,}" != *"${2,,}"* ]]; then
        echo "ERROR: \`$1\` does not contain \`$2\`"
        exit 1
    else
        echo "SUCCESS: \`$1\` contains \`$2\`"
    fi
}

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

# run all tests:

current_dir="$(dirname "$0")"
for test in "$current_dir"/options/*; do
    if [ -f "$test" ]; then
        # setting global variable current_file so it can be picked up in error
        # strings if its test fail
        current_file=$(basename -- $test)

        log_file_length=$(($(wc -l < $LOG)+3))

        echo -e "\n\nRunning $current_file" >> $LOG

        echo -e "\nRunning $current_file ($LOG:$log_file_length)"

        source "$test"
    fi
done

# output results of running all tests:

echo -e "\n$passed passed; $failed failed"
if [ $failed -gt 0 ]; then
    echo -e "\nFailures:"
    for failure in "${failures[@]}"; do echo "$failure"; done
    echo -e "\nExiting with code 1"
    exit 1
fi

# remove temp files:

rm $original
rm $modified
