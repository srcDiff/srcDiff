# set compiler version to match that of antlr library
CXX=/usr/bin/g++

# associated text tools
SED=${firstword ${wildcard /opt/local/bin/gsed /bin/sed}}
GREP=${firstword ${wildcard /bin/grep /usr/bin/grep}}
ECHO=/bin/echo
CUT=/usr/bin/cut

VERSION="\"Trunk $(shell svnversion -n)$(shell svn info | ${GREP} 'Last Changed Date:' | ${CUT} -d: -f2-)\""

# compiler optimizations
CXXFLAGS = -DVERSION=$(VERSION) -O3 -Wall -march=nocona -minline-all-stringops #-fno-rtti

# compiler gcc linking flags
CXX_LINK_FLAGS = #-s

# antlr tool
ANTLR=${firstword ${wildcard /usr/bin/antlr /usr/bin/runantlr /usr/bin/cantlr /opt/local/bin/antlr}}

# antlr c++ include files
ifndef MINGW32
ANTLR_INC_DIR=${dir ${firstword ${wildcard /usr/include/antlr /opt/local/include/antlr}}}
ANTLR_INC=-I${ANTLR_INC_DIR}
endif

# antlr c++ library
ifndef MINGW32
ANTLR_LIB_DIR=${dir ${firstword ${wildcard /usr/lib64/libantlr* /usr/lib/libantlr* /opt/local/lib/libantlr*}}}
ANTLR_LIB=-L${ANTLR_LIB_DIR}
endif 
ANTLR_LIB += -lantlr

LDFLAGS=$(ANTLR_LIB)

# libxml
LIBXML_INCLUDE=-I/usr/include/libxml2

ifndef MINGW32
XML_LIB_SRC2SRCDIFF=-lxml2
else
XML_LIB_SRC2SRCDIFF=-lxml2 -lregex -liberty
endif

# project dirs
EXE_DIR = ../bin/
OBJ_DIR = ../obj/

EXE= diffe2diff diffeopt difffact diffver $(EXE_DIR)src2srcdiff
#diffe2diff diffe2src

all : ${EXE}

$(OBJ_DIR)src2srcdiff.o : src2srcdiff.cpp  ../src/srcMLTranslator.hpp ../src/srcmlapps.hpp ../src/Options.hpp ../src/srcmlns.hpp shortest_edit_script.h xmlrw.h diffrw.h
	$(CXX) $(ANTLR_INC) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(EXE_DIR)src2srcdiff : $(OBJ_DIR)src2srcdiff.o  $(OBJ_DIR)shortest_edit_script.o xmlrw.o ${OBJ_DIR}srcmlapps.o ${EXE_DIR}libsrcml.a
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $^  $(LDFLAGS) ${XML_LIB_SRC2SRCDIFF} ${XML_IO_LIB} -o $@

#src2srcdiff : src2srcdiff.o xmlrw.o
#	g++ src2srcdiff.o xmlrw.o -O3 -I/usr/include/libxml2 -lxml2 -o src2srcdiff

#src2srcdiff.o : src2srcdiff.cpp
#	g++ -c src2srcdiff.cpp -O3 -I/usr/include/libxml2

diffe2diff : diffe2diff.o xmlrw.o
	g++ diffe2diff.o xmlrw.o -O3 -I/usr/include/libxml2 -lxml2 -o diffe2diff

diffe2diff.o : diffe2diff.cpp xmlrw.h diffrw.h
	g++ -c diffe2diff.cpp -O3 -I/usr/include/libxml2

diffeopt : diffeopt.o xmlrw.o
	g++ diffeopt.o xmlrw.o -O3 -I/usr/include/libxml2 -lxml2 -o diffeopt

diffeopt.o : diffeopt.cpp
	g++ -c diffeopt.cpp -O3 -I/usr/include/libxml2

diffe2src : diffe2src.o xmlrw.o
	g++ diffe2src.o xmlrw.o -O3 -I/usr/include/libxml2 -lxml2 -o diffe2src

diffe2src.o : diffe2src.cpp
	g++ -c diffe2src.cpp -O3 -I/usr/include/libxml2

xmlrw.o : xmlrw.cpp xmlrw.h
	g++ -c xmlrw.cpp -O3 -I/usr/include/libxml2

v1v2diff.cpp.xml :
	diff --ifdef SRCDIFF v1.cpp v2.cpp | src2srcml - v1v2diff.cpp.xml

difffact : difffact.o xmlrw.o
	g++ difffact.o xmlrw.o -O3 -I/usr/include/libxml2 -lxml2 -o difffact

difffact.o : difffact.cpp
	g++ -c difffact.cpp -O3 -I/usr/include/libxml2

diffver : diffver.o xmlrw.o
	g++ diffver.o xmlrw.o -O3 -I/usr/include/libxml2 -lxml2 -o diffver

diffver.o : diffver.cpp
	g++ -c diffver.cpp -O3 -I/usr/include/libxml2

$(OBJ_DIR)diff_shortest_edit_script_srcml.o : diff_shortest_edit_script_srcml.cpp diff_shortest_edit_script_srcml.hpp shortest_edit_script.h
	g++ -c $< -o $@

$(OBJ_DIR)shortest_edit_script.o : shortest_edit_script.c shortest_edit_script.h
	g++ -c $< -o $@

$(OBJ_DIR)diff_file_replace.o : diff_file_replace.cpp diff_file_replace.hpp
	g++ -c $< -o $@

$(OBJ_DIR)diff_optimization.o : diff_optimization.cpp diff_optimization.hpp                                                                                                            
	g++ $(LIBXML_INCLUDE) -c $< -o $@ 

clean :
	rm -f ${EXE} diffe2diff.o xmlrw.o diffeopt.o diffe2src.o difffact difffact.o diffver diffver.o $(OBJ_DIR)src2srcdiff.o $(OBJ_DIR)diff_mba_srcml.o $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)diff_file_replace.o $(OBJ_DIR)diff_optimization.o


