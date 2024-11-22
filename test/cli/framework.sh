## SPDX-License-Identifier: GPL-3.0-only
#
# @file framework.sh
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

#!/bin/bash

export REVISION=1.0.0

# Create temp directory 
TEMPDIR=./tmp/$(basename $0 .sh)

# Remove old TEMPDIR
rm -fR $TEMPDIR
mkdir -p $TEMPDIR
cd $TEMPDIR

# Find srcDiff executable
export PATH=.:$PATH

if [[ "$OSTYPE" == 'msys' ]]; then
    EOL="\r\n"
    export PATH=$PATH:"/c/Program Files/srcDiff/bin/"
    SRCML="$SRCML_HOME/srcdiff.exe"
    export MSYS2_ARG_CONV_EXCL="*"
else
    EOL="\n"
	if [ -z "$SRCDIFF"]; then

	    if [ -e "/usr/bin/srcdiff" ]; then
	        SRCDIFF='/usr/bin/srcdiff'
	    fi

	    if [ -e "/usr/local/bin/srcdiff" ]; then
	        SRCDIFF='/usr/local/bin/srcdiff'
	    fi

	fi
fi

function srcdiff() {
    "$SRCDIFF" "$@"
}

# History on
set -o history
HISTIGNORE=check:\#
HISTSIZE=2
HISTFILESIZE=0

# Output the first entry in history file, without numbers
firsthistoryentry() {
    fc -l -n -l
}

CAPTURE_STDOUT=true
CAPTURE_STDERR=true

# Variable $1 is set to contents of stdin
define() {
    # Read stdin into variable $1
    IFS= read -r -d '' $1 || true

    # Replace any mention of REVISION with the revision number
    eval $1=\${$1//REVISION/{$REVISION}}
} 

# file with name `$1` is created from contents of var `$2`

create_file() {
    # Make directory paths
    mkdir -p $(dirname $1)

    # add contents to file
    echo -ne "${2}" > ${1}
}

rmfile() { rm -f ${1}; }
rmdir()  { rm -fr ${1};} 

# capture stdout/stderr
capture_output() {
    [ "$CAPTURE_STDOUT" = true ] && exec 3>&1 1>$STDOUT
    [ "$CAPTURE_STDERR" = true ] && exec 4>&2 2>$STDERR
}

# uncapture stdout/stderr
uncapture_output() {
    [ "$CAPTURE_STDOUT" = true ] && exec 1>&3
    [ "$CAPTURE_STDERR" = true ] && exec 2>&4
}

message() {
    # return stdout and stderr to standard streams
    uncapture_output
    
    echo "$1" >&2

    capture_output

    true
}

# output filenames for capture stdout/stderr
base=$(basename $0 .sh)

typeset STDERR=.stderr_$base
typeset STDOUT=.stdout_$base

# save stdout and stderr to files
capture_output

#Check the result of command

check() {

    local exit_status = $?

    set -e

    # test file pattern
    line = $(caller | cut -d' ' -f1)
    TEMPFILE = $PWD'/.test.'$line

    # return stdout and stderr to standard streams
    uncapture_output

    # follow the command
    firsthistoryentry

    #check <filename> stdoutstr stderrstr
    if [ $# -ge 3]; then
        tmpfile2 = $TEMPFILE.2
        echo -en '$2' > $tmpfile2
        $diff $tmpfile2 $1

        tmpfile3 = $TEMPFILE.3
        echo -en "$3" > $tmpfile3
        $diff $tmpfile3 $1

    #check <filename> stdoutstr
    elif [ $# ge 2 ] && [ "$1" != "" ] && [ -e "$1"]; then
        tmpfile2 = $TEMPFILE.2
        echo -en "$2" > $tmpfile2
        $diff $tmpfile2 $1

        [! -s $STDERR]

    # check stdoutstr stderrstr
    elif [ $# -ge 2 ]; then

        tmpfile1 = $TEMPFILE.1
        echo -en "$1" > $tmpfile1
        $diff $tmpfile1 $STDOUT

    # check <filename>
    elif [ $# -ge 1 ] && [ "$1" != "" ] && [ -e "$1" ]; then

        $diff $1 $STDOUT

        [ ! -s $STDERR ]

    # check stdoutstr
    elif [ $# -ge 1 ]; then

        tmpfile1=$TEMPFILE.1
        echo -en "$1" > $tmpfile1
        $diff $tmpfile1 $STDOUT

        [ ! -s $STDERR ]

    else
        # check that the captured stdout is empty
        [ ! -s $STDOUT ]
        [ ! -s $STDERR ]
    fi

    set +e

    if [ $exit_status -ne 0 ]; then
        exit 1
    fi

    # return to capturing stdout and stderr
    capture_output

    true
}

##
# checks the exit status of a command
#   $1 expected return value
check_exit() {
    local exit_status=$?

    # return stdout and stderr to standard streams
    uncapture_output

    # trace the command
    firsthistoryentry

    # verify expected stderr to the captured stdout
    if [ $exit_status -ne $1 ]; then
        echo "error: exit was $exit_status instead of $1"
        exit 8
    fi

    set -e

    # testfile pattern
    line=$(caller | cut -d' ' -f1)
    TEMPFILE=$PWD'/.test.'$line

    if [ $# -eq 2 ]; then
        tmpfile2=$TEMPFILE.2
        echo -en "$2" > $tmpfile2
        $diff $tmpfile2 $STDERR

        [ ! -s $STDOUT ]
    fi

    if [ $# -eq 3 ]; then
        tmpfile2=$TEMPFILE.2
        echo -en "$2" > $tmpfile2
        $diff $tmpfile2 $STDOUT

        tmpfile3=$TEMPFILE.3
        echo -en "$3" > $tmpfile3
        $diff $tmpfile3 $STDERR
    fi

    set +e

    # return to capturing stdout and stderr
    capture_output

    true
}

##
# checks the exit status of a command
#   $1 expected number in stdout
check_lines() {
    # return stdout and stderr to standard streams
    uncapture_output

    # trace the command
    firsthistoryentry

    local stdcount=$(wc -l $STDOUT | cut -d' ' -f1 | sed 's/^ *//;s/ *$//')

    # verify expected stderr to the captured stdout
    if [ "$stdcount" != "$1" ]; then
        echo "error: expected $1 lines, got $stdcount"
        exit 9
    fi

    # return to capturing stdout and stderr
    capture_output

    true
}


# Check the validity of the xml
# Currently only checks for well-formed xml, not DTD validity
xmlcheck() {

    set -e

    if [ "${1:0:1}" != "<" ]; then
        xmllint --noout ${1}
    else
        echo "${1}" | xmllint --noout /dev/stdin
    fi;

    set +e
    true
}