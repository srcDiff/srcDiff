##
# Makefile for building srcdiff distributions
#

# tools
ECHO=${firstword ${wildcard /opt/local/bin/gecho /bin/echo}}
SED=${firstword ${wilcard /opt/local/bin/gsed /bin/sed}}
GREP=${firstword ${wildcard /bin/grep /usr/bin/grep}}
CUT=/usr/bin/cut
TR=/usr/bin/tr
CP=${firstword ${wildcard /opt/local/bin/gcp /bin/cp}}

# svn repo
SVN_URL=http://calder.sdml.cs.kent.edu/svn/srcML/trunk/srcdiff/trunk

# file name base
BASE=srcdiff
WIN_BASE=${BASE}-Win

# generated compressed files
SRC_TAR=${BASE}-src.tar
SRC_TAR_GZ=${SRC_TAR}.gz
SRC_ZIP=${BASE}-src.zip

UNIX_TAR_GZ=${BASE}.tar.gz
WIN_ZIP=${BASE}-Win.zip

# directories
SRC_DIR_BASE=${BASE}-src
SRC_DIR=${SRC_DIR_BASE}/src
OBJ_DIR=${SRC_DIR_BASE}/obj
BIN_DIR=${SRC_DIR_BASE}/bin
DOC_DIR=${SRC_DIR_BASE}/doc
WINBIN_DIR=${BASE}-windows
DLL_DIR=../../../bin
SRCML_SRC=../../../src

# srcml library
LIBSRCML_DLL=${DLL_DIR}/libsrcml.dll

# executables
SRCDIFF_BIN=srcdiff
SRCDIFF_EXE=srcdiff.exe
SRCDIFF_PATH=${BIN_DIR}/${SRCDIFF_BIN}
WIN_SRC2SRCML_PATH=${BIN_DIR}/${SRCDIFF_EXE}

# doc
#DOC_FILES=${DOC_DIR}/src2srcml.1.gz ${DOC_DIR}/srcml2src.1.gz ${DOC_DIR}/src2srcml.html ${DOC_DIR}/srcml2src.html
#WIN_DOC_FILES=${DOC_DIR}/src2srcml.html ${DOC_DIR}/srcml2src.html
DIST_FILES=${SRCDIFF_PATH} 
WIN_DIST_FILES=${WIN_SRCDIFF_PATH} ${DLL_DIR}/*.dll

# generate files
all : src bin

REVISION=$(strip $(shell svn info ${SVN_URL}/src | ${GREP} 'Revision:' | ${CUT} -d: -f2-))

# export the directories
exp :
	@echo ${REVISION}
	make cleansrc
	mkdir ${SRC_DIR_BASE}
	svn export -r${REVISION} ${SVN_URL}/src/ ${SRC_DIR_BASE}/src
	${ECHO} -n "Trunk ${REVISION}" > ${SRC_DIR_BASE}/src/VERSION
	svn info -r${REVISION} ${SVN_URL}/src | ${GREP} 'Last Changed Date:' | ${CUT} -d: -f2- | ${TR} -d '\n' >> ${SRC_DIR_BASE}/src/VERSION
	svn export -r${REVISION} --depth='empty' ${SVN_URL}/obj/ ${SRC_DIR_BASE}/obj --native-eol LF
	svn export -r${REVISION} --depth='empty' ${SVN_URL}/bin/ ${SRC_DIR_BASE}/bin --native-eol LF

clean :
	make cleansrc
	rm -fR  ${SRC_TAR_GZ} ${UNIX_TAR_GZ} ${SRC_ZIP} ${WIN_ZIP}

cleansrc :
	rm -fR ${SRC_DIR_BASE} ${BASE} ${WIN_BASE}

##
# src

src : ${SRC_TAR_GZ} ${SRC_ZIP}

${SRC_TAR_GZ} : exp
	tar czf $@ ${SRC_DIR} ${OBJ_DIR} ${BIN_DIR}
	gunzip --test $@

${SRC_ZIP} : exp
	zip -orql $@ ${SRC_DIR} ${OBJ_DIR} ${BIN_DIR}
	unzip -qt $@

##
# bin
bin : ${SRC_TAR_GZ}
	tar xzf ${SRC_TAR_GZ}
	cd ${SRCML_SRC} && make clean
	cd ${SRCML_SRC} && make DYNAMIC=1
	cd ${SRC_DIR} && make
	strip ${SRCDIFF_PATH}
	mkdir ${BASE} && ${CP} -t ${BASE} ${DIST_FILES}
	tar czf ${UNIX_TAR_GZ} ${BASE}

windows : ${SRC_ZIP}
	unzip -aq ${SRC_ZIP}
	cd ${SRC_DIR} && make mingw32
	cd ${SRC_DIR} && make cleanbuild && make
	cd ${DOC_DIR} && make
	i686-pc-mingw32-strip ${WIN_SRC2SRCML_PATH} ${WIN_SRCML2SRC_PATH}
	mkdir ${WIN_BASE} && ${CP} -t ${WIN_BASE} ${WIN_DIST_FILES}
	unix2dos ${WIN_BASE}/*.html
	zip -oql ${WIN_ZIP} ${WIN_BASE}/src2srcml.exe ${WIN_BASE}/srcml2src.exe ${WIN_BASE}/*.dll ${WIN_BASE}/src2srcml.html ${WIN_BASE}/srcml2src.html

windowsdyn : ${SRC_ZIP}
	unzip -aq ${SRC_ZIP}
	cd ${SRCML_SRC} && make mingw32 DYNAMIC=1
	cd ${SRC_DIR} && make mingw32
	i686-pc-mingw32-strip ${WIN_SRCDIFF_PATH} ${LIBSRCML_DLL}
	mkdir ${WIN_BASE} && ${CP} -t ${WIN_BASE} ${WIN_DIST_FILES}
	${CP} ${DLL_DIR}/*.dll ${WIN_BASE}
	zip -oql ${WIN_ZIP} ${WIN_BASE}/srcdiff.exe ${WIN_BASE}/*.dll 
