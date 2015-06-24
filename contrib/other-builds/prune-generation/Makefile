all: prune-generation

clean: 
	rm -f *.o prune-generation

.cpp.o:
	g++ -std=c++0x -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = Main.o

prune-generation: $(OBJECTS)
	g++ $(OBJECTS)  -L../../../boost/lib64  -lz -lboost_program_options -lboost_filesystem -lboost_system -o prune-generation


