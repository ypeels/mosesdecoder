all: calc-oov

clean: 
	rm -f *.o calc-oov

.cpp.o:
	g++ -std=c++11 -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = Main.o Base.o Simple.o Compound.o Node.o

calc-oov: $(OBJECTS)
	g++ $(OBJECTS)  -L../../../boost/lib64  -lz -lboost_program_options -lboost_filesystem -lboost_system -o calc-oov


