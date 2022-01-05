comp=g++

comp_flags=-O3 -Wall -shared -std=c++17 -fPIC

include_dirs=-I ../extern/pybind11/include/pybind11/ \
	     -I /home/anvar/anaconda3/include/python3.9/ \
	     -I ./

defaults:
	${comp} ${comp_flags} $$(python3 -m pybind --include) fit.cpp -o fit$$(python3-config --extension-suffix) ${include_dirs}
