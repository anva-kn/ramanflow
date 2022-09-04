##
# Makefile for Ramanflow project
#
# @version 0.1
# For the next time add compilation of models.


CXX = g++
CFLAGS = --std=c++17
WFLAGS = -O3 -Wall -shared
LBS = -I /extern/pybind11/include

clean: 
	rm -rf 

background: background.o 
	$(CXX) background.o -o background

background.o: backgound.hpp background.cpp 
	$(CXX) -c background.hpp background.cpp -o background.o

