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
endif
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
    WFLAGS = -O3 -Wall
endif

CFLAGS = -std=c++17
LDFLAGS = -ltiff
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
