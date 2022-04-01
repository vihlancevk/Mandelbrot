out:
	g++ -c Mandelbrot.cpp
	g++ Mandelbrot.o -o Mandelbrot -lsfml-graphics -lsfml-window -lsfml-system
	./Mandelbrot
