# Makefile for Ramanflow project
#
# @version 0.2.0
# For the next time add compilation of models.

CXX = g++
SHELL = /bin/sh
CFLAGS = --std=c++17
WFLAGS = -O3 -Wall -shared -fPIC

# Include directories
LIB = -I/extern/pybind11/include \
      -I/usr/bin/env python3
# include pybind11/pybind11.h
# Directory containing the models source files
MODELS_DIR = models
UTILS_DIR = utils
TESTS_DIR = tests
PYBIND_DIR = extern/pybind11

.PHONY: compile_models clean

compile_models:
	$(CXX) $(CFLAGS) $(WFLAGS) $(LIB) -c $(MODELS_DIR)/SpectrumComponentBbindings.cpp -o $(MODELS_DIR)/spectrum_component_bindings.o
	$(CXX) $(CFLAGS) $(WFLAGS) $(LIB) -c $(MODELS_DIR)/SpectrumComponent.cpp $(MODELS_DIR)/SpectrumComponent.hpp -o SpectrumComponent.o

# Compile the Python extension module
#spectrum_component.so: $(MODELS_DIR)/spectrum_component.o $(MODELS_DIR)/spectrum_component_bindings.o
#	$(CXX) $(WFLAGS) $(LIB) -o $@ $^

# Clean up
clean:
	rm -f $(MODELS_DIR)/*.o $(MODELS_DIR)/*.so
