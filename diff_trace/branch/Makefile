##
# Diff trace Makefile

BIN=diff_trace_simple

all : ${BIN}

diff_trace_simple : diff_trace_simple.cpp SAX2DiffTraceSimple.cpp SAX2DiffTraceSimple.hpp
	g++ diff_trace_simple.cpp SAX2DiffTraceSimple.cpp -I/usr/include/libxml2 -lxml2 -I/opt/local/include/ -L/opt/local/lib -lmba -o $@

test : ${BIN}
	test/test_suite.py

clean :
	rm -f ${BIN}