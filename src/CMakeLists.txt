###
#    CMakeLists.txt
#
#
#    Optional cmake file for srcML.
#    The main way to build srcML is with normal Makefile

cmake_minimum_required(VERSION 2.8)
project(srcDiff)

# get version and revision
execute_process(COMMAND cat VERSION COMMAND tr -d "\n" OUTPUT_VARIABLE VERSION)
execute_process(COMMAND cat REVISION COMMAND tr -d "\n" OUTPUT_VARIABLE REVISION)
add_definitions(-DVERSION=\"${VERSION}\")
add_definitions(-DREVISION=\"${REVISION}\")

# get needed packages and include
find_package(LibXml2)
find_package(LibArchive)
find_package(LibXslt)
include_directories(${LIBXML2_INCLUDE_DIR} ${LibArchive_INCLUDE_DIRS} ${LIBXSLT_INCLUDE_DIR})

# define needed programs
find_program(ANTLR NAMES antlr runantlr cantlr antlr2 PATHS /usr/bin /opt/local/bin /usr/local/bin)
find_program(SED NAMES gsed sed PATHS /opt/local/bin /usr/local /bin)
find_program(GREP grep PATHS /bin /usr/bin)
find_program(ECHO echo PATH /bin)
find_program(CUT cut PATH /usr/bin)
find_program(SVNVERSION svnversion PATHS /usr/local/bin /usr/bin)

# src2srcml
add_executable(srcdiff srcdiff.cpp ColorDiff.cpp srcDiffChange.cpp srcDiffMatch.cpp srcDiffSingle.cpp LineDiffRange.cpp srcDiffCommentDiff.cpp srcDiffMeasure.cpp srcDiffTranslator.cpp srcdiff_options.cpp SAX2ColorDiff.cpp srcDiffCommon.cpp srcDiffMove.cpp srcDiffUtility.cpp srcdiffdirect.cpp ShortestEditScript.cpp srcDiffDiff.cpp srcDiffNested.cpp  srcDiffWhiteSpace.cpp svn_io.cpp mingw32.cpp srcDiffMany.cpp srcDiffOutput.cpp srcMLUtility.cpp xmlrw.cpp)
target_link_libraries(srcdiff archive xml2 antlr srcml)
