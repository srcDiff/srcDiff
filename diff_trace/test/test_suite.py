#!/usr/bin/python
#
# clitest.py
#
# Michael L. Collard
# collard@cs.kent.edu

import os.path
import subprocess

test_count = 0
error_count = 0

diff_trace = "../diff_mba_srcML_trace"
old_filename = "old.cpp"
new_filename = "new.cpp"
old_srcml_filename = "old.cpp.xml"
new_srcml_filename = "new.cpp.xml"

diff_trace_command = [ diff_trace, old_filename, new_filename, old_srcml_filename, new_srcml_filename ]

default_src2srcml_encoding = "UTF-8"
default_srcml2src_encoding = "UTF-8"

xml_declaration= '<?xml version="1.0" encoding="' + default_src2srcml_encoding + '" standalone="yes"?>'


def test(command, input, output, test_description):

	globals()["test_count"] += 1
	print test_count, test_description
	
	line = execute(command, input)

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

def write_test_files(old, new, old_srcml, new_srcml) :

        write_file(old_filename, old)
        write_file(new_filename, new)
        write_file(old_srcml_filename, old_srcml)
        write_file(new_srcml_filename, new_srcml)
        
        
        return

def write_file(filename, contents) :

        file = open(filename, "w")
        file.write(contents)
        file.close

        return

print

##
# empty default

old = """
"""

new = """
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"/>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"/>
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", "", "Empty Files")

##
# same

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = old

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = old_srcml

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", "", "No Difference")

##
# global level

old = """#import <stdio>
int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int global = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:decl_stmt[1]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - No White Space Before or After");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int global = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit
Insert:\t/src:unit/src:decl_stmt[1]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Change White Space to Condition");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:decl_stmt[1]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Existing Space Before");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:decl_stmt[1]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Existing Space After");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:decl_stmt[1]
Insert:\t/src:unit
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Empty End Line");

old = """#import <stdio>
double variable = 1;
int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int global = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>double</name></type> <name>variable</name> =<init> <expr>1</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:decl_stmt[1]/src:decl
Insert:\t/src:unit/src:decl_stmt[1]/src:decl
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Change Statement Completely (Same Structure)");

old = """#import <stdio>
double variable = 1;
int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>double</name></type> <name>variable</name> =<init> <expr>1</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

# may be correct
output = """Delete:\t/src:unit/src:decl_stmt[1]/src:decl
Insert:\t/src:unit/src:decl_stmt[1]/src:decl
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Change Statement Completely (Same Structure) Line After");

old = """#import <stdio>
double variable = 1;
double variable = 1;
int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int global = 0;
int global = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>double</name></type> <name>variable</name> =<init> <expr>1</expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>double</name></type> <name>variable</name> =<init> <expr>1</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

# may be correct
output = """Delete:\t/src:unit/src:decl_stmt[1]/src:decl
Insert:\t/src:unit/src:decl_stmt[1]/src:decl
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Two Consecutive Change Statement Completely (Same Structure)");

##
# function signature changes

old = """#import <stdio>

int global = 0;

void main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>void</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('void main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Return Type");

old = """#import <stdio>

int global = 0;

int _main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Name");

old = """#import <stdio>

int global = 0;

int main(double argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>double</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(double argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Parameter Type");

old = """#import <stdio>

int global = 0;

int main(int argc, char * args[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>args</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * args[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Parameter Name");

old = """#import <stdio>

int global = 0;

void main(int argc, char * argv[]);
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]);
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>void</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

output = """Delete:\t/src:unit/src:function_decl[src:signature('void main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Declaration Return Value");

old = """#import <stdio>

int global = 0;

int _main(int argc, char * argv[]);
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]);
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

output = """Delete:\t/src:unit/src:function_decl[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Declaration Name");

old = """#import <stdio>

int global = 0;

int main(double argc, char * argv[]);
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]);
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>double</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

output = """Delete:\t/src:unit/src:function_decl[src:signature('int main(double argc, char * argv[])')]
Insert:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Declaration Parameter Type");

old = """#import <stdio>

int global = 0;

int main(int argc, char * args[]);
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]);
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>args</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

output = """Delete:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * args[])')]
Insert:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Declaration Parameter Name");

old = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]);
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Declaration to Definition");

old = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int global = 0;

int main(int argc, char * argv[]);
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<decl_stmt><decl><type><name>int</name></type> <name>global</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<function_decl><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list>;</function_decl>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function_decl[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Function Signature - Definition to Declaration");

##
# Struct Name

old = """#import <stdio>

struct name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

