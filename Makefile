CXX=g++
CC=gcc
CXXFLAGS=-std=c++11 -O3 -g

all: route

route: route.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@

clean:
	rm -rf route *.exe *.o *.dSYM gmon.out *~

