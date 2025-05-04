##
# Makefile for Ramanflow project
#
# @version 0.1
# For the next time add compilation of models.

SHELL=/usr/bin/env sh

# Detect the operating system
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    CXX = g++
    WFLAGS = -O3 -Wall -shared
    PYTHON_INCLUDE = -I/usr/include/python3.x
    PYTHON_LIB = -lpython3.x
endif
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
    WFLAGS = -O3 -Wall
    PYTHON_INCLUDE = -I/usr/local/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/include/python3.13 -I/usr/local/lib/python3.13/site-packages/numpy/_core/include
    PYTHON_LIB = -lpython3.13
    PYTHON_LIB_PATH = -L/usr/local/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/lib/python3.13/config-3.13-darwin -lintl -ldl -framework CoreFoundation
endif

CFLAGS = -std=c++17 -I. $(PYTHON_INCLUDE)
LDFLAGS = -ltiff $(PYTHON_LIB_PATH) $(PYTHON_LIB)
DEBUG_FLAGS = -g -O0

.PHONY: all debug clean

# Default target
all: DataLoader

# DataLoader: DataLoader.cpp DataLoader.hpp
#    $(CXX) $(CFLAGS) $(WFLAGS) -o DataLoader DataLoader.cpp $(LDFLAGS)
DataLoader: DataLoader.cpp DataLoader.hpp tiff_inspector.cpp tiff_inspector.hpp
	$(CXX) $(CFLAGS) $(WFLAGS) -o DataLoader DataLoader.cpp tiff_inspector.cpp $(LDFLAGS)

debug: DataLoader.cpp DataLoader.hpp
	$(CXX) $(CFLAGS) $(DEBUG_FLAGS) -o DataLoader_debug DataLoader.cpp $(LDFLAGS)

clean:
	rm -f DataLoader DataLoader_debug
