all: diff-with-ref

clean: 
	rm -f *.o diff-with-ref

.cpp.o:
	g++ -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = Main.o

diff-with-ref: $(OBJECTS)
	g++ -lz -lbz2 $(OBJECTS)  -L../../../boost/lib -L../../../lib -o diff-with-ref


