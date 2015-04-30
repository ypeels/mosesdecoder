all: manual-label

clean: 
	rm -f *.o manual-label

.cpp.o:
	g++ -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = DeEn.o EnOpenNLPChunker.o EnPhrasalVerb.o Main.o LabelByInitialLetter.o ZonesAndWalls.o

manual-label: $(OBJECTS)
	g++ $(OBJECTS)  -L../../../boost/lib64  -lz -lboost_program_options -lboost_filesystem -lboost_system -o manual-label


