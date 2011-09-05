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

#libsrcml include
LIBSRCML_INCLUDE = ../bin/src
LIBSRCML_LIB = ../bin/

# project dirs
EXE_DIR = ../bin/
OBJ_DIR = ../obj/

ifdef MINGW32
EXE = $(EXE_DIR)src2srcdiff.exe
else
EXE = $(EXE_DIR)src2srcdiff
endif


all : ${EXE}

mingw32 : 
	make CXX=/usr/bin/i686-pc-mingw32-g++ MINGW32=1

$(OBJ_DIR)src2srcdiff.o : src2srcdiff.cpp shortest_edit_script.h xmlrw.h diffrw.h
	$(CXX) $(ANTLR_INC) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(EXE_DIR)src2srcdiff : $(OBJ_DIR)src2srcdiff.o  $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o 
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $^ $(LIBSRCML_LIB)libsrcml.a $(LDFLAGS) ${XML_LIB_SRC2SRCDIFF} ${XML_IO_LIB} -o $@

$(EXE_DIR)src2srcdiff.exe : $(OBJ_DIR)src2srcdiff.o  $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o 
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $^ $(LIBSRCML_LIB)libsrcml.dll $(LDFLAGS) ${XML_LIB_SRC2SRCDIFF} ${XML_IO_LIB} -o $@

$(OBJ_DIR)xmlrw.o : xmlrw.cpp xmlrw.h
	g++ -c xmlrw.cpp -O3 $(LIBXML_INCLUDE) -o $@

$(OBJ_DIR)shortest_edit_script.o : shortest_edit_script.c shortest_edit_script.h
	g++ -c $< -o $@

clean :
	rm -f ${EXE} $(OBJ_DIR)*


