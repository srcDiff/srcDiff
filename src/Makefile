# set compiler version to match that of antlr library
CXX=/usr/bin/g++

# associated text tools
SED=${firstword ${wildcard /opt/local/bin/gsed /bin/sed}}
GREP=${firstword ${wildcard /bin/grep /usr/bin/grep}}
ECHO=/bin/echo
CUT=/usr/bin/cut

VERSION="\"Trunk $(shell svnversion -n)$(shell svn info | ${GREP} 'Last Changed Date:' | ${CUT} -d: -f2-)\""

# compiler optimizations
CXXFLAGS = -DVERSION=$(VERSION) -O3 -Wall -march=nocona -minline-all-stringops #-g -fno-rtti

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
ifdef DIST
LIBSRCML_INCLUDE = -I ../../srcml-src/
else
LIBSRCML_INCLUDE = -I ../../../src
endif

ifdef MINGW32
LIBSRCML_LIB = ../../../bin/libsrcml.dll
else
LIBSRCML_LIB = -L ../bin/ -lsrcml
LIBSRCMLC_LIB = -L ../../../bin/ -lsrcml
endif

# libarchive
XML_IO_LIB=-larchive
IO_LIB_DEPENDS=libxml_archive_read.hpp libxml_archive_write.hpp
IO_LIB=../../../obj/libxml_archive_read.o ../../../obj/libxml_archive_write.o

# project dirs
EXE_DIR = ../bin/
OBJ_DIR = ../obj/

ifdef MINGW32
EXE = $(EXE_DIR)srcdiff.exe
else
EXE = $(EXE_DIR)srcdiffdirect $(EXE_DIR)srcdiff $(EXE_DIR)colordiffc
endif


all : ${EXE}

mingw32 : 
	make CXX=/usr/bin/i686-pc-mingw32-g++ MINGW32=1

