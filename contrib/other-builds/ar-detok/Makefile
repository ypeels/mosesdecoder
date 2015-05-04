all: ar-detok

clean: 
	rm -f *.o ar-detok

.cpp.o:
	g++ -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = Main.o

ar-detok: $(OBJECTS)
	g++ $(OBJECTS)  -L../../../boost/lib64  -lz -lboost_program_options -lboost_filesystem -lboost_system -o ar-detok


