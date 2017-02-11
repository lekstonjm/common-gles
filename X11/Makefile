all: simple_triangle

simple_triangle:../src/Renderer.cpp ../src/Renderer.h main.cpp
	g++  -std=c++11 -Wall -Wextra -pedantic main.cpp ../src/Renderer.cpp   -I../src -I/home/jm/glm -lX11 -lEGL -lGLESv2 -o simple_triangle

.PHONY: clean

clean:_
	rm -f *.o  simple_triangle
