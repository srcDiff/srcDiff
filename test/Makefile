##
# Makefile

SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=bin/


all: $(BIN_DIR)ExtractSource $(BIN_DIR)ExtractsrcML $(BIN_DIR)SwitchDifferences

$(BIN_DIR)ExtractSource: $(OBJ_DIR)ExtractSource.o $(OBJ_DIR)SAX2ExtractSource.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

$(OBJ_DIR)ExtractSource.o: $(SRC_DIR)ExtractSource.cpp
	g++ -I /usr/include/libxml2 -c $< -o $@ 

$(BIN_DIR)ExtractsrcML: $(OBJ_DIR)ExtractsrcML.o $(OBJ_DIR)SAX2ExtractsrcML.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

$(OBJ_DIR)ExtractsrcML.o: $(SRC_DIR)ExtractsrcML.cpp
	g++ -I /usr/include/libxml2 -c $< -o $@ 

$(OBJ_DIR)SAX2ExtractSource.o: $(SRC_DIR)SAX2ExtractSource.cpp $(SRC_DIR)SAX2ExtractSource.hpp
	g++ -I /usr/include/libxml2 -lxml2 -c $< -o $@ 


$(OBJ_DIR)SAX2ExtractsrcML.o: $(SRC_DIR)SAX2ExtractsrcML.cpp $(SRC_DIR)SAX2ExtractsrcML.hpp
	g++ -I /usr/include/libxml2 -lxml2 -c $< -o $@ 

$(BIN_DIR)SwitchDifferences: $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

$(OBJ_DIR)SwitchDifferences.o: $(SRC_DIR)SwitchDifferences.cpp ../src/xmlrw.h
	g++ -I /usr/include/libxml2 -c $< -o $@ 

$(OBJ_DIR)xmlrw.o: $(SRC_DIR)xmlrw.cpp ../src/xmlrw.h
	g++ -I /usr/include/libxml2 -lxml2 -c $< -o $@ 

clean:
	rm -f $(BIN_DIR)ExtractSource $(BIN_DIR)ExtractsrcML $(OBJ_DIR)ExtractSource.o $(OBJ_DIR)SAX2ExtractSource.o $(OBJ_DIR)ExtractsrcML.o $(OBJ_DIR)SAX2ExtractsrcML.o $(BIN_DIR)SwitchDifferences $(OBJ_DIR)SwitchDifferences.o $(OBJ_DIR)xmlrw.o