all: tok-align

clean: 
	rm -f *.o tok-align

.cpp.o:
	g++ -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = Main.o

tok-align: $(OBJECTS)
	g++ $(OBJECTS)  -L../../../boost/lib64  -lz -lboost_program_options -lboost_filesystem -lboost_system -o tok-align


