# TOP Makefile

CC = g++
CXX_FLAGS = -g -Wall -pthread --std=c++11 `mysql_config --cflags --libs`

all: *.cc
	$(CC) $(CXX_FLAGS) *.cc -o stardns
