# Plain Makefile for anyone not using CLion:   make && ./serverconfig_demo ...
CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Werror -pedantic -O2
serverconfig_demo: demo_main.cpp ServerConfig.cpp *.h
	$(CXX) $(CXXFLAGS) -o serverconfig_demo demo_main.cpp ServerConfig.cpp
clean:
	rm -f serverconfig_demo *.o *.tga
