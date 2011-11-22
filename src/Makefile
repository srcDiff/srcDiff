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

DLL_DIR=../dlls
# libxml
LIBXML_INCLUDE=-I/usr/include/libxml2

ifndef MINGW32
XML_LIB_SRCDIFF=-lxml2
else
XML_LIB_SRCDIFF=${DLL_DIR}/libxml2.dll -lregex -liberty
endif

#libsrcml include
LIBSRCML_INCLUDE = -I ../include/
ifdef MINGW32
LIBSRCML_LIB = ${DLL_DIR}/libsrcml.dll
else
LIBSRCML_LIB = -L ../bin/ -lsrcml
endif


# project dirs
EXE_DIR = ../bin/
OBJ_DIR = ../obj/

ifdef MINGW32
EXE = $(EXE_DIR)srcdiff.exe
else
EXE = $(EXE_DIR)srcdiff
endif


all : ${EXE}

mingw32 : 
	make CXX=/usr/bin/i686-pc-mingw32-g++ MINGW32=1

$(OBJ_DIR)srcdiff.o : srcdiff.cpp srcDiffTypes.hpp srcDiffUtility.hpp srcDiffWhiteSpace.hpp srcDiffCommon.hpp shortest_edit_script.h xmlrw.h
	$(CXX) $(ANTLR_INC) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffUtility.o : srcDiffUtility.cpp srcDiffUtility.hpp shortest_edit_script.h
	$(CXX) $(ANTLR_INC) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffWhiteSpace.o : srcDiffWhiteSpace.cpp srcDiffWhiteSpace.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffCommon.o : srcDiffCommon.cpp srcDiffCommon.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffOutput.o : srcDiffOutput.cpp srcDiffOutput.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(EXE_DIR)srcdiff : $(OBJ_DIR)srcdiff.o $(OBJ_DIR)srcDiffUtility.o $(OBJ_DIR)srcDiffWhiteSpace.o $(OBJ_DIR)srcDiffCommon.o $(OBJ_DIR)srcDiffCommon.o $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o 
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $^ $(LIBSRCML_LIB) $(LDFLAGS) ${XML_LIB_SRCDIFF} ${XML_IO_LIB} -o $@

$(EXE_DIR)srcdiff.exe : $(OBJ_DIR)srcdiff.o $(OBJ_DIR)srcDiffUtility.o $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o 
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $^ $(LIBSRCML_LIB) $(LDFLAGS) ${XML_LIB_SRCDIFF} ${XML_IO_LIB} -o $@

$(OBJ_DIR)xmlrw.o : xmlrw.cpp xmlrw.h
	${CXX} -c xmlrw.cpp -O3 $(LIBXML_INCLUDE) -o $@

$(OBJ_DIR)shortest_edit_script.o : shortest_edit_script.c shortest_edit_script.h
	${CXX} -c $< -o $@

clean :
	rm -f ${EXE} $(OBJ_DIR)*


