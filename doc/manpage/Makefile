## SPDX-License-Identifier: GPL-3.0-only
#
# @file Makefile
#
# @copyright Copyright (C) 2012-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

MAN_PAGES= srcdiff.1
HTML_PAGES = srcdiff.html
MAN_GZIP= srcdiff.1.gz
SRCML_DIR=${firstword ${wildcard /home/srcml/srcML /srcML /Users/srcml/srcML }}
SRC2SRCML=${SRCML_DIR}/bin/src2srcml
SRCML2SRC=${SRCML_DIR}/bin/srcml2src

DOCBOOK2XMAN=${firstword ${wildcard /usr/bin/db2x_docbook2man /usr/bin/docbook2x-man /opt/local/bin/docbook2man /usr/local/bin/docbook2man}}
MAN2HTML=${firstword ${wildcard /usr/bin/man2html /opt/local/bin/man2html} /usr/local/bin/man2html}
MAN=${firstword ${wildcard /opt/local/bin/man /usr/bin/man}}

ECHO=/bin/echo

all : man srcdiff.html srcdiff.1.gz

man : ${MAN_PAGES}

srcdiff.1 : srcdiff.xml option.dtd option.dtd
	${DOCBOOK2XMAN} srcdiff.xml

view : man
	man ./srcdiff.1

srcdiff.1.gz : srcdiff.1
	cp srcdiff.1 temp
	gzip -f srcdiff.1
	mv temp srcdiff.1

srcdiff.html : srcdiff.1
	${MAN2HTML} < $<| tail -n +3 > $@

OPTION_SRC= ../src/srcdiff.cpp ../src/srcdiff_options.hpp ${SRCML_DIR}/src/srcmlapps.hpp ${SRCML_DIR}/src/srcmlns.hpp
option.dtd : ${OPTION_SRC}
	${ECHO} -n "<!ENTITY DATE \"" > option.dtd
	date | perl -p -e "s|\n||" >> option.dtd
	${ECHO} "\" >" >> option.dtd
	${SRC2SRCML} ${OPTION_SRC} | ${SRCML2SRC} --xslt=extract_options.xsl >> option.dtd
	${SRC2SRCML} --literal ${OPTION_SRC} | ${SRCML2SRC} --xslt=extract_return_status.xsl - >> option.dtd

gzip : srcdiff.1.gz

install : ${MAN_GZIP}
	cp srcdiff.1.gz /usr/share/man/man1/.

clean :
	rm -f ${MAN_PAGES} ${MAN_GZIP} option.dtd ${HTML_PAGES}
