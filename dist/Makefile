##
# Makefile for building srcML distributions
#

# tools
ECHO=${firstword ${wildcard /opt/local/bin/gecho /usr/local/bin/gecho /bin/echo}}
SED=${firstword ${wilcard /opt/local/bin/gsed /usr/local/bin/gecho /bin/sed}}
GREP=${firstword ${wildcard /bin/grep /usr/bin/grep}}
CUT=/usr/bin/cut
TR=/usr/bin/tr
CP=${firstword ${wildcard /opt/local/bin/gcp /usr/local/bin/gcp /bin/cp}}
STRIP=${firstword ${wildcard /usr/bin/i686-pc-mingw32-strip /usr/bin/i686-w64-mingw32-strip}}

# svn repo
SVN_URL=http://calder.sdml.cs.kent.edu/svn/srcDiffPurged/trunk

# git repo
GIT_URL=git@github.com:srcML/srcML.git

# file name base
BASE=srcDiff
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
DLL_DIR=../../../dlls

# executables
SRCDIFF_BIN=srcdiff
SRCDIFF_EXE=srcdiff.exe

SRCDIFF_PATH=${BIN_DIR}/${SRCDIFF_BIN}

WIN_SRC2SRCML_PATH=${BIN_DIR}/${SRCDIFF_EXE}
WIN_SRCML2SRC_PATH=${BIN_DIR}/${SRCDIFF_EXE}
LIBSRCML_BASE=${BIN_DIR}/libsrcml
LIBSRCML_SO=${LIBSRCML_BASE}.so
LIBSRCML_DYLIB=${LIBSRCML_BASE}.dylib
LIBSRCML_DLL=${LIBSRCML_BASE}.dll

DOC_FILES=${DOC_DIR}/srcdiff.1.gz ${DOC_DIR}/srcdiff.html
WIN_DOC_FILES=${DOC_DIR}/srcdiff.html
DIST_FILES=${DOC_FILES} ${SRCDIFF_PATH}
WIN_DIST_FILES=${WIN_DOC_FILES} ${WIN_SRC2SRCML_PATH} ${WIN_SRCML2SRC_PATH}

# generate files
all : src bin

REVISION=$(strip $(shell svn info ${SVN_URL}/src | ${GREP} 'Revision:' | ${CUT} -d: -f2-))
#GITREVISION=$(strip $(shell git log -n 1 --remotes --format="%H"))
GITREVISION=$(strip $(shell git log -n1 --format="%b" | cut -d " " -f 2 | cut -d "@" -f 2))

# export the directories
exp :
	@echo ${GITREVISION}
	make cleansrc
	mkdir ${SRC_DIR_BASE}
	cd ..; git archive master src doc | tar xv -C dist/${SRC_DIR_BASE}
	git log -n 1 --remotes --format="Trunk ${GITREVISION} %cd" > ${SRC_DIR_BASE}/src/VERSION
	${ECHO} "${GITREVISION}" > ${SRC_DIR_BASE}/src/REVISION
	mkdir ${SRC_DIR_BASE}/obj
	mkdir ${SRC_DIR_BASE}/bin

# export the directories
svnexp :
	@echo ${REVISION}
	make cleansrc
	mkdir ${SRC_DIR_BASE}
	svn export -r${REVISION} ${SVN_URL}/src/ ${SRC_DIR_BASE}/src
	${ECHO} -n "Trunk ${REVISION}" > ${SRC_DIR_BASE}/src/VERSION
	${ECHO} -n "${REVISION}" > ${SRC_DIR_BASE}/src/REVISION
	svn info -r${REVISION} ${SVN_URL}/src | ${GREP} 'Last Changed Date:' | ${CUT} -d: -f2- | ${TR} -d '\n' >> ${SRC_DIR_BASE}/src/VERSION
	mkdir ${SRC_DIR_BASE}/obj
	mkdir ${SRC_DIR_BASE}/bin
	svn export -r${REVISION} ${SVN_URL}/doc/ ${SRC_DIR_BASE}/doc --native-eol LF

clean :
	make cleansrc
	rm -fR  ${SRC_TAR_GZ} ${UNIX_TAR_GZ} ${SRC_ZIP} ${WIN_ZIP}

cleansrc :
	rm -fR ${SRC_DIR_BASE} ${BASE} ${WIN_BASE}

##
# src

src : ${SRC_TAR_GZ} ${SRC_ZIP}

${SRC_TAR_GZ} : svnexp
	tar czf $@ ${SRC_DIR} ${OBJ_DIR} ${BIN_DIR} ${DOC_DIR}
	gunzip --test $@

${SRC_ZIP} : svnexp
	zip -orql $@ ${SRC_DIR} ${OBJ_DIR} ${BIN_DIR} ${DOC_DIR}
	unzip -uq $@

##
# bin
bin : ${SRC_TAR_GZ}
	tar xzf ${SRC_TAR_GZ}
	cd ${SRC_DIR} && cmake . && make
	cd ${DOC_DIR} && make
	strip ${SRCDIFF_PATH}
	mkdir ${BASE} && ${CP} ${DIST_FILES} ${BASE}
	( test -e ${LIBSRCML_SO} && cp ${LIBSRCML_SO} ${BASE} ) || ( test -e ${LIBSRCML_DYLIB} && cp ${LIBSRCML_DYLIB} ${BASE} )
	tar czf ${UNIX_TAR_GZ} ${BASE}

windows : ${SRC_ZIP}
	unzip -uq ${SRC_ZIP}
	cd ${SRC_DIR} && mingw32-cmake . && make
	cd ${SRC_DIR} && rm -f CMakeCache.txt && cmake . && make
	cd ${DOC_DIR} && make
	i686-pc-mingw32-strip ${WIN_SRCDIFF_PATH}
	mkdir ${WIN_BASE} && ${CP} ${WIN_DIST_FILES} ${WIN_BASE}
	unix2dos ${WIN_BASE}/*.html
	zip -oql ${WIN_ZIP} ${WIN_BASE}/srcdiff.exe ${WIN_BASE}/*.dll ${WIN_BASE}/srcdiff.html

windowsdyn : ${SRC_ZIP}
	unzip -uq  ${SRC_ZIP}
	cd ${SRC_DIR} && mingw32-cmake . && make
	cd ${SRC_DIR} && rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake && cmake . && make
	cd ${DOC_DIR} && make clean && make
	${STRIP} ${WIN_SRCDIFF_PATH} ${LIBSRCML_DLL}
	mkdir ${WIN_BASE} && ${CP} ${WIN_DIST_FILES} ${WIN_BASE}
	${CP} ../dlls/*.dll ${WIN_BASE}
	${CP} ${BIN_DIR}/libsrcml.dll ${WIN_BASE}
	unix2dos ${WIN_BASE}/*.html
	zip -oql ${WIN_ZIP} ${WIN_BASE}/srcdiff.exe ${WIN_BASE}/*.dll ${WIN_BASE}/srcdiff.html

dll :
	echo ${DLL_DIR}
