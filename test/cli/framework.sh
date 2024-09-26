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
	        SRCML='/usr/local/bin/srcdiff'
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
    ["$CAPTURE_STDOUT" = true ] && exec 1>&3
    ["$CAPTURE_STDERR" = true ] && exec 2>&4
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