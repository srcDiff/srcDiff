MAN_PAGES= src2diff.1
HTML_PAGES = srcdiff.html
MAN_GZIP= srcdiff.1.gz
SRC2SRCML=../bin/src2srcml
SRCML2SRC=../bin/srcml2src

DOCBOOK2XMAN=${firstword ${wildcard /usr/bin/db2x_docbook2man /usr/bin/docbook2x-man /opt/local/bin/docbook2man /usr/local/bin/docbook2man}}
MAN2HTML=${firstword ${wildcard /usr/bin/man2html /opt/local/bin/man2html} /usr/local/bin/man2html}
MAN=${firstword ${wildcard /opt/local/bin/man /usr/bin/man}}

ECHO=/bin/echo

all : man gzip

man : ${MAN_PAGES}

src2srcml.1 : srcdiff.xml option.dtd option.dtd
	${DOCBOOK2XMAN} srcdiff.xml

view : man
	man ./srcdiff.1

src2srcml.1.gz : srcdiff.xml
	gzip -f srcdiff.1

src2srcml.html : src2srcml.1
	${MAN2HTML} < $<| tail -n +3 > $@

srcml2src.html : srcml2src.1
	${MAN2HTML} < $< | tail -n +3 > $@

OPTION_SRC= ../src/src2srcml.cpp ../src/srcml2src.cpp ../src/srcmlapps.hpp ../src/srcmlns.hpp
option.dtd : ${OPTION_SRC}
	${ECHO} -n "<!ENTITY DATE \"" > option.dtd
	date | perl -p -e "s|\n||" >> option.dtd
	${ECHO} "\" >" >> option.dtd
	${SRC2SRCML} ${OPTION_SRC} | ${SRCML2SRC} --xslt=extract_options.xsl >> option.dtd
	${SRC2SRCML} --literal ${OPTION_SRC} | ${SRCML2SRC} --xslt=extract_return_status.xsl - >> option.dtd

gzip : src2srcml.1.gz srcml2src.1.gz

install : ${MAN_GZIP}
	cp src2srcml.1.gz srcml2src.1.gz /usr/share/man/man1/.

clean :
	rm -f ${MAN_PAGES} ${MAN_GZIP} option.dtd ${HTML_PAGES}
