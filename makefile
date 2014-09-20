osx:
	GYP_GENERATORS=ninja gyp bsp-loader.gyp --toplevel-dir=`pwd` --depth=0
	ninja -C out/Default/ osx

clean:
	ninja -C out/Default/ -t clean
