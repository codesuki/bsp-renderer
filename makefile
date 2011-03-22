all:
	g++ `sdl-config --cflags --libs` -lSDL_image -lSDL_ttf -lGL -lGLU *.cpp -o bsp-renderer

