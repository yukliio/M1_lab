# Plain Makefile for anyone not using CLion:   make && ./loganalyzer ...
CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Werror -pedantic -O2
loganalyzer: main.cpp LogEntry.cpp Player.cpp ServerLog.cpp *.h
	$(CXX) $(CXXFLAGS) -o loganalyzer main.cpp LogEntry.cpp Player.cpp ServerLog.cpp
clean:
	rm -f loganalyzer *.o *.tga