$(OBJ_DIR)srcDiffTranslator.o : srcDiffTranslator.cpp srcDiffTranslator.hpp srcDiffTypes.hpp srcMLUtility.hpp srcDiffUtility.hpp srcDiffWhiteSpace.hpp srcDiffCommon.hpp shortest_edit_script.h xmlrw.h
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) $(LIBSRCML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcdiffdirect.o : srcdiffdirect.cpp
	$(CXX) $(CXXFLAGS)$(LIBXML_INCLUDE) $(LIBSRCML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcdiff.o : srcdiff.cpp 
	$(CXX) $(CXXFLAGS)$(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcMLUtility.o : srcMLUtility.cpp srcMLUtility.hpp
	$(CXX) $(ANTLR_INC) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffUtility.o : srcDiffUtility.cpp srcDiffUtility.hpp shortest_edit_script.h
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffTool.o : srcDiffTool.cpp srcDiffTool.hpp
	$(CXX) $(ANTLR_INC) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffWhiteSpace.o : srcDiffWhiteSpace.cpp srcDiffWhiteSpace.hpp srcDiffCommon.hpp srcDiffOutput.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffCommon.o : srcDiffCommon.cpp srcDiffCommon.hpp srcDiffTypes.hpp srcDiffUtility.hpp shortest_edit_script.h
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffChange.o : srcDiffChange.cpp srcDiffChange.hpp srcDiffTypes.hpp srcDiffUtility.hpp shortest_edit_script.h
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffNested.o : srcDiffNested.cpp srcDiffNested.hpp srcDiffTypes.hpp srcDiffUtility.hpp shortest_edit_script.h
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffOutput.o : srcDiffOutput.cpp srcDiffOutput.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffCommentDiff.o : srcDiffCommentDiff.cpp srcDiffCommentDiff.hpp srcDiffCommon.hpp srcDiffOutput.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)srcDiffDiff.o : srcDiffDiff.cpp srcDiffDiff.hpp srcDiffCommon.hpp srcDiffOutput.hpp srcDiffTypes.hpp srcDiffUtility.hpp
	$(CXX) $(CXXFLAGS) $(LIBSRCML_INCLUDE) $(LIBXML_INCLUDE) -c $< -o $@

$(EXE_DIR)srcdiffdirect : srcdiffdirect.cpp $(OBJ_DIR)srcMLUtility.o $(OBJ_DIR)srcDiffUtility.o $(OBJ_DIR)srcDiffWhiteSpace.o $(OBJ_DIR)srcDiffCommon.o $(OBJ_DIR)srcDiffChange.o $(OBJ_DIR)srcDiffNested.o $(OBJ_DIR)srcDiffChange.o $(OBJ_DIR)srcDiffOutput.o $(OBJ_DIR)srcDiffDiff.o $(OBJ_DIR)srcDiffCommentDiff.o $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o $(OBJ_DIR)srcDiffTool.o
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $(LIBXML_INCLUDE) -I ../../../src -I/opt/local/include -L../../../obj $^ $(LIBSRCMLC_LIB) $(LDFLAGS) ${XML_LIB_SRCDIFF} ${XML_IO_LIB} ../../../obj/srcmlapps.o -o $@

$(EXE_DIR)srcdiff : srcdiff.cpp $(OBJ_DIR)srcMLUtility.o $(OBJ_DIR)srcDiffUtility.o $(OBJ_DIR)srcDiffWhiteSpace.o $(OBJ_DIR)srcDiffCommon.o $(OBJ_DIR)srcDiffChange.o $(OBJ_DIR)srcDiffNested.o $(OBJ_DIR)srcDiffChange.o $(OBJ_DIR)srcDiffOutput.o $(OBJ_DIR)srcDiffDiff.o $(OBJ_DIR)srcDiffCommentDiff.o $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o $(OBJ_DIR)srcDiffTool.o
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $(LIBXML_INCLUDE) -I ../../../src -I/opt/local/include -L../../../obj $^ $(LIBSRCMLC_LIB) $(LDFLAGS) ${XML_LIB_SRCDIFF} ${XML_IO_LIB} ../../../obj/srcmlapps.o -o $@

$(EXE_DIR)srcdiff.exe :  srcdiff.cpp $(OBJ_DIR)srcMLUtility.o $(OBJ_DIR)srcDiffUtility.o $(OBJ_DIR)srcDiffWhiteSpace.o $(OBJ_DIR)srcDiffCommon.o $(OBJ_DIR)srcDiffChange.o $(OBJ_DIR)srcDiffNested.o $(OBJ_DIR)srcDiffChange.o $(OBJ_DIR)srcDiffOutput.o $(OBJ_DIR)srcDiffDiff.o $(OBJ_DIR)srcDiffCommentDiff.o $(OBJ_DIR)shortest_edit_script.o $(OBJ_DIR)xmlrw.o $(OBJ_DIR)srcDiffTool.o $(OBJ_DIR)mingw32.o
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $(LIBXML_INCLUDE) -I ../../../src -I/opt/local/include -L../../../obj $^ $(LIBSRCML_LIB) $(LDFLAGS) ${XML_LIB_SRCDIFF} ${XML_IO_LIB} ../../../obj/srcmlapps.o -o $@

$(OBJ_DIR)mingw32.o : mingw32.cpp mingw32.hpp
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)xmlrw.o : xmlrw.cpp xmlrw.hpp
	$(CXX) $(CXXFLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)shortest_edit_script.o : shortest_edit_script.c shortest_edit_script.h
	${CXX} $(CXXFLAGS) -c $< -o $@

$(EXE_DIR)colordiffc : $(OBJ_DIR)colordiff.o $(OBJ_DIR)SAXColorDiff.o
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $(XML_LIB_SRCDIFF) $^ -o $@

$(OBJ_DIR)colordiff.o : colordiff.cpp SAX2ColorDiff.hpp
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $(LIBXML_INCLUDE) -c $< -o $@

$(OBJ_DIR)SAXColorDiff.o :  SAX2ColorDiff.cpp SAX2ColorDiff.hpp
	$(CXX) $(CXXFLAGS) $(CXX_LINK_FLAGS) $(LIBXML_INCLUDE) -c $< -o $@

clean :
	rm -f ${EXE} $(OBJ_DIR)*


