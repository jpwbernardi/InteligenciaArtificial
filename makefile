all:
	g++ CodigoFonte/main.cpp -Wall -O2 -o gen -std=c++11
	g++ CodigoFonte/csv.cpp -Wall -O2 -o csv -std=c++11

clear:
	rm gen csv
