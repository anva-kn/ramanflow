##
# Makefile for Ramanflow project (SpectralIO)
#
# Builds:
#   - libSpectralIO.a          (static library)
#   - libSpectralIO.so/.dylib  (shared library)
# Optional:
#   - spectral_demo            (if main.cpp is present)
#
# @version 0.2

SHELL := /usr/bin/env sh

# -------- OS detection --------
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    CXX := g++
    SO_EXT        := so
    SHARED_FLAGS  := -shared -Wl,-soname,libSpectralIO.$(SO_EXT)
    PYTHON_INCLUDE ?= -I/usr/include/python3.x
    PYTHON_LIB     ?= -lpython3.x
    PYTHON_LIB_PATH?=
endif

ifeq ($(UNAME_S),Darwin)
    CXX := clang++
    SO_EXT        := dylib
    SHARED_FLAGS  := -dynamiclib -Wl,-install_name,@rpath/libSpectralIO.$(SO_EXT)
    RPATH_FLAG    := -Wl,-rpath,@executable_path
    PYTHON_INCLUDE ?= -I/usr/local/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/include/python3.13 \
                      -I/usr/local/lib/python3.13/site-packages/numpy/_core/include
    PYTHON_LIB     ?= -lpython3.13
    PYTHON_LIB_PATH?= -L/usr/local/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/lib/python3.13/config-3.13-darwin \
                      -lintl -ldl -framework CoreFoundation
endif

# -------- Flags --------
# SANITIZE := -fsanitize=address -fno-omit-frame-pointer

CXXFLAGS  := -std=c++17 -O3 -Wall -fPIC $(SANITIZE)
CPPFLAGS  := -I. $(PYTHON_INCLUDE)
LDFLAGS   := $(PYTHON_LIB_PATH) $(SANITIZE)
LDLIBS    := -ltiff $(PYTHON_LIB)

DEBUG_FLAGS := -g -O0 $(SANITIZE)

# -------- Sources / Targets --------
# Replace these with your actual file names/paths as needed
LIB_NAME      := SpectralIO
LIB_STATIC    := lib$(LIB_NAME).a
LIB_SHARED    := lib$(LIB_NAME).$(SO_EXT)

LIB_SOURCES   := SpectralIO.cpp
LIB_HEADERS   := SpectralIO.hpp

# Optional plotting/object files can be added here if you want them in the lib:
LIB_SOURCES  += SpectralPlotter.cpp
LIB_HEADERS  += SpectralPlotter.hpp

LIB_SOURCES  += SpectralPreprocessor.cpp
LIB_HEADERS  += SpectralPreprocessor.hpp

LIB_OBJECTS   := $(LIB_SOURCES:.cpp=.o)

# Demo executable (optional) – only if you have main.cpp
DEMO          := spectral_demo
DEMO_SOURCES  := main.cpp
DEMO_OBJECTS  := $(DEMO_SOURCES:.cpp=.o)

.PHONY: all libs demo debug clean

# Default: just build the libraries
all: libs

libs: $(LIB_STATIC) $(LIB_SHARED)

# -------- Library builds --------
$(LIB_STATIC): $(LIB_OBJECTS)
	@echo "  AR    $@"
	@ar rcs $@ $^

$(LIB_SHARED): $(LIB_OBJECTS)
	@echo "  LD    $@"
	@$(CXX) $(CXXFLAGS) $(SHARED_FLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# -------- Objects --------
%.o: %.cpp $(LIB_HEADERS)
	@echo "  CXX   $<"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# -------- Demo (optional) --------
# Build with: `make demo` (requires main.cpp present)
demo: $(DEMO)

$(DEMO): $(DEMO_OBJECTS) $(LIB_STATIC)
	@echo "  LD    $@"
	@$(CXX) $(CXXFLAGS) $(RPATH_FLAG) -o $@ $(DEMO_OBJECTS) -L. -l$(LIB_NAME) $(LDFLAGS) $(LDLIBS)

# Debug build of libraries + demo
debug: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(DEBUG_FLAGS)" all

# -------- Clean --------
clean:
	@echo "  CLEAN"
	@rm -f $(LIB_OBJECTS) $(DEMO_OBJECTS) $(LIB_STATIC) $(LIB_SHARED) $(DEMO)
