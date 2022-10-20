CPPFLAGS = -O3 -std=c++17 -Wall -Werror -pedantic

all: test_dlopen libhello_world.so

libhello_world.so: hello_world.cpp
	g++ $(CPPFLAGS) -shared -fpic hello_world.cpp -o libhello_world.so
	
test_dlopen: test_dlopen.cpp plugin.hpp
	g++ $(CPPFLAGS) test_dlopen.cpp -fpic -ldl -o test_dlopen
	
clean:
	rm -f test_dlopen *.so
