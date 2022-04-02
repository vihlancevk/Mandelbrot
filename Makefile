out:
	g++ -O3 -mavx -mavx2 -c Mandelbrot.cpp
	g++ Mandelbrot.o -o Mandelbrot -lsfml-graphics -lsfml-window -lsfml-system
	./Mandelbrot
