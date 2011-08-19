#!/usr/bin/python
#
# test_format_data.py
#
# Michael J. Decker
# mjd52@zips.uakron.edu

import os
import os.path
import subprocess

test_count = 0
error_count = 0

form_data = "../form_data.py"
complete_form_data = "../complete_form_data.py"
list_diffs = "test_diffs"
sequence_output_data = "test_sequence"
complete_output_data = "complete_test_sequence"

sequence_command = [ form_data, list_diffs, sequence_output_data]
format_command = [ complete_form_data, sequence_output_data, complete_output_data ]

default_src2srcml_encoding = "UTF-8"
default_srcml2src_encoding = "UTF-8"

xml_declaration= '<?xml version="1.0" encoding="' + default_src2srcml_encoding + '" standalone="yes"?>'


def test(output, test_description):

	globals()["test_count"] += 1
	print test_count, test_description
	
	execute(sequence_command, "")
	execute(format_command, "")

        complete_output_file = open(complete_output_data, "r")
        line = complete_output_file.read()
        complete_output_file.close()

	return validate(line, output)

def validate(gen, expected):
    if gen != expected:
        globals()["error_count"] = globals()["error_count"] + 1
        print "ERROR"
        print "expected|" + str(expected) + "|"
        print "gen|" + str(gen) + "|"
	
        return

def execute(command, input):
    p = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    last_line = p.communicate(input)[0]

    if p.returncode != 0:
        globals()["error_count"] = globals()["error_count"] + 1
        print "Status error:  ", p.returncode, command

	return last_line

def write_file(filename, contents) :

        file = open(filename, "w")
        file.write(contents)
        file.close

        return

print

diffs = """
"""

complete_sequence = """@relation source_changes
@data
"""

write_file(list_diffs, diffs)
test(complete_sequence, "Empty Test")

diffs = ""

complete_sequence = """@relation source_changes
@data
"""

write_file(list_diffs, diffs)
test(complete_sequence, "Empty Test New Line")

diffs = """Delete:	/src:unit
Insert:	/src:unit
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2 }
@attribute structure { 'Delete:=src:unit', 'Insert:=src:unit' }
@data
1,'Delete:=src:unit'
2,'Insert:=src:unit'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "Simple Test")

diffs = """Delete:	/src:unit[@filename=old.c]
Insert:	/src:unit[@filename=new.c]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "@ Test")

diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "\' Test")

diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
Delete:	/src:unit[@filename='old.c']/src:function[src:signature('int main(int argc char * argv'))]
Insert:	/src:unit[@filename='new.c']/src:function[src:signature('int main(int argc char * argv'))]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2, 3, 4 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]', 'Delete:=src:function', 'Delete:=src:function[src:signature(int main(int argc char * argv))]', 'Insert:=src:function', 'Insert:=src:function[src:signature(int main(int argc char * argv))]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
3,'Delete:=src:unit'
3,'Delete:=src:unit[@filename=old.c]'
3,'Delete:=src:function'
3,'Delete:=src:function[src:signature(int main(int argc char * argv))]'
4,'Insert:=src:unit'
4,'Insert:=src:unit[@filename=new.c]'
4,'Insert:=src:function'
4,'Insert:=src:function[src:signature(int main(int argc char * argv))]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "src:signature Test ' '")

diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
Delete:	/src:unit[@filename='old.c']/src:function[src:signature('int main(int argc char * argv[)')]
Insert:	/src:unit[@filename='new.c']/src:function[src:signature('int main(int argc char * argv[)')]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2, 3, 4 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]', 'Delete:=src:function', 'Delete:=src:function[src:signature(int main(int argc char * argv[))]', 'Insert:=src:function', 'Insert:=src:function[src:signature(int main(int argc char * argv[))]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
3,'Delete:=src:unit'
3,'Delete:=src:unit[@filename=old.c]'
3,'Delete:=src:function'
3,'Delete:=src:function[src:signature(int main(int argc char * argv[))]'
4,'Insert:=src:unit'
4,'Insert:=src:unit[@filename=new.c]'
4,'Insert:=src:function'
4,'Insert:=src:function[src:signature(int main(int argc char * argv[))]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "src:signature Test '['")

diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
Delete:	/src:unit[@filename='old.c']/src:function[src:signature('int main(int argc char * argv[])')]
Insert:	/src:unit[@filename='new.c']/src:function[src:signature('int main(int argc char * argv[])')]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2, 3, 4 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]', 'Delete:=src:function', 'Delete:=src:function[src:signature(int main(int argc char * argv[]))]', 'Insert:=src:function', 'Insert:=src:function[src:signature(int main(int argc char * argv[]))]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
3,'Delete:=src:unit'
3,'Delete:=src:unit[@filename=old.c]'
3,'Delete:=src:function'
3,'Delete:=src:function[src:signature(int main(int argc char * argv[]))]'
4,'Insert:=src:unit'
4,'Insert:=src:unit[@filename=new.c]'
4,'Insert:=src:function'
4,'Insert:=src:function[src:signature(int main(int argc char * argv[]))]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "src:signature Test ']'")

diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
Delete:	/src:unit[@filename='old.c']/src:function[src:signature('int main(int argc, char * argv[])')]
Insert:	/src:unit[@filename='new.c']/src:function[src:signature('int main(int argc, char * argv[])')]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2, 3, 4 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]', 'Delete:=src:function', 'Delete:=src:function[src:signature(int main(int argc, char * argv[]))]', 'Insert:=src:function', 'Insert:=src:function[src:signature(int main(int argc, char * argv[]))]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
3,'Delete:=src:unit'
3,'Delete:=src:unit[@filename=old.c]'
3,'Delete:=src:function'
3,'Delete:=src:function[src:signature(int main(int argc, char * argv[]))]'
4,'Insert:=src:unit'
4,'Insert:=src:unit[@filename=new.c]'
4,'Insert:=src:function'
4,'Insert:=src:function[src:signature(int main(int argc, char * argv[]))]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "src:signature Test ','")


diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
Delete:	/src:unit[@filename='old.c']/src:function[src:signature('enum { OK, FAILED, DONE  send(int argc, char * argv[])')]
Insert:	/src:unit[@filename='new.c']/src:function[src:signature('enum { OK, FAILED, DONE  send(int argc, char * argv[])')]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2, 3, 4 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]', 'Delete:=src:function', 'Delete:=src:function[src:signature(enum { OK, FAILED, DONE  send(int argc, char * argv[]))]', 'Insert:=src:function', 'Insert:=src:function[src:signature(enum { OK, FAILED, DONE  send(int argc, char * argv[]))]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
3,'Delete:=src:unit'
3,'Delete:=src:unit[@filename=old.c]'
3,'Delete:=src:function'
3,'Delete:=src:function[src:signature(enum { OK, FAILED, DONE  send(int argc, char * argv[]))]'
4,'Insert:=src:unit'
4,'Insert:=src:unit[@filename=new.c]'
4,'Insert:=src:function'
4,'Insert:=src:function[src:signature(enum { OK, FAILED, DONE  send(int argc, char * argv[]))]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "src:signature Test '{'")


diffs = """Delete:	/src:unit[@filename='old.c']
Insert:	/src:unit[@filename='new.c']
Delete:	/src:unit[@filename='old.c']/src:function[src:signature('enum { OK, FAILED, DONE } send(int argc, char * argv[])')]
Insert:	/src:unit[@filename='new.c']/src:function[src:signature('enum { OK, FAILED, DONE } send(int argc, char * argv[])')]
"""

complete_sequence = """@relation source_changes
@attribute change { 1, 2, 3, 4 }
@attribute structure { 'Delete:=src:unit', 'Delete:=src:unit[@filename=old.c]', 'Insert:=src:unit', 'Insert:=src:unit[@filename=new.c]', 'Delete:=src:function', 'Delete:=src:function[src:signature(enum { OK, FAILED, DONE } send(int argc, char * argv[]))]', 'Insert:=src:function', 'Insert:=src:function[src:signature(enum { OK, FAILED, DONE } send(int argc, char * argv[]))]' }
@data
1,'Delete:=src:unit'
1,'Delete:=src:unit[@filename=old.c]'
2,'Insert:=src:unit'
2,'Insert:=src:unit[@filename=new.c]'
3,'Delete:=src:unit'
3,'Delete:=src:unit[@filename=old.c]'
3,'Delete:=src:function'
3,'Delete:=src:function[src:signature(enum { OK, FAILED, DONE } send(int argc, char * argv[]))]'
4,'Insert:=src:unit'
4,'Insert:=src:unit[@filename=new.c]'
4,'Insert:=src:function'
4,'Insert:=src:function[src:signature(enum { OK, FAILED, DONE } send(int argc, char * argv[]))]'
"""

write_file(list_diffs, diffs)
test(complete_sequence, "src:signature Test '}'")

