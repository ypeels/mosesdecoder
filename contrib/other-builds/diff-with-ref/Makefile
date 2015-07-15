all: diff-with-ref

clean: 
	rm -f *.o diff-with-ref

.cpp.o:
	g++ -std=c++11 -I../../../boost/include -I../../../ -O3 -g -c $<

OBJECTS = Main.o

diff-with-ref: $(OBJECTS)
	g++ $(OBJECTS) -std=c++11 -L../../../boost/lib64 -L../../../lib  -lz -lmoses  -o diff-with-ref


