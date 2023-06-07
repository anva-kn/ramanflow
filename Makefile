##
# Makefile for Ramanflow project
#
# @version 0.1
# For the next time add compilation of models.


CXX = g++
SHELL = /bin/sh
CFLAGS = --std=c++17
WFLAGS = -O3 -Wall -shared
LIB = -I./extern/pybind11/include \
      -I/opt/intel/oneapi/mkl/latest/include \

# Building the models module

models:

#background: background.o
#	$(CXX) $^ -o $@
#
#background.o: backgound.hpp background.cpp
#	$(CXX) -c $^ -o $@
#
#peak:
#
#
#peak_cluster:
#
#
#spectrum_component:

# Building the utils module

utils:

#fit: 
#
#
#importance:
#
#
#loss:

# Run tests

test:


# Cleaning up 
clean:
	rm -rf

#comp=g++
#
#comp_flags=-O3 -Wall -shared -std=c++17 -fPIC
#
#include_dirs=-I/usr/include/pybind11/ \
#		-I/usr/include/python3.10/ \
#		-Iusr/include/eigen3/ \
#	       -I./
#
#defaults:
#	${comp} ${comp_flags} $$(python3 -m pybind --includes) poly1d.cpp fit.cpp -o fit$$(python3-config --extension-suffix) ${include_dirs}
#	# ${comp} ${comp_flags} poly1d.cpp loss.cpp -o loss
