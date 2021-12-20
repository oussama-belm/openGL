g++ -Wall -g -I/usr/include -c /home/oussama/Desktop/freeglut/mayGame/first.cpp -o obj/Debug/first.o
g++ -L/usr/lib -o bin/Debug/mayGame obj/Debug/first.o   -lglut -lGL -lGLU -lXxf86vm
xterm -T mayGame -e /usr/bin/cb_console_runner LD_LIBRARY_PATH=:.:/usr/lib /home/oussama/Desktop/freeglut/mayGame/bin/Debug/mayGame 