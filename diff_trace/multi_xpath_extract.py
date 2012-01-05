#!/usr/bin/python

import subprocess


def run(command, inputs) :

    process = subprocess.Popen(command, stdin=Popen.stdin, stdout=Popen.stdout, stderr=Popen.stderr)

    output = process.communicate(inputs)[0]

    return output

print run(["ls"])
