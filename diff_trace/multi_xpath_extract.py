#!/usr/bin/python

import subprocess


def run(command, inputs) :

    process = subprocess.Popen(command, stdin=subprocess.Popen.stdin, stdout=subprocess.Popen.stdout, stderr=subprocess.Popen.stderr)

    output = process.communicate(inputs)[0]

    return output

print run(["ls"], "")
