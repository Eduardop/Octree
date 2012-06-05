all: sample

sample: sample.cpp Octree.h
	g++ -O3 -o sample sample.cpp
    
clean:
	rm -f sample sample.o