struct Name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct>struct <name>name</name> <block>{<public type="default">
</public>}</block>;</struct>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct>struct <name>Name</name> <block>{<public type="default">
</public>}</block>;</struct>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:struct[src:construct('struct name')]
Insert:\t/src:unit/src:struct[src:construct('struct Name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct Name - Definition");

old = """#import <stdio>

struct name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

struct Name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct_decl>struct <name>name</name>;</struct_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct_decl>struct <name>Name</name>;</struct_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:struct_decl[src:construct('struct name')]
Insert:\t/src:unit/src:struct_decl[src:construct('struct Name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct Name - Declaration");

old = """#import <stdio>

struct name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

struct name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct_decl>struct <name>name</name>;</struct_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct>struct <name>name</name> <block>{<public type="default">
</public>}</block>;</struct>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:struct_decl[src:construct('struct name')]
Insert:\t/src:unit/src:struct[src:construct('struct name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct Name - Declaration to Definition");

old = """#import <stdio>

struct name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

struct name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct>struct <name>name</name> <block>{<public type="default">
</public>}</block>;</struct>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct_decl>struct <name>name</name>;</struct_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:struct[src:construct('struct name')]
Insert:\t/src:unit/src:struct_decl[src:construct('struct name')]
"""

# may be an error
write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct Name - Definition to Declaration");

##
# Class Name

old = """#import <stdio>

class name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class Name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>Name</name> <block>{<private type="default">
</private>}</block>;</class>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class[src:construct('class name')]
Insert:\t/src:unit/src:class[src:construct('class Name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Definition");

old = """#import <stdio>

class name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class Name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>Name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class_decl[src:construct('class name')]
Insert:\t/src:unit/src:class_decl[src:construct('class Name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Declaration");

old = """#import <stdio>

class name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class_decl[src:construct('class name')]
Insert:\t/src:unit/src:class[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Declaration to Definition");

old = """#import <stdio>

class name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class[src:construct('class name')]
Insert:\t/src:unit/src:class_decl[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Definition to Declaration");

old = """#import <stdio>

class name {
};


int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>


<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class[src:construct('class name')]
Delete:\t/src:unit
Insert:\t/src:unit/src:class_decl[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Definition to Declaration Extra Space");

##
# Class and Struct

old = """#import <stdio>

class name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

struct name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct>struct <name>name</name> <block>{<public type="default">
</public>}</block>;</struct>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class[src:construct('class name')]
Insert:\t/src:unit/src:struct[src:construct('struct name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class to Struct - Definition");

old = """#import <stdio>

class name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

struct name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct_decl>struct <name>name</name>;</struct_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:class_decl[src:construct('class name')]
Insert:\t/src:unit/src:struct_decl[src:construct('struct name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class to Struct - Declaration");

old = """#import <stdio>

struct name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class name {
};

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct>struct <name>name</name> <block>{<public type="default">
</public>}</block>;</struct>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:struct[src:construct('struct name')]
Insert:\t/src:unit/src:class[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct to Class");

old = """#import <stdio>

struct name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

class name;

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<struct_decl>struct <name>name</name>;</struct_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<class_decl>class <name>name</name>;</class_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:struct_decl[src:construct('struct name')]
Insert:\t/src:unit/src:class_decl[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct to Class - Declaration");

##
# Add Function

old = """#import <stdio>

"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Basic");

old = """#import <stdio>
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Space Before");

old = """#import <stdio>
"""

new = """#import <stdio>
int main(int argc, char * argv[]) {

  return 0;
}

"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>

</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
Insert:\t/src:unit
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Space After");

old = """#import <stdio>

"""

new = """#import <stdio>

int _main(int argc, char * argv[]) {

  return 0;
}
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Two Consecutive");

old = """#import <stdio>

"""

new = """#import <stdio>

int _main(int argc, char * argv[]) {

  return 0;
}

int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Two Consecutie with Space Between");

old = """#import <stdio>
int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int _main(int argc, char * argv[]) {
  return 1;
}
int main(int argc, char * argv[]) {

  return 0;
}
int __main(int argc, char * argv[]) {
  return 2;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<function><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <return>return <expr>1</expr>;</return>
}</block></function>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
<function><type><name>int</name></type> <name>__main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <return>return <expr>2</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int __main(int argc, char * argv[])')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Two Apart No Space");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int _main(int argc, char * argv[]) {
  return 1;
}

int main(int argc, char * argv[]) {

  return 0;
}

int __main(int argc, char * argv[]) {
  return 2;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <return>return <expr>1</expr>;</return>
}</block></function>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>

<function><type><name>int</name></type> <name>__main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <return>return <expr>2</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit/src:function[src:signature('int __main(int argc, char * argv[])')]
"""

# diff is wrong and spacing is wrong (Limitation)
write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Two Apart Space");

##
# Block Level

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  return 1;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:return/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:return/src:expr
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Function Return");

##
# Block Level

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  int value = 0;
  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]
"""

# error is the white space in front
write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Statement");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  int value = 0;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  double value = 0;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>double</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]/src:decl/src:type/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]/src:decl/src:type/src:name
"""

# Change type is type/name?
write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statment Change Declaration Type");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  int value = 0;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  int value = 1;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]/src:decl/src:init/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]/src:decl/src:init/src:expr
"""

# Change type is type/name?
write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statment Change Declaration Value");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  int value = 0;

  valu = 1;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  int value = 0;

  value = 1;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

  <expr_stmt><expr><name>valu</name> = 1</expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

  <expr_stmt><expr><name>value</name> = 1</expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]/src:decl/src:init/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt[1]/src:decl/src:init/src:expr
"""

# Change type is type/name?
write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statment Change Expression Name");

# footer
print
print "Error count: ", error_count
print

exit
