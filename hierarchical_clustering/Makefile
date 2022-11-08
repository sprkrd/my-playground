all: hc hc-opt

hc: hc.cpp
	g++ -Wall -Werror -pedantic --std=c++17 -O3 -o hc hc.cpp

hc-opt: hc.cpp
	g++ -Wall -Werror -pedantic --std=c++17 -O3 -DOPTIMIZE_DJS -o hc-opt hc.cpp
