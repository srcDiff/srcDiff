###
#    CMakeLists.txt
#
#
#    Optional cmake file for srcML.
#    The main way to build srcML is with normal Makefile

cmake_minimum_required(VERSION 2.8)
project(srcDiff)

cmake_policy(SET CMP0015 OLD)

set_source_files_properties(shortest_edit_script.c PROPERTIES LANGUAGE CXX)

# get version and revision
execute_process(COMMAND cat VERSION COMMAND tr -d "\n" OUTPUT_VARIABLE VERSION)
execute_process(COMMAND cat REVISION COMMAND tr -d "\n" OUTPUT_VARIABLE REVISION)
add_definitions(-DVERSION=\"${VERSION}\")
add_definitions(-DREVISION=\"${REVISION}\")

# get needed packages and include
find_package(LibXml2)
find_package(LibArchive)
find_package(LibXslt)

#find_file(LIBSVN_INCLUDE_DIR subversion-1 PATHS /usr/include)
#find_file(LIBAPR_INCLUDE_DIR NAMES apr-1 apr-1.0 PATHS /usr/include)

#include_directories(${LIBXML2_INCLUDE_DIR} ${LibArchive_INCLUDE_DIRS} ${LIBXSLT_INCLUDE_DIR} ${LIBAPR_INCLUDE_DIR} ${LIBSVN_INCLUDE_DIR} ../../../../../srcML/src)
include_directories(${LIBXML2_INCLUDE_DIR} ${LibArchive_INCLUDE_DIRS} ${LIBXSLT_INCLUDE_DIR} ../../../../../srcML/src)
link_directories(../../../../../srcML/bin)

if(EXISTS /usr/include/i386-linux-gnu)

          link_directories(/usr/lib/i386-linux-gnu)

endif()

# define needed programs
find_program(ANTLR NAMES antlr runantlr cantlr antlr2 PATHS /usr/bin /opt/local/bin /usr/local/bin)
find_program(SED NAMES gsed sed PATHS /opt/local/bin /usr/local /bin)
find_program(GREP grep PATHS /bin /usr/bin)
find_program(ECHO echo PATH /bin)
find_program(CUT cut PATH /usr/bin)
find_program(SVNVERSION svnversion PATHS /usr/local/bin /usr/bin)

# srcdiff
add_executable(srcdiff srcdiff.cpp ColorDiff.cpp srcDiffChange.cpp srcDiffMatch.cpp srcDiffSingle.cpp LineDiffRange.cpp srcDiffCommentDiff.cpp srcDiffMeasure.cpp srcDiffTranslator.cpp srcdiff_options.cpp SAX2ColorDiff.cpp srcDiffCommon.cpp srcDiffMove.cpp srcDiffUtility.cpp ShortestEditScript.cpp srcDiffDiff.cpp srcDiffNested.cpp  srcDiffWhiteSpace.cpp srcDiffMany.cpp srcDiffOutput.cpp srcMLUtility.cpp xmlrw.cpp shortest_edit_script.c)
target_link_libraries(srcdiff archive xml2 antlr srcml)
