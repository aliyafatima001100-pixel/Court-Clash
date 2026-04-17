CXX = g++
SRC = main.cpp
OUT = tennis.exe

all:
	g++ main.cpp -o tennis.exe -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

run:
	./tennis.exe

clean:
	del tennis.exe