all:
	g++ main.cpp -Wall -O2 -o gen -std=c++11
	g++ csv.cpp -Wall -O2 -o csv -std=c++11
