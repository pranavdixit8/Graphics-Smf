
gui: gui.cpp

	g++ -std=c++11 -O0 -Wall -pedantic -I/usr/X11R6/include -I./ -I./../glui/include -o bin/gui ./gui.cpp  -L./../glui/lib -lglui -L/usr/X11R6/lib -lglut -lGLU -lGL -lXmu -lXext -lX11 -lXi -lm
	