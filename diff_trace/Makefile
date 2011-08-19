##
# Diff trace Makefile

BIN=diff_mba_srcML_trace

all : ${BIN} diff_mba_srcML

${BIN} : diff_mba_srcML_trace.cpp SAX2DiffTrace.cpp SAX2DiffTrace.hpp
	g++ diff_mba_srcML_trace.cpp SAX2DiffTrace.cpp -I/usr/include/libxml2 -lxml2 -I/opt/local/include/ -L/opt/local/lib -lmba -o $@

diff_mba_srcML : diff_mba_srcML.cpp
	g++ diff_mba_srcML.cpp -I/opt/local/include/ -L/opt/local/lib -lmba -o $@

test : ${BIN} diff_mba_srcML
	#./diff_mba_srcML test/examples/exampleold.c test/examples/examplenew.c
	./diff_mba_srcML_trace test/examples/exampleold.c test/examples/examplenew.c test/examples/exampleold.c.xml test/examples/examplenew.c.xml

clean :
	rm -f ${BIN} diff_mba_srcML