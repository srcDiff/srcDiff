##
# Makefile

all: ExtractSource #SwitchDifferences
	./test_suite.py

ExtractSource: ExtractSource.o SAX2ExtractSource.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

ExtractSource.o: ExtractSource.cpp
	g++ -c $< -o $@ 

SAX2ExtractSource.o: SAX2ExtractSource.cpp SAX2ExtractSource.hpp
	g++ -c $< -o $@ 

SwitchDifferences: SwitchDifferences.o SAX2SwitchDifferences.o
	g++ -I /usr/include/libxml2 -lxml2 $^ -o $@ 

SwitchDifferences.o: SwitchDifferences.cpp
	g++ -c $< -o $@ 

SAX2SwitchDifferences.o: SAX2SwitchDifferences.cpp SAX2SwitchDifferences.hpp
	g++ -c $< -o $@ 


clean:
	rm -f ExtractSource ExtractSource.o SwitchDifferences SwitchDifferences.o