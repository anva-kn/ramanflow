comp=clang++

comp_flags=-O3 -Wall -shared -std=c++17 -fPIC

include_dirs=-I/usr/local/Cellar/2.9.0/include/pybind11/ \
			 -I/usr/local/Cellar/python@3.8/ \
			 -I/usr/local/Cellar/eigen/3.4.0_1/include/eigen3/ \
			-I./ \
			-I/usr/local/include \
			-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/

defaults:
	        ${comp} ${comp_flags} $$(python3 -m pybind --includes) poly1d.cpp fit.cpp -o fit$$(python3-config --extension-suffix) ${include_dirs}
