all:
	g++ src/main.cpp -o tennis.exe -I. -Isrc -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

run:
	./tennis.exe

clean:
	del tennis.exe
