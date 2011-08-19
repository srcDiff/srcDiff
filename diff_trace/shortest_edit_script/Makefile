##
# Build shortest_edit_script functionality

EXE=shortest_edit_script_t shortest_edit_script_u
LIBS=libdiff.so
OBJS=shortest_edit_script.o shortest_edit_script_u.o shortest_edit_script_t.o

CC=g++
CCOPTS=-g -Wall

all : ${EXE} fcomp shortest_edit_script_length diff_mba_srcML

shortest_edit_script_t : shortest_edit_script_t.o libdiff.a
	${CC} ${CCOPTS} $^ -o $@

shortest_edit_script_t.o : shortest_edit_script_t.c shortest_edit_script.h shortest_edit_script.c
	g++ ${CCOPTS} -c $<

shortest_edit_script_u : shortest_edit_script_u.o libdiff.a
	g++ $^ ${CCOPTS} -o $@

shortest_edit_script_u.o : shortest_edit_script_u.c shortest_edit_script.h
	${CC} ${CCOPTS} -c $<

shortest_edit_script.o : shortest_edit_script.c shortest_edit_script.h
	${CC} ${CCOPTS} -fPIC -c $<

fcomp : fcomp.c
	gcc $^ -o $@

shortest_edit_script_length : shortest_edit_script_length.c libdiff.a
	g++ $^ ${CCOPTS} -o $@

diff_mba_srcML : diff_mba_srcML.cpp
	g++ diff_mba_srcML.cpp -I/opt/local/include/ -L/opt/local/lib -lmba -o $@

${LIBS} : shortest_edit_script.o
	gcc -shared -fPIC ${CCOPTS} $^ -o ${LIBS}

libdiff.a : shortest_edit_script.o
	ar rvs $@ $<

# run general testing program
test : shortest_edit_script_t shortest_edit_script_u fcomp
	./shortest_edit_script_t
	./shortest_edit_script_test.py

test_user : shortest_edit_script_u
	./shortest_edit_script_u ../test/examples/timing_long.h ../test/examples/timing_long.new.h

# checking memory usage
checkmem : shortest_edit_script_t
	valgrind --leak-check=full ./shortest_edit_script_t

view: shortest_edit_script.xml
	docbook2man $^
	gzip -f shortest_edit_script.1
	man ./shortest_edit_script.1.gz

clean :
	rm -rf ${EXE} ${OBJS} ${LIBS} shortest_edit_script_length.dSYM libdiff.a fcomp shortest_edit_script.1.gz shortest_edit_script_length diff_mba_srcML
