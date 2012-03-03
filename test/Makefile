##
# Makefile

SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=bin/

INCLUDES=-I /usr/include/libxml2 -I ../../../src/

all: $(BIN_DIR)SwitchDifferences

$(BIN_DIR)SwitchDifferences: $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o
	g++ $(INCLUDES) -lxml2 $^ -o $@ 

$(OBJ_DIR)SwitchDifferences.o: $(SRC_DIR)SwitchDifferences.cpp ../src/xmlrw.hpp
	g++ $(INCLUDES) -c $< -o $@ 

$(OBJ_DIR)xmlrw.o: ../src/xmlrw.cpp ../src/xmlrw.hpp
	g++ $(INCLUDES) -c $< -o $@ 

clean:
	rm -f $(BIN_DIR)SwitchDifferences $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o