comp=g++

comp_flags=-O3 -Wall -shared -std=c++17 -fPIC

include_dirs=-I/usr/include/pybind11/ \
		-I/usr/include/python3.8/ \
	       -I./

defaults:
	${comp} ${comp_flags} $$(python3 -m pybind --includes) poly1d.cpp fit.cpp -o fit$$(python3-config --extension-suffix) ${include_dirs}
