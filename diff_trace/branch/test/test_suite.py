#!/usr/bin/python
#
# test_suite.py
#
# Michael J. Decker
# mjd52@zips.uakron.edu

import os.path
import subprocess

test_count = 0
error_count = 0

diff_trace = "../diff_trace_simple"
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

output = """Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
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
Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
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

output = """Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
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

output = """Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
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

output = """Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Empty End Line");

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

output = """Insert:\t/src:unit
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Empty Before Line");

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

output = """Delete:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
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

output = """Delete:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Change Statement Completely (Same Structure) Line After");

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

output = """Delete:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
Insert:\t/src:unit
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Global Level - Change Statement Completely (Same Structure) Line Before");

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

output = """Delete:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
Insert:\t/src:unit/src:decl_stmt/src:decl/src:type/src:name
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
Insert:\t/src:unit/src:class_decl[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Definition to Declaration Extra Space After");

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

output = """Delete:\t/src:unit
Insert:\t/src:unit/src:class_decl[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class Name - Definition to Declaration Extra Space Vefore");

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

class name {
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

<class>class <name>name</name> <block>{<private type="default">
</private>}</block>;</class>

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

output = """Delete:\t/src:unit/src:class[src:construct('class name')]
Insert:\t/src:unit/src:struct_decl[src:construct('struct name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class to Struct - Definition to Declaration");

old = """#import <stdio>

class name;

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

<class_decl>class <name>name</name>;</class_decl>

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

output = """Delete:\t/src:unit/src:class_decl[src:construct('class name')]
Insert:\t/src:unit/src:struct[src:construct('struct name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Class to Struct - Declaration to Definition");

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
test(diff_trace_command, "", output, "Struct to Class - Definition");

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

old = """#import <stdio>

struct name {
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

<struct>struct <name>name</name> <block>{<public type="default">
</public>}</block>;</struct>

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

output = """Delete:\t/src:unit/src:struct[src:construct('struct name')]
Insert:\t/src:unit/src:class_decl[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct to Class - Definition to Declaration");

old = """#import <stdio>

struct name;

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

<struct_decl>struct <name>name</name>;</struct_decl>

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

output = """Delete:\t/src:unit/src:struct_decl[src:construct('struct name')]
Insert:\t/src:unit/src:class[src:construct('class name')]
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Struct to Class - Declaration to Definition");

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
test(diff_trace_command, "", output, "Add Function - Space After");

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

output = """Insert:\t/src:unit
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Space Before");

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
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Add Function - Two Consecutive with Space Between");

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

  return 0;
}

int main(int argc, char * argv[]) {

  return 0;
}

int __main(int argc, char * argv[]) {

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
<function><type><name>int</name></type> <name>_main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>

<function><type><name>int</name></type> <name>__main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int _main(int argc, char * argv[])')]
Insert:\t/src:unit
"""

# diff is sometimes wrong (Limitation)
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

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt/src:decl/src:type/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Statement No Space");

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

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt/src:decl/src:type/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Statement Space After");

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

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Statement Space Before");

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

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt/src:decl/src:type/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt/src:decl/src:type/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement Change Declaration Type");

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

  <decl_stmt><decl><type><name>int</name></type> <name>value</name> =<init> <expr>1</expr></init></decl>;</decl_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt/src:decl/src:init/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:decl_stmt/src:decl/src:init/src:expr
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement Change Declaration Value");

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

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement Change Expression Name");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  call(one, two);
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
  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Call Statement No Space");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  call(one, two);

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
  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Call Statement Space After");

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

  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Call Statement Space Before");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  call(one, two);

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  acall(one, two);

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>acall</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Call Statement Change Name Begin");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  call(one, two);

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  called(one, two);

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>called</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:argument_list
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Call Statement Change Name End");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  call(one, two);

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  call(red, blue);

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>red</name></expr></argument>, <argument><expr><name>blue</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:argument_list/src:argument/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt/src:expr/src:call/src:argument_list/src:argument/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Call Statement Change Argument");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  call(one, two)

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  call(one, two);

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <macro><name>call</name><argument_list>(<argument>one</argument>, <argument>two</argument>)</argument_list></macro>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <expr_stmt><expr><call><name>call</name><argument_list>(<argument><expr><name>one</name></expr></argument>, <argument><expr><name>two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:expr_stmt
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Call Statement Change ;");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i < 10) {

    ++i;
  }
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
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add While No Space");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i < 10) {

    ++i;
  }

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
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block></while>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add While Space After");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  while(i < 10) {

    ++i;
  }
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

  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add While Space Before");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  int i = 0;
  while(j < 10) {

    ++i;
  }

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  int i = 0;
  while(i < 10) {

    ++i;
  }

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>j</name> &lt; 10</expr>)</condition> <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block></while>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block></while>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while/src:condition/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while/src:condition/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - While Change Condition");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i++ < 10);
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
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition><empty_stmt>;</empty_stmt></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Empty While No Space");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i++ < 10);

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
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition><empty_stmt>;</empty_stmt></while>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Empty While Space After");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  while(i++ < 10);
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

  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition><empty_stmt>;</empty_stmt></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add While Space Before");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  while(j < 10) ++i;
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i < 10) i++;
  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <while>while<condition>(<expr><name>j</name> &lt; 10</expr>)</condition> <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <expr_stmt><expr><name>i</name>++</expr>;</expr_stmt></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while/src:condition/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while/src:condition/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement While Change Condition");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i < 10) ++i;
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  while(i < 10) i++;
  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <while>while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition> <expr_stmt><expr><name>i</name>++</expr>;</expr_stmt></while>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while/src:expr_stmt/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:while/src:expr_stmt/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement While Change Statement");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  for(int i = 0; i < 10; ++i) {

    i = i;
  }
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
  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add For No Space");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  for(int i = 0; i < 10; ++i)

    i = i;
  }
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
  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add For Space After");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) {

    i = i;
  }
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

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add For Space Before");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) {

    i = i;
  }

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 1; i < 10; ++i) {

    i = i;
  }

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>1</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:init/src:decl/src:init/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:init/src:decl/src:init/src:expr
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - For Change Init");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; j < 10; ++i) {

    i = i;
  }

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) {

    i = i;
  }

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>j</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:condition/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:condition/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - For Change Condition");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++j) {

    i = i;
  }

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) {

    i = i;
  }

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>j</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <block>{

    <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt>
  }</block></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:incr/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:incr/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - For Change Incr");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) i = i;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 1; i < 10; ++i) i = i;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>1</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:init/src:decl/src:init/src:expr
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:init/src:decl/src:init/src:expr
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement For Change Init");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; j < 10; ++i) i = i;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) i = i;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:condition/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:condition/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement For Change Condition");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++j) i = i;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) i = i;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>j</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:incr/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:incr/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement For Change Incr");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) i = j;

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  for(int i = 0; i < 10; ++i) i = i;

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>j</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>j</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</init> <condition><expr><name>i</name> &lt; 10</expr>;</condition> <incr><expr>++<name>i</name></expr></incr>) <expr_stmt><expr><name>i</name> = <name>i</name></expr>;</expr_stmt></for>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:expr_stmt/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:for/src:expr_stmt/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Statement For Change Block");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  do{

    ++i;
  } while(i < 10);
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
  <do>do <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block> while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition>;</do>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:do
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Do While No Space");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  do{

    ++i;
  } while(i < 10);

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
  <do>do <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block> while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition>;</do>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:do
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Do While Space After");

old = """#import <stdio>

int main(int argc, char * argv[]) {
  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  do{

    ++i;
  } while(i < 10);
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

  <do>do <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block> while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition>;</do>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Add Do While Space Before");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  do{

    ++i;
  } while(j < 10);

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {

  do{

    ++i;
  } while(i < 10);

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <do>do <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block> while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition>;</do>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <do>do <block>{

    <expr_stmt><expr>++<name>i</name></expr>;</expr_stmt>
  }</block> while<condition>(<expr><name>i</name> &lt; 10</expr>)</condition>;</do>

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:do/src:condition/src:expr/src:name
Insert:\t/src:unit/src:function[src:signature('int main(int argc, char * argv[])')]/src:block/src:do/src:condition/src:expr/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Do While Change Condition");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
#import <stdio>
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++" filename="old.cpp"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++" filename="new.cpp"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit[@filename='old.cpp']
Insert:\t/src:unit[@filename='new.cpp']/src:cpp:error
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Unit Filename Test Global");

old = """#import <stdio>

int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>

int main(int argc, char * argv[]) {
  int i = 0;
  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++" filename="old.cpp"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++" filename="new.cpp"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit[@filename='old.cpp']/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
Insert:\t/src:unit[@filename='new.cpp']/src:function[src:signature('int main(int argc, char * argv[])')]/src:block
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Unit Filename Test Block");

old = """#import <stdio>
int i = 0;

int j = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

new = """#import <stdio>
int k = 0;

int l = 0;
int main(int argc, char * argv[]) {

  return 0;
}
"""

old_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>i</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>int</name></type> <name>j</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{

  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

new_srcml = xml_declaration + """
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" language="C++"><cpp:error>#<cpp:directive>import</cpp:directive> &lt;stdio&gt;</cpp:error>
<decl_stmt><decl><type><name>int</name></type> <name>k</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>int</name></type> <name>l</name> =<init> <expr>0</expr></init></decl>;</decl_stmt>
<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> *</type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <block>{
  <return>return <expr>0</expr>;</return>
}</block></function>
</unit>
"""

output = """Delete:\t/src:unit/src:decl_stmt/src:decl/src:name
Delete:\t/src:unit/src:decl_stmt/src:decl/src:name
Insert:\t/src:unit/src:decl_stmt/src:decl/src:name
Insert:\t/src:unit/src:decl_stmt/src:decl/src:name
"""

write_test_files(old, new, old_srcml, new_srcml)
test(diff_trace_command, "", output, "Block Level - Unit Filename Test Block");

# footer
print
print "Error count: ", error_count
print

exit
