##
# Makefile

OBJ_DIR=obj/
BIN_DIR=bin/


all: $(BIN_DIR)ExtractSource $(BIN_DIR)SwitchDifferences
	./test_suite.py

$(BIN_DIR)ExtractSource: $(OBJ_DIR)ExtractSource.o $(OBJ_DIR)SAX2ExtractSource.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

$(OBJ_DIR)ExtractSource.o: ExtractSource.cpp
	g++ -I /usr/include/libxml2 -c $< -o $@ 

$(OBJ_DIR)SAX2ExtractSource.o: SAX2ExtractSource.cpp SAX2ExtractSource.hpp
	g++ -I /usr/include/libxml2 -lxml2 -c $< -o $@ 

$(BIN_DIR)SwitchDifferences: $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

$(OBJ_DIR)SwitchDifferences.o: SwitchDifferences.cpp xmlrw.h
	g++ -I /usr/include/libxml2 -c $< -o $@ 

$(OBJ_DIR)xmlrw.o: xmlrw.cpp xmlrw.h
	g++ -I /usr/include/libxml2 -lxml2 -c $< -o $@ 

clean:
	rm -f $(BIN_DIR)ExtractSource $(OBJ_DIR)ExtractSource.o $(OBJ_DIR)SAX2ExtractSource.o $(BIN_DIR)SwitchDifferences $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o