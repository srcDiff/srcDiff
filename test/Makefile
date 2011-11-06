##
# Makefile

SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=bin/


all: $(BIN_DIR)SwitchDifferences

$(BIN_DIR)SwitchDifferences: $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

$(OBJ_DIR)SwitchDifferences.o: $(SRC_DIR)SwitchDifferences.cpp ../src/xmlrw.h
	g++ -I /usr/include/libxml2 -c $< -o $@ 

$(OBJ_DIR)xmlrw.o: $(SRC_DIR)xmlrw.cpp ../src/xmlrw.h
	g++ -I /usr/include/libxml2 -lxml2 -c $< -o $@ 

clean:
	rm -f $(BIN_DIR)SwitchDifferences $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o