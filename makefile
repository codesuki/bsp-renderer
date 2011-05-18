all:
	g++ `sdl-config --cflags --libs` -lSDL_image -lGL -lGLU *.cpp -o bsp-renderer 

new: 
	g++ `thirdparty/bin/sdl-config --cflags --libs` -I/home/neri/Desktop/bsp-loader-ogl/thirdparty/include/ -L/home/neri/Desktop/bsp-loader-ogl/thirdparty/lib64 -lSDL_image -lGL -lGLU *.cpp *.c -o bsp-renderer
